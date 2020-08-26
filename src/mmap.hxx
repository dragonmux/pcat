#ifndef MMAP__HXX
#define MMAP__HXX

#include <cstdint>
#ifndef _WINDOWS
#	include <unistd.h>
#	include <sys/mman.h>
#else
#	include <io.h>
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	undef WIN32_LEAN_AND_MEAN
#	undef min
#	undef max
#endif
#include <stdexcept>
#include <cstring>
#include <cassert>
#include <substrate/fd>

namespace pcat
{
	using substrate::off_t;

#ifdef _WINDOWS
	const DWORD PROT_READ{PAGE_READONLY};
	const DWORD PROT_WRITE{PAGE_READWRITE};
	const int32_t MAP_PRIVATE{0};

	const auto MADV_SEQUENTIAL{0};
	const auto MADV_WILLNEED{0};
	const auto MADV_DONTDUMP{0};
#endif

	struct mmap_t final
	{
	private:
		off_t _len{0};
#ifdef _WINDOWS
		HANDLE _mapping{INVALID_HANDLE_VALUE};
#endif
		void *_addr{nullptr};

		[[nodiscard]] void *index(const off_t idx) const
		{
			if (idx < _len)
			{
				// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
				const auto addr = reinterpret_cast<std::uintptr_t>(_addr); // lgtm[cpp/reinterpret-cast]
				// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
				return reinterpret_cast<void *>(addr + idx); // lgtm[cpp/reinterpret-cast]
			}
			throw std::out_of_range("mmap_t index out of range");
		}

#ifdef _WINDOWS
		constexpr static DWORD cleanProt(const DWORD prot) noexcept
		{
			if (prot & PAGE_READWRITE)
				return prot & ~PAGE_READONLY;
			return prot;
		}

		constexpr static DWORD protToAccess(const DWORD prot) noexcept
		{
			if (prot & PAGE_READWRITE)
				return FILE_MAP_WRITE;
			else if (prot & PAGE_READONLY)
				return FILE_MAP_READ;
			else if (prot & PAGE_WRITECOPY)
				return FILE_MAP_WRITE;
			return {};
		}
#endif

		using fd_t = substrate::fd_t;

	public:
		constexpr mmap_t() noexcept = default;
#ifndef _WINDOWS
		mmap_t(const fd_t &fd, const off_t len, const int32_t prot, const int32_t flags = MAP_SHARED,
			void *addr = nullptr) noexcept : _len{len}, _addr{[&]() noexcept -> void *
			{
				auto *const ptr = ::mmap(addr, len, prot, flags, fd, 0);
				// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
				return ptr == MAP_FAILED ? nullptr : ptr;
			}()} { }

		mmap_t(const fd_t &fd, const off_t offset, const off_t length, const int32_t prot,
			const int32_t flags = MAP_SHARED, void *addr = nullptr) noexcept : _len{length},
			_addr{[&]() noexcept -> void *
			{
				auto *const ptr = ::mmap(addr, length, prot, flags, fd, offset);
				// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
				return ptr == MAP_FAILED ? nullptr : ptr;
			}()} { }

		~mmap_t() noexcept
		{
			if (_addr)
				::munmap(_addr, _len);
		}

		[[nodiscard]] constexpr bool valid() const noexcept { return _addr; }
#else
		mmap_t(const fd_t &fd, const off_t len, const DWORD prot, const int32_t = 0,
			void * = nullptr) noexcept : _len{len}, _mapping{[&]() noexcept -> HANDLE
			{
				const auto file = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
				static_assert(sizeof(DWORD) == 4);
				return CreateFileMappingA(file, nullptr, cleanProt(prot), DWORD(len >> 32U),
					DWORD(len), nullptr);
			}()}, _addr{[&]() noexcept -> void *
			{
				if (!_mapping)
					return nullptr;
				return MapViewOfFile(_mapping, protToAccess(prot), 0, 0, 0);
			}()} { }

