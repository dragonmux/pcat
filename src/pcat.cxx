#include <cstring>
#include <array>
#include <type_traits>
#include <utility>
#include <string>
#include <substrate/fd>
#include <substrate/mmap>
#include <substrate/units>
#include "args.hxx"

namespace substrate
{
	namespace impl
	{
		template<typename T, size_t N, size_t... index> constexpr std::array<T, N>
			makeArray(T (&&elems)[N], std::index_sequence<index...>) // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
		{
			return {elems[index]...};
		}
	}

	template<typename T, size_t N> constexpr std::array<T, N> make_array(T (&&elems)[N]) // NOLINT(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
	{
		return impl::makeArray(std::move(elems), std::make_index_sequence<N>{});
	}
}

namespace pcat
{
	using substrate::mmap_t;
	using substrate::operator ""_KiB;
	using std::literals::string_literals::operator ""s;

	static const auto options{substrate::make_array<args::option_t>({ // NOLINT(cert-err58-cpp)
		{"--version"s, argType_t::version},
		{"--help"s, argType_t::help}
	})};

	constexpr static size_t pageSize = 4_KiB;
}

int main(int argCount, char **argList)
{
	if (!parseArguments(argCount, argList, pcat::options))
	{
		puts("Failed to parse arguments");
		return 1;
	}
	return 0;
}
