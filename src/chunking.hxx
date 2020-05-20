#ifndef CHUNKING__HXX
#define CHUNKING__HXX

#include <vector>
#include <substrate/fd>
#include <substrate/units>

namespace pcat
{
	using substrate::fd_t;
	using substrate::operator ""_KiB;

	constexpr static size_t pageSize = 4_KiB;
	constexpr static size_t transferBlockSize = 64 * pageSize;
	extern std::vector<fd_t> inputFiles;
	extern fd_t outputFile;

	extern void calculateInputChunking() noexcept;
}

#endif /*CHUNKING__HXX*/