		mmap_t(const fd_t &fd, const off_t offset, const off_t length, const DWORD prot,
			const int32_t = 0, void * = nullptr) noexcept : _len{length},
			_mapping{[&]() noexcept -> HANDLE
			{
				const auto file = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
				static_assert(sizeof(DWORD) == 4);
#if 0
				return CreateFileMappingA(file, nullptr, cleanProt(prot), DWORD(length >> 32U),
					DWORD(length), nullptr);
#else
				return CreateFileMappingA(file, nullptr, cleanProt(prot), 0, 0, nullptr);
#endif
			}()}, _addr{[&]() noexcept -> void *
			{
				if (!_mapping)
					return nullptr;
#if 0
				return MapViewOfFile(_mapping, protToAccess(prot), DWORD(offset >> 32U), DWORD(offset), 0);
#else
				if (DWORD(length >> 32U))
					return nullptr;
				return MapViewOfFile(_mapping, protToAccess(prot), DWORD(offset >> 32U), DWORD(offset), length);
#endif
			}()} { }

		~mmap_t() noexcept
		{
			if (_addr)
				UnmapViewOfFile(_addr);
			if (_mapping)
				CloseHandle(_mapping);
		}

		[[nodiscard]] constexpr bool valid() const noexcept { return _mapping && _addr; }
#endif
		mmap_t(const mmap_t &) = delete;
		mmap_t(mmap_t &&map) = delete;
		mmap_t &operator =(const mmap_t &) = delete;
		mmap_t &operator =(mmap_t &&map) = delete;

		void swap(mmap_t &map) noexcept
		{
			std::swap(_addr, map._addr);
			std::swap(_len, map._len);
		}

		[[nodiscard]] constexpr off_t length() const noexcept { return _len; }
		[[nodiscard]] void *address(const off_t offset) { return index(offset); }
		[[nodiscard]] const void *address(const off_t offset) const { return index(offset); }

#ifndef _WINDOWS
		[[nodiscard]] bool sync(const int32_t flags = MS_SYNC | MS_INVALIDATE) const noexcept
			{ return sync(_len, flags); }

		[[nodiscard]] bool sync(const off_t length, const int32_t flags = MS_SYNC | MS_INVALIDATE) const noexcept
			{ return msync(_addr, length, flags) == 0; }

		[[nodiscard]] bool advise(const int32_t adviceFlags) const noexcept
			{ return madvise(_addr, _len, adviceFlags) == 0; }
#else
		[[nodiscard]] bool sync() const noexcept { return sync(_len); }
		[[nodiscard]] bool sync(const off_t length) const noexcept { return FlushViewOfFile(_addr, length); }
		[[nodiscard]] bool advise(const int32_t) const noexcept { return true; }
#endif

		template<int32_t adviceFlag, int32_t... adviceFlags> [[nodiscard]] bool advise() const noexcept
		{
			if (!advise(adviceFlag))
				return false;
			if constexpr (sizeof...(adviceFlags) == 0)
				return true;
			else
				return advise<adviceFlags...>();
		}

		template<typename T> void copyFrom(const off_t idx, T &value) const
		{
			const auto *const src = index(idx);
			memcpy(&value, src, sizeof(T));
		}

		template<typename T> void copyFrom(const off_t idx, T *value, const off_t length) const
		{
			const auto *const src = index(idx);
			assert(length <= _len - idx);
			std::memcpy(value, src, length);
		}

		template<typename T> void copyTo(const off_t idx, const T &value) const
		{
			auto *const dest = index(idx);
			memcpy(dest, &value, sizeof(T));
		}

		template<typename T> void copyTo(const off_t idx, const T &value, const off_t length) const
		{
			auto *const dest = index(idx);
			assert(length <= _len - idx);
			std::memcpy(dest, value, length);
		}

		constexpr bool operator ==(const mmap_t &b) const noexcept
			{ return _addr == b._addr && _len == b._len; }
		constexpr bool operator !=(const mmap_t &b) const noexcept { return !(*this == b); }
	};

	inline void swap(mmap_t &a, mmap_t &b) noexcept { a.swap(b); }
} // namespace pcat

#endif /* MMAP__HXX */
