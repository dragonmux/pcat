#ifndef CHUNKING__HXX
#define CHUNKING__HXX

#include <vector>
#include <sys/types.h>
#include <substrate/fd>
#include <substrate/units>

namespace pcat
{
	using substrate::fd_t;
	using substrate::operator ""_KiB;
	using substrate::operator ""_MiB;

	constexpr static off_t pageSize = 4_KiB;
	constexpr static off_t transferBlockSize = 1_MiB;
	extern std::vector<fd_t> inputFiles;
	extern fd_t outputFile;

	extern int32_t chunkedCopy() noexcept;
}

#endif /*CHUNKING__HXX*/
