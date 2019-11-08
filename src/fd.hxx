#ifndef FD__HXX
#define FD__HXX

#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <utility>
#include <memory>
#include "mappedFile.hxx"

#ifdef _MSVC
#define O_NOCTTY _O_BINARY
#endif

#ifndef WARN_UNUSED
#ifdef __GNUC__
#define WARN_UNUSED		__attribute__((warn_unused_result))
#else
#define WARN_UNUSED
#endif
#endif /*WARN_UNUSED*/

/*!
 * @internal
 * While this is supposed to be a very thin, RAII-only layer,
 * between a file descriptor and the code that uses it, due to the need to know
 * EOF outside of just read() calls, this also holds a flag for that express purpose.
 */
struct fd_t final
{
private:
	int32_t fd;
	mutable bool eof;

public:
	constexpr fd_t() noexcept : fd(-1), eof(false) { }
	constexpr fd_t(const int32_t file) noexcept : fd(file), eof(false) { }
	fd_t(const char *const file, int flags, mode_t mode = 0) noexcept : fd(::open(file, flags, mode)), eof(false) { }
	fd_t(fd_t &&file) noexcept : fd_t() { swap(file); }
	~fd_t() noexcept { if (fd != -1) close(fd); }
	void operator =(fd_t &&file) noexcept { swap(file); }

	operator int32_t() const noexcept WARN_UNUSED { return fd; }
	bool operator ==(const int32_t desc) const noexcept WARN_UNUSED { return fd == desc; }
	bool valid() const noexcept WARN_UNUSED { return fd != -1; }
	bool isEOF() const noexcept WARN_UNUSED { return eof; }
	void swap(fd_t &file) noexcept
	{
		std::swap(fd, file.fd);
		std::swap(eof, file.eof);
	}

	template<typename T> bool read(T &value) const noexcept
		{ return read(&value, sizeof(T)); }
	template<typename T> bool read(const std::unique_ptr<T> &value, const size_t valueLen) const noexcept
		{ return read(value.get(), valueLen); }
	/*bool read(const managedPtr_t<void> &value, const size_t valueLen) const noexcept
		{ return read(value.get(), valueLen); }*/

	template<typename T, size_t N> bool read(std::array<T, N> &value, const size_t count_ = N) const noexcept
	{
		const size_t count = count_ < N ? count_ : N;
		bool result = true;
		for (size_t i = 0; i < count; ++i)
			result &= read(value[i]);
		return result;
	}

	template<typename T> bool write(T &value) const noexcept
		{ return write(&value, sizeof(T)); }
	template<typename T> bool write(const std::unique_ptr<T> &value, const size_t valueLen) const noexcept
		{ return write(value.get(), valueLen); }

	template<typename T, size_t N> bool write(const std::array<T, N> &value, const size_t count_ = N) const noexcept
	{
		const size_t count = count_ < N ? count_ : N;
		bool result = true;
		for (size_t i = 0; i < count; ++i)
			result &= write(value[i]);
		return result;
	}

	bool read(void *const value, const size_t valueLen) const noexcept WARN_UNUSED
	{
		size_t actualLen;
		return read(value, valueLen, actualLen);
	}

	bool read(void *const value, const size_t valueLen, size_t &actualLen) const noexcept WARN_UNUSED
	{
		actualLen = 0;
		if (eof)
			return 0;
		const ssize_t result = ::read(fd, value, valueLen);
		if (result == 0 && valueLen != 0)
			eof = true;
		else if (result > 0)
			actualLen = size_t(result);
		return actualLen == valueLen;
	}
	ssize_t write(const void *const bufferPtr, const size_t len) const noexcept { return ::write(fd, bufferPtr, len); }
	off_t seek(off_t offset, int32_t whence) const noexcept WARN_UNUSED { return ::lseek(fd, offset, whence); }
	off_t tell() const noexcept WARN_UNUSED { return seek(0, SEEK_CUR); }

	off_t length() const noexcept WARN_UNUSED
	{
		struct stat fileStat;
		if (!fstat(fd, &fileStat))
			return fileStat.st_size;
		return 0;
	}

	mappedFile_t map(const int32_t prot) noexcept WARN_UNUSED
		{ return map(prot, length()); }

	mappedFile_t map(const int32_t prot, const off_t len) noexcept WARN_UNUSED
	{
		// If we don't represent a valid file desriptor, don't return a valid mappedFile_t,
		// but rather invoke it's default constructor.
		if (!valid())
			return {};
		int32_t file = std::exchange(fd, -1);
		return {file, len, prot};
	}

	fd_t(const fd_t &) = delete;
	fd_t &operator =(const fd_t &) = delete;
};

#endif /*FD__HXX*/
