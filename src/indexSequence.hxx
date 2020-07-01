#ifndef INDEX_SEQUENCE__HXX
#define INDEX_SEQUENCE__HXX

#include <cstdint>

namespace pcat
{
	struct indexIterator_t final
	{
	private:
		std::size_t index;

	public:
		constexpr indexIterator_t(const std::size_t value) noexcept : index{value} { }

		[[nodiscard]] constexpr auto operator *() const noexcept { return index; }

		constexpr auto operator ++() noexcept
		{
			++index;
			return *this;
		}
	};

	[[nodiscard]] constexpr inline auto operator ==(const indexIterator_t &a, const indexIterator_t &b) noexcept
		{ return *a == *b; }
	[[nodiscard]] constexpr inline auto operator !=(const indexIterator_t &a, const indexIterator_t &b) noexcept
		{ return *a != *b; }

	struct indexSequence_t final
	{
	private:
		std::size_t begin_;
		std::size_t end_;

	public:
		constexpr indexSequence_t(const std::size_t begin, const std::size_t end) noexcept :
			begin_{begin}, end_{end} { }

		[[nodiscard]] constexpr auto begin() const noexcept { return indexIterator_t{begin_}; }
		[[nodiscard]] constexpr auto end() const noexcept { return indexIterator_t{end_}; }
	};
} // namespace pcat

#endif /*INDEX_SEQUENCE__HXX*/
