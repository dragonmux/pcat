#include <type_traits>
#include <string_view>
#include <substrate/utility>
#include <args.hxx>
#include <crunch++.h>
#include "testArgsParser.hxx"

using std::literals::string_view_literals::operator ""sv;
using pcat::args::argUnrecognised_t;

constexpr auto emptyArgs = substrate::make_array<const char *>({"test"});
constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--output"sv};
constexpr static auto stringValue{"file"sv};
constexpr static auto stringVersion{"--version"sv};
constexpr static auto simpleArgs = substrate::make_array<const char *>({"test", "--help"});
constexpr static auto assignedArgs = substrate::make_array<const char *>({"test", "--argument=value"});
constexpr static auto multipleArgs = substrate::make_array<const char *>(
{
	"test",
	"--version",
	"--output=file",
	"--help"
});

namespace parser
{
	void testEmpty(testsuit &suite)
	{
		suite.assertNull(args);
		suite.assertFalse(parseArguments(0, nullptr, nullptr, nullptr));
		suite.assertNull(args);
		suite.assertFalse(parseArguments(2, nullptr, nullptr, nullptr));
		suite.assertNull(args);
		suite.assertFalse(parseArguments(emptyArgs.size(), emptyArgs.data(), nullptr, nullptr));
		suite.assertNull(args);
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

	void testUnknown(testsuit &suite)
	{
		suite.assertNull(args);
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
		args = {};
	}
} // namespace parser
