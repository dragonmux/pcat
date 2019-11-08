#ifndef MAPPED_FILE__HXX
#define MAPPED_FILE__HXX

#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdexcept>

#ifndef WARN_UNUSED
#ifdef __GNUC__
#define WARN_UNUSED		__attribute__((warn_unused_result))
#else
#define WARN_UNUSED
#endif
#endif /*WARN_UNUSED*/

struct mappedFile_t final
{
private:
	int32_t fd;
	void *addr;
	off_t len;

	template<typename T> T &index(const off_t idx) const
	{
		if (idx < len)
		{
			const uintptr_t _addr = uintptr_t(addr) + idx;
			const auto ptr = reinterpret_cast<void *>(_addr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast) lgtm[cpp/reinterpret-cast]
			return *static_cast<T *>(ptr);
		}
		throw std::out_of_range("index out of range in mappedFile_t");
	}

public:
	constexpr mappedFile_t() noexcept : fd(-1), addr(nullptr), len(0) { }
	mappedFile_t(const int32_t file, const off_t length, const int32_t prot) noexcept : fd(file),
			addr(mmap(nullptr, length, prot, MAP_SHARED, fd, 0)), len(length)
		{ if (addr == MAP_FAILED) addr = nullptr; }
	mappedFile_t(mappedFile_t &&file) noexcept : mappedFile_t() { swap(file); }
	~mappedFile_t() noexcept { if (fd != -1) close(fd); munmap(addr, len); }
	void operator =(mappedFile_t &&file) noexcept { swap(file); }

	bool valid() const noexcept WARN_UNUSED { return fd != -1 && addr; }
	void swap(mappedFile_t &file) noexcept
	{
		std::swap(fd, file.fd);
		std::swap(addr, file.addr);
		std::swap(len, file.len);
	}

	template<typename T> T *address() noexcept { return static_cast<T *>(addr); }
	template<typename T> const T *address() const noexcept { return static_cast<T *const>(addr); }
	off_t length() const noexcept WARN_UNUSED { return len; }
	template<typename T> T &operator [](const off_t idx) { return index<T>(idx); }
	template<typename T> const T &operator [](const off_t idx) const { return index<const T>(idx); }
	template<typename T> T &at(const off_t idx) { return index<T>(idx); }
	template<typename T> const T &at(const off_t idx) const { return index<const T>(idx); }

	bool operator ==(const mappedFile_t &b) const noexcept { return fd == b.fd && addr == b.addr && len == b.len; }
	bool operator !=(const mappedFile_t &b) const noexcept { return !(*this == b); }
};

#endif /*MAPPED_FILE__HXX*/
