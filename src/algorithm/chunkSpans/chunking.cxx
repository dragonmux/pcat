#include <string_view>
#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"
#include "threadPool.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat::algorithm::chunkSpans
{
	struct chunkState_t final
	{
	private:

	public:
	};

	struct chunking_t final
	{
	private:

	public:
	};

	int32_t copyChunk(chunkState_t chunk)
	{
	}

	int32_t chunkedCopy() noexcept try
	{
#if 0
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
#endif
	}
	catch (std::system_error &error)
	{
		console.error("Copying failed: "sv, error.what());
		return error.code().value();
	}
} // namespace pcat::algorithm::chunkSpans
