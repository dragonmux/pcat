#include <cerrno>
#include <string_view>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"
#include "threadPool.hxx"
#include "fileChunker.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat
{
	int32_t copyChunk(chunkState_t chunk)
	{
		const auto &outputOffset = chunk.outputOffset();
		[[maybe_unused]] const auto outputLength{outputOffset.length()};
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
		while (!chunk.atEnd())
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
			assert(offset <= outputLength);
			++chunk;
		}

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
