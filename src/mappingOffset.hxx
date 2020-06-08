#ifndef MAPPING_OFFSET_HXX
#define MAPPING_OFFSET_HXX

#include "chunking.hxx"

namespace pcat
{
	struct mappingOffset_t final
	{
	private:
		off_t offset_{0};
		off_t adjust_{0};
		off_t length_{0};

		constexpr void recalcAdjustment()
		{
			adjust_ = offset_ % pageSize;
			offset_ -= adjust_;
		}

	public:
		constexpr mappingOffset_t() noexcept = default;
		constexpr mappingOffset_t(const off_t offset) noexcept : offset_{offset} { recalcAdjustment(); }
		constexpr mappingOffset_t(const off_t offset, const off_t length) noexcept :
			offset_{offset}, length_{length} { recalcAdjustment(); }
		[[nodiscard]] constexpr off_t offset() const noexcept { return offset_ + adjust_; }
		[[nodiscard]] constexpr off_t adjustment() const noexcept { return adjust_; }
		[[nodiscard]] constexpr off_t length() const noexcept { return length_; }
		[[nodiscard]] constexpr off_t adjustedOffset() const noexcept { return offset_; }
		[[nodiscard]] constexpr off_t adjustedLength() const noexcept { return length_ + adjust_; }
		constexpr void length(const off_t length) noexcept { length_ = length; }
		constexpr operator off_t() const noexcept { return offset(); }

		constexpr mappingOffset_t &operator +=(const off_t amount) noexcept
		{
			offset_ += amount + adjust_;
			recalcAdjustment();
			return *this;
		}
	};
} // namespace pcat

#endif /*MAPPING_OFFSET_HXX*/
