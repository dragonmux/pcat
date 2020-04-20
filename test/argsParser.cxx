#include <type_traits>
#include <string_view>
#include <substrate/utility>
#include <args.hxx>
#include <crunch++.h>
#include "testArgsParser.hxx"

using std::literals::string_view_literals::operator ""sv;
using pcat::args::option_t;
using pcat::args::argUnrecognised_t;

constexpr auto emptyArgs = substrate::make_array<const char *>({"test"});
constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--output"sv};
constexpr static auto stringValue{"file"sv};
constexpr static auto stringVersion{"--version"sv};
constexpr static auto simpleArgs = substrate::make_array<const char *>({"test", "--help"});
constexpr static auto assignedArgs = substrate::make_array<const char *>({"test", "--output=file"});
constexpr static auto delimitedArgs = substrate::make_array<const char *>({"test", "--output", "file"});
constexpr static auto multipleArgs = substrate::make_array<const char *>(
{
	"test",
	"--version",
	"--output=file",
	"--help"
});
constexpr static auto simpleOptions = substrate::make_array<option_t>({{"--help"sv, argType_t::help}});
constexpr static auto assignedOptions = substrate::make_array<option_t>({{"--output"sv, argType_t::outputFile}});
constexpr static auto multipleOptions = substrate::make_array<option_t>(
{
	{"--help"sv, argType_t::help},
	{"--version"sv, argType_t::version},
	{"--output"sv, argType_t::outputFile}
});

namespace parser
{
	void testEmpty(testsuit &suite)
	{
		args = {};
		suite.assertFalse(parseArguments(0, nullptr, nullptr, nullptr));
		suite.assertNull(args);
		suite.assertFalse(parseArguments(2, nullptr, nullptr, nullptr));
		suite.assertNull(args);
		suite.assertFalse(parseArguments(emptyArgs.size(), emptyArgs.data(), nullptr, nullptr));
		suite.assertNull(args);
	}

	void testSimple(testsuit &suite)
	{
		args = {};
		suite.assertTrue(parseArguments(simpleArgs.size(), simpleArgs.data(), simpleOptions));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 1);
		auto iterator = args->begin();
		const std::remove_pointer_t<decltype(iterator->get())> *arg{nullptr};

		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::help));

		++iterator;
		suite.assertTrue(iterator == args->end());
		const auto help = args->find(argType_t::help);
		suite.assertNotNull(help);
		suite.assertEqual(help, const_cast<decltype(help)>(arg));
		suite.assertNull(args->find(argType_t::version));
	}

	template<size_t argsCount> void testAssigned(testsuit &suite,
		const std::array<const char *, argsCount> &testArgs)
	{
		args = {};
	}

	void testAssigned(testsuit &suite)
	{
		testAssigned(suite, assignedArgs);
		testAssigned(suite, delimitedArgs);
	}

	void testMultiple(testsuit &suite)
	{
		args = {};
	}

	void testUnknown(testsuit &suite)
	{
		args = {};
		suite.assertTrue(parseArguments(multipleArgs.size(), multipleArgs.data(), nullptr, nullptr));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 3);
		auto iterator = args->begin();
		const std::remove_pointer_t<decltype(iterator->get())> *arg{nullptr};
		const argUnrecognised_t *node{nullptr};

		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringVersion.size());
		suite.assertEqual(node->argument().data(), stringVersion.data(), stringVersion.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringArgument.size());
		suite.assertEqual(node->argument().data(), stringArgument.data(), stringArgument.size());
		suite.assertEqual(node->parameter().size(), stringValue.size());
		suite.assertEqual(node->parameter().data(), stringValue.data(), stringValue.size());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringHelp.size());
		suite.assertEqual(node->argument().data(), stringHelp.data(), stringHelp.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator == args->end());
		suite.assertNull(args->find(argType_t::help));
	}
} // namespace parser
