#include <substrate/console>
#include "chunking.hxx"
#include "mmap.hxx"

using substrate::console;

namespace pcat
{
	void calculateInputChunking() noexcept
	{
		for (const auto &file : inputFiles)
		{
			const size_t length = file.length();
			for (size_t offset = 0; offset < length; offset += transferBlockSize)
			{
				const size_t amount = std::min(length - offset, transferBlockSize);
				console.info("Copying ", amount, " bytes from ", int32_t{file},
					" starting at offset ", offset);
			}
		}
	}
}
