#include <cassert>
#include <cerrno>
#include <string_view>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"
#include "threadPool.hxx"
#include "chunkState.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat
{
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

	int32_t copyChunk(chunkState_t chunk)
	{
		const auto &outputOffset = chunk.outputOffset();
		const auto outputLength{outputOffset.length()};
		const mmap_t outputChunk{outputFile, outputOffset.adjustedOffset(),
			outputOffset.adjustedLength(), PROT_WRITE};
		if (!outputChunk.valid())
		{
			const auto error = errno;
			console.error("Failed to map destination file transfer chunk: "sv, std::strerror(error));
			return error;
		}
		else if (!outputChunk.advise<MADV_SEQUENTIAL, MADV_DONTDUMP>())
		{
			const auto error = errno;
			console.error("Failed to advise the source map: "sv, std::strerror(error));
			return error;
		}

		auto offset{outputOffset.adjustment()};
		do
		{
			const auto &inputFile = chunk.inputFile();
			const auto &inputOffset = chunk.inputOffset();
			const mmap_t inputChunk{inputFile, inputOffset.adjustedOffset(),
				inputOffset.adjustedLength(), PROT_READ, MAP_PRIVATE};
			if (!inputChunk.valid())
			{
				const auto error = errno;
				console.error("Failed to map source file transfer chunk: "sv, std::strerror(error));
				return error;
			}
			else if (!inputChunk.advise<MADV_SEQUENTIAL, MADV_WILLNEED, MADV_DONTDUMP>())
			{
				const auto error = errno;
				console.error("Failed to advise the source map: "sv, std::strerror(error));
				return error;
			}

			try
			{
				outputChunk.copyTo(
					offset,
					inputChunk.address(inputOffset.adjustment()),
					inputOffset.length()
				);
			}
			catch (const std::out_of_range &error)
			{
				console.error("Failure while copying data block: "sv, error.what());
				return EINVAL;
			}
			offset += inputOffset.length();
			++chunk;
		}
		while (offset < outputLength);

		if (!outputChunk.sync())
		{
			const auto error = errno;
			console.error("Failed to synchronise the mapping for region "sv, outputOffset.offset(),
				':', outputOffset.length(), " at address "sv, outputChunk.address(0));
			console.error("Failure reason: "sv, std::strerror(error));
			return error;
		}
		return 0;
	}

	int32_t chunkedCopy() noexcept try
	{
		threadPool_t copyThreads{copyChunk};
		fileChunker_t chunker{};
		assert(copyThreads.ready());

		for (const chunkState_t &chunk : chunker)
		{
			const int32_t result{copyThreads.queue(chunk)};
			if (result)
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
} // namespace pcat
