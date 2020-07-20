#include <string_view>
#include <substrate/console>
#include "copyChunk.hxx"
#include "threadPool.hxx"
#include "algorithm/blockLinear/fileChunker.hxx"

using namespace std::literals::string_view_literals;
using substrate::console;

namespace pcat::algorithm::blockLinear
{
	int32_t chunkedCopy() noexcept try
	{
		threadPool_t copyThreads{copyChunk<chunkState_t>};
		fileChunker_t chunker{};
		assert(copyThreads.ready());

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
} // namespace pcat::algorithm::blockLinear
