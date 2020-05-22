#include <cassert>
#include <cerrno>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"

using substrate::console;

namespace pcat
{
	struct mappingOffset_t final
	{
	private:
		off_t offset_{0};
		off_t adjust_{0};
		off_t length_{0};

		constexpr void recalcAdjustment()
		{
			adjust_ = offset_ % pageSize;
			offset_ -= adjust_;
		}

	public:
		constexpr mappingOffset_t() noexcept = default;
		constexpr mappingOffset_t(const off_t offset) noexcept : offset_{offset} { recalcAdjustment(); }
		constexpr mappingOffset_t(const off_t offset, const off_t length) noexcept :
			offset_{offset}, length_{length} { recalcAdjustment(); }
		[[nodiscard]] constexpr off_t offset() const noexcept { return offset_ + adjust_; }
		[[nodiscard]] constexpr off_t adjustment() const noexcept { return adjust_; }
		[[nodiscard]] constexpr off_t length() const noexcept { return length_; }
		[[nodiscard]] constexpr off_t adjustedOffset() const noexcept { return offset_; }
		[[nodiscard]] constexpr off_t adjustedLength() const noexcept { return length_ + adjust_; }
		constexpr void length(const off_t length) noexcept { length_ = length; }
		constexpr operator off_t() const noexcept { return offset(); }

		constexpr mappingOffset_t &operator +=(const off_t amount) noexcept
		{
			offset_ += amount + adjust_;
			recalcAdjustment();
			return *this;
		}
	};

	constexpr off_t blockLength(const off_t length)
		{ return std::min(transferBlockSize, length); }

	using inputFilesIterator_t = typename decltype(inputFiles)::iterator;

	struct chunkState_t final
	{
	private:
		const fd_t &inputFile_;
		mappingOffset_t inputOffset_;
		mappingOffset_t outputOffset_;

	public:
		constexpr chunkState_t(const fd_t &inputFile, const mappingOffset_t &inputOffset,
			const mappingOffset_t &outputOffset) noexcept : inputFile_{inputFile},
			inputOffset_{inputOffset}, outputOffset_{outputOffset} { }
		[[nodiscard]] constexpr const fd_t &inputFile() const noexcept { return inputFile_; }
		[[nodiscard]] constexpr mappingOffset_t inputOffset() const noexcept { return inputOffset_; }
		[[nodiscard]] constexpr mappingOffset_t outputOffset() const noexcept { return outputOffset_; }
	};

	struct chunking_t final
	{
	private:
		inputFilesIterator_t file{inputFiles.begin()};
		off_t inputLength{file->length()};
		mappingOffset_t inputOffset{0, blockLength(inputLength)};
		const off_t outputLength{outputFile.length()};
		mappingOffset_t outputOffset{};

		constexpr void nextInputBlock() noexcept
		{
			inputOffset += inputOffset.length();
			if (inputOffset.offset() == inputLength)
			{
				assert(file < inputFiles.end()); // NOLINT
				++file;
				inputLength = file == inputFiles.end() ? 0 : file->length();
				inputOffset = {};
			}
		};

	public:
		chunking_t() noexcept { outputOffset.length(blockLength(outputLength - outputOffset)); }
		chunking_t(const inputFilesIterator_t file_) noexcept : file{file_}, inputLength{0}, inputOffset{},
			outputOffset{outputLength} { }
		chunkState_t operator *() const noexcept { return {*file, inputOffset, outputOffset}; }

		constexpr void operator ++() noexcept
		{
			if (outputOffset == outputLength)
				return;
			else if (outputOffset.length() != inputOffset.length())
			{
				const auto originalOutputOffset = outputOffset;
				while (outputOffset.length() - inputOffset.length())
				{
					const off_t remainder = outputOffset.length() - inputOffset.length();
					console.info("Transfer caused a remainder of ", remainder, " bytes to go for output block");
					outputOffset += inputOffset.length();
					outputOffset.length(remainder);
					nextInputBlock();
					inputOffset.length(std::min(remainder, inputLength));

					console.info("Copying ", inputOffset.length(), " bytes at ", inputOffset.offset(),
						" to ", outputOffset.length(), " byte region at ", outputOffset.offset());
				}
				outputOffset = originalOutputOffset;
			}
			nextInputBlock();
			inputOffset.length(blockLength(inputLength - inputOffset));
			outputOffset += outputOffset.length();
			outputOffset.length(blockLength(outputLength - outputOffset));
		}

		bool operator ==(const chunking_t &other) const noexcept
		{
			return file == other.file &&
				inputOffset == other.inputOffset &&
				outputOffset == other.outputOffset;
		}
		bool operator !=(const chunking_t &other) const noexcept { return !(*this == other); }
	};

	// begin() and end() are non-static to fufill the requirements of for-each looping
	struct fileChunker_t final
	{
		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		chunking_t begin() noexcept { return {}; }
		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		chunking_t end() noexcept { return {inputFiles.end()}; }
	};

	int32_t chunkedCopy() noexcept
	{
		fileChunker_t chunker{};
		for (chunkState_t chunk : chunker)
		{
			const auto &inputFile = chunk.inputFile();
			const auto inputOffset = chunk.inputOffset();
			const auto outputOffset = chunk.outputOffset();
			console.info("Copying ", inputOffset.length(), " bytes at ", inputOffset.offset(),
				" to ", outputOffset.length(), " byte region at ", outputOffset.offset());

			mmap_t inputChunk{inputFile, inputOffset.adjustedOffset(),
				inputOffset.adjustedLength(), PROT_READ, MAP_PRIVATE | MAP_POPULATE};
			if (!inputChunk.valid())
			{
				const auto error = errno;
				console.error("Failed to map source file transfer chunk: ", std::strerror(error));
				return error;
			}

			mmap_t outputChunk{outputFile, outputOffset.adjustedOffset(),
				outputOffset.adjustedLength(), PROT_WRITE, MAP_PRIVATE};
			if (!outputChunk.valid())
			{
				const auto error = errno;
				console.error("Failed to map destination file transfer chunk: ", std::strerror(error));
				return error;
			}

			try
			{
				outputChunk.copyTo(
					outputOffset.adjustment(),
					inputChunk.address(inputOffset.adjustment()),
					inputOffset.length()
				);
			}
			catch (const std::out_of_range &error)
			{
				console.error("Failure while copying data block: ", error.what());
				return EINVAL;
			}

			if (!outputChunk.sync())
			{
				const auto error = errno;
				console.error("Failed to synchronise the mapping for region ", outputOffset.offset(),
					':', outputOffset.length(), " at address ", outputChunk.address(0));
				console.error("Failure reason: ", std::strerror(error));
				return error;
			}
		}
		return 0;
	}
} // namespace pcat
