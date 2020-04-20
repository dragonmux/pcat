#include <string_view>
#include <substrate/utility>
#include <args.hxx>
#include <crunch++.h>
#include "testArgsParser.hxx"

using std::literals::string_view_literals::operator ""sv;

constexpr auto emptyArgs = substrate::make_array<const char *>({"test"});
constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--argument"sv};
constexpr static auto stringValue{"value"sv};
constexpr static auto stringVersion{"--version"sv};
constexpr static auto simpleArgs = substrate::make_array<const char *>({"--help"});
constexpr static auto assignedArgs = substrate::make_array<const char *>({"--argument=value"});
constexpr static auto multipleArgs = substrate::make_array<const char *>(
{
	"--version",
	"--argument=value",
	"--help"
});

namespace parser
{
	void testEmpty(testsuit &suite)
	{
		suite.assertFalse(parseArguments(0, nullptr, nullptr, nullptr));
		suite.assertFalse(parseArguments(2, nullptr, nullptr, nullptr));
		suite.assertFalse(parseArguments(emptyArgs.size(), emptyArgs.data(), nullptr, nullptr));
	}

	void testSimple(testsuit &suite)
	{
	}

	void testAssigned(testsuit &suite)
	{
	}

	void testMultiple(testsuit &suite)
	{
	}
}
