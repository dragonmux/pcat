#include <cassert>
#include <string_view>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"
#include "threadPool.hxx"
#include "mappingOffset.hxx"
#include "copyChunk.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat::algorithm::chunkSpans
{
	struct chunkState_t final
	{
	private:
		inputFilesIterator_t file_{};
		off_t inputLength_{};
		mappingOffset_t inputOffset_{};
		mappingOffset_t outputOffset_{};

		constexpr void nextInputBlock() noexcept
		{
			inputOffset_ += inputOffset_.length();
			if (inputOffset_.offset() == inputLength_)
			{
				assert(file_ != inputFiles.end()); // NOLINT
				++file_;
				inputLength_ = file_ == inputFiles.end() ? 0 : file_->length();
				inputOffset_ = {};
			}
		};

	public:
		chunkState_t() noexcept = default;
		constexpr chunkState_t(const inputFilesIterator_t &file, const off_t inputLength,
			const mappingOffset_t &inputOffset, const mappingOffset_t &outputOffset) noexcept :
			file_{file}, inputLength_{inputLength}, inputOffset_{inputOffset}, outputOffset_{outputOffset} { }
		constexpr chunkState_t(const chunkState_t &) noexcept = default;
		constexpr chunkState_t(chunkState_t &&) noexcept = default;
		chunkState_t &operator =(const chunkState_t &) noexcept = default;
		chunkState_t &operator =(chunkState_t &&) noexcept = default;
		~chunkState_t() noexcept = default;
		[[nodiscard]] constexpr const inputFilesIterator_t &file() const noexcept { return file_; }
		[[nodiscard]] constexpr off_t inputLength() const noexcept { return inputLength_; }
		[[nodiscard]] const fd_t &inputFile() const noexcept { return *file_; }
		[[nodiscard]] constexpr const mappingOffset_t &inputOffset() const noexcept { return inputOffset_; }
		[[nodiscard]] constexpr const mappingOffset_t &outputOffset() const noexcept { return outputOffset_; }

		[[nodiscard]] constexpr chunkState_t end() const noexcept
		{
			chunkState_t state{*this};
			while (!state.atEnd())
				++state;
			return state;
		}

		[[nodiscard]] constexpr bool atEnd() const noexcept
			{ return !outputOffset_.length(); }

		constexpr void operator ++() noexcept
		{
			if (atEnd())
				return;
			const off_t remainder = outputOffset_.length() - inputOffset_.length();
			outputOffset_ += inputOffset_.length();
			outputOffset_.length(remainder);
			nextInputBlock();
			inputOffset_.length(std::min(transferBlockSize, std::min(remainder, inputLength_)));
		}

		bool operator ==(const chunkState_t &other) const noexcept
		{
			return file_ == other.file_ &&
				inputOffset_ == other.inputOffset_ &&
				outputOffset_ == other.outputOffset_;
		}
		bool operator !=(const chunkState_t &other) const noexcept { return !(*this == other); }
	};

	struct chunking_t final
	{
	private:
		std::size_t spanLength;
		inputFilesIterator_t file{inputFiles.begin()};
		off_t inputLength{file == inputFiles.end() ? 0 : file->length()};
		mappingOffset_t inputOffset{0, blockLength(inputLength)};
		const off_t outputLength{outputFile.length()};
		mappingOffset_t outputOffset{};

		constexpr off_t spanOf(const off_t length)
			{ return std::min(off_t(spanLength), length); }

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
			inputOffset.length(blockLength(inputLength - inputOffset));
		};

	public:
		chunking_t(const std::size_t spanLength_) noexcept : spanLength{spanLength_}
			{ outputOffset.length(spanOf(outputLength - outputOffset)); }
		chunking_t(const std::size_t spanLength_, const inputFilesIterator_t file_) noexcept :
			spanLength{spanLength_}, file{file_}, outputOffset{outputLength} { }
		[[nodiscard]] constexpr chunkState_t subchunkState() const noexcept
			{ return {file, inputLength, inputOffset, outputOffset}; }
		chunkState_t operator *() const noexcept { return subchunkState(); }

		constexpr void operator ++() noexcept
		{
			if (outputOffset == outputLength)
				return;
			else if (outputOffset.length() != inputOffset.length())
			{
				const auto state{subchunkState().end()};
				file = state.file();
				inputLength = state.inputLength();
				inputOffset = state.inputOffset();
			}
			if (file != inputFiles.end())
				nextInputBlock();
			outputOffset += outputOffset.length();
			if (outputOffset == outputLength)
				outputOffset.length(0);
			else if (outputLength - outputOffset - spanLength < spanLength)
				outputOffset.length(outputLength - outputOffset);
			else
				outputOffset.length(spanLength);
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
	private:
		std::size_t spanLength_;

	public:
		constexpr fileChunker_t(const std::size_t spanLength) : spanLength_{spanLength} { }

		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		[[nodiscard]] chunking_t begin() const noexcept { return {spanLength_}; }
		// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
		[[nodiscard]] chunking_t end() const noexcept { return {spanLength_, inputFiles.end()}; }
	};

	/*!
	 * The idea with this is that, we take the total length of the file,
	 * the number of threads and divide them together if there are more than
	 * (transferBlockSize * threads) bytes to move, otherwise splitting
	 * the blocks up into transferBlockSize chunks.
	 *
	 * The result should be up to threads chunks which are then scheduled
	 * off one onto each thread, and copied linearly within that thread.
	 * The goal is to create a mostly linear access order for sub-chunks
	 * of the input files to reduce IO Wait issues on Lustre + GPFS, and
	 * significantly boost the speed of copying in that way.
	 */

	template<typename int_t> constexpr inline std::make_unsigned_t<int_t>
		asUnsigned(int_t value) noexcept { return value; }

	int32_t chunkedCopy() noexcept try
	{
		const auto length{asUnsigned(outputFile.length())};
		threadPool_t copyThreads{copyChunk<chunkState_t>};
		assert(copyThreads.ready());

		if ((transferBlockSize * copyThreads.numProcessors()) >= length)
		{
			console.info("Using the short file form of the algorithm"sv);
			return 0;
		}

		const auto chunksPerSpan{outputFile.length() / (transferBlockSize * copyThreads.numProcessors())};
		fileChunker_t chunker{chunksPerSpan * transferBlockSize};

		for (const chunkState_t &chunk : chunker)
		{
			if (const auto result{copyThreads.queue(chunk)}; result)
			{
				console.error("Copying failed: "sv, std::strerror(result));
				return result;
			}
		}
		return copyThreads.finish();
	}
	catch (std::system_error &error)
	{
		console.error("Copying failed: "sv, error.what());
		return error.code().value();
	}
} // namespace pcat::algorithm::chunkSpans
