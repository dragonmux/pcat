#ifndef MMAP__HXX
#define MMAP__HXX

#include <cstdint>
#include <unistd.h>
#include <sys/mman.h>
#include <stdexcept>
#include <substrate/fd>

namespace pcat
{
	struct mmap_t final
	{
	private:
		off_t _len;
		void *_addr;

		constexpr void *index(const off_t idx) const
		{
			if (idx < _len)
			{
				const auto addr = reinterpret_cast<std::uintptr_t>(_addr);
				return reinterpret_cast<void *>(addr + idx);
			}
			throw std::out_of_range("mmap_t index out of range");
		}

		using fd_t = substrate::fd_t;

	public:
		constexpr mmap_t() noexcept : _len{0}, _addr{nullptr} { }
		mmap_t(const fd_t &fd, const off_t len, const int32_t prot, const int32_t flags = MAP_SHARED,
			void *addr = nullptr) noexcept : _len{len}, _addr{[&]() noexcept -> void *
			{
				const auto ptr = ::mmap(addr, len, prot, flags, fd, 0);
				return ptr == MAP_FAILED ? nullptr : ptr;
			}()} { }
		mmap_t(mmap_t &&map) noexcept : mmap_t{} { swap(map); }
		void operator =(mmap_t &&map) noexcept { swap(map); }

		~mmap_t() noexcept
		{
			if (_addr)
				::munmap(_addr, _len);
		}

		[[nodiscard]] constexpr bool valid() const noexcept { return _addr; }

		void swap(mmap_t &map) noexcept
		{
			std::swap(_addr, map._addr);
			std::swap(_len, map._len);
		}

		[[nodiscard]] constexpr off_t length() const noexcept { return _len; }
		[[nodiscard]] constexpr void *address(const off_t offset) noexcept { return index(offset); }
		[[nodiscard]] constexpr const void *address(const off_t offset) const noexcept { return index(offset); }

		template<typename T> void copyTo(const off_t idx, T &value) const
		{
			const auto *const src = index(idx);
			memcpy(&value, src, sizeof(T));
		}

		template<typename T> void copyFrom(const off_t idx, const T &value) const
		{
			const auto dest = index(idx);
			memcpy(dest, &value, sizeof(T));
		}

		constexpr bool operator ==(const mmap_t &b) const noexcept
			{ return _addr == b._addr && _len == b._len; }
		constexpr bool operator !=(const mmap_t &b) const noexcept { return !(*this == b); }
	};

	inline void swap(mmap_t &a, mmap_t &b) noexcept { a.swap(b); }
}

#endif /* MMAP__HXX */
