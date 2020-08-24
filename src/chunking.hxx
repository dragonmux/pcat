#ifndef CHUNKING__HXX
#define CHUNKING__HXX

#include <vector>
#include <atomic>
#include <sys/types.h>
#include <substrate/fd>
#include <substrate/units>

namespace pcat
{
	using substrate::fd_t;
	using substrate::off_t;
	using substrate::operator ""_KiB;
	using substrate::operator ""_MiB;

#ifndef _WINDOWS
	constexpr static auto pageSize{off_t(4_KiB)};
#else
	constexpr static auto pageSize{off_t(64_KiB)};
#endif
	constexpr static auto transferBlockSize{off_t(1_MiB)};
	extern std::vector<fd_t> inputFiles;
	extern fd_t outputFile;
	extern std::atomic<bool> sync;

	constexpr off_t blockLength(const off_t length)
		{ return std::min(transferBlockSize, length); }

	using inputFilesIterator_t = typename decltype(inputFiles)::iterator;

	namespace algorithm
	{
		namespace blockLinear { extern int32_t chunkedCopy() noexcept; }
		namespace chunkSpans { extern int32_t chunkedCopy() noexcept; }
	}
} // namespace pcat

#endif /*CHUNKING__HXX*/
