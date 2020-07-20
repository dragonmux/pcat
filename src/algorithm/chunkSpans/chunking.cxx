#include <string_view>
#include <substrate/console>
#include "copyChunk.hxx"
#include "threadPool.hxx"
#include "algorithm/chunkSpans/fileChunker.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat::algorithm::chunkSpans
{
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
