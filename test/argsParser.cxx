#include <type_traits>
#include <string_view>
#include <substrate/utility>
#include <args.hxx>
#include "testArgsParser.hxx"

using std::literals::string_view_literals::operator ""sv;
using pcat::args::option_t;
using pcat::args::argNode_t;
using pcat::args::argOfType_t;
using pcat::args::argHelp_t;
using pcat::args::argVersion_t;
using pcat::args::argOutputFile_t;
using pcat::args::argUnrecognised_t;

constexpr auto emptyArgs = substrate::make_array<const char *>({"test"});
constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--output"sv};
constexpr static auto stringFile{"file"sv};
constexpr static auto stringVersion{"--version"sv};
constexpr static auto stringValue{"--value"sv};
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
constexpr static auto invalidAssignedArgs = substrate::make_array<const char *>({"test", "--value=", "file"});
constexpr static auto invalidEqualsArgs = substrate::make_array<const char *>({"test", "="});
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
	void testEmpty(testsuite &suite)
	{
		args = {};
		suite.assertFalse(parseArguments(0, nullptr, nullptr, nullptr));
		suite.assertNull(args);
		suite.assertFalse(parseArguments(2, nullptr, nullptr, nullptr));
		suite.assertNull(args);
		suite.assertFalse(parseArguments(emptyArgs.size(), emptyArgs.data(), nullptr, nullptr));
		suite.assertNull(args);
	}

	template<typename> struct assertNode_t;

	template<argType_t type> struct assertNode_t<argOfType_t<type>>
	{
		void operator ()(testsuite &suite, const std::unique_ptr<argNode_t> &arg)
		{
			suite.assertNotNull(arg);
			suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(type));

			const auto search = args->find(type);
			suite.assertNotNull(search);
			suite.assertEqual(search, arg.get());
		}
	};

	template<> struct assertNode_t<argOutputFile_t>
	{
		void operator()(testsuite &suite, const std::unique_ptr<argNode_t> &arg)
		{
			suite.assertNotNull(arg);
			suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::outputFile));
			const auto node = dynamic_cast<argOutputFile_t *>(arg.get());
			suite.assertEqual(node->fileName().size(), stringFile.size());
			suite.assertEqual(node->fileName().data(), stringFile.data(), stringFile.size());

			const auto search = args->find(argType_t::outputFile);
			suite.assertNotNull(search);
			suite.assertEqual(search, arg.get());
		}
	};

	void testSimple(testsuite &suite)
	{
		args = {};
		suite.assertTrue(parseArguments(simpleArgs.size(), simpleArgs.data(), simpleOptions));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 1);
		auto iterator = args->begin();
		suite.assertTrue(iterator != args->end());
		assertNode_t<argHelp_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator == args->end());
		suite.assertNull(args->find(argType_t::version));
		suite.assertNull(args->find(argType_t::unrecognised));
	}

	template<size_t argsCount> void testAssigned(testsuite &suite,
		const std::array<const char *, argsCount> &testArgs)
	{
		args = {};
		suite.assertTrue(parseArguments(testArgs.size(), testArgs.data(), assignedOptions));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 1);
		auto iterator = args->begin();
		suite.assertTrue(iterator != args->end());
		assertNode_t<argOutputFile_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator == args->end());
		suite.assertNull(args->find(argType_t::version));
		suite.assertNull(args->find(argType_t::unrecognised));
	}

	void testAssigned(testsuite &suite)
	{
		testAssigned(suite, assignedArgs);
		testAssigned(suite, delimitedArgs);
	}

	void testMultiple(testsuite &suite)
	{
		args = {};
		suite.assertTrue(parseArguments(multipleArgs.size(), multipleArgs.data(), multipleOptions));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 3);
		auto iterator = args->begin();
		suite.assertTrue(iterator != args->end());
		assertNode_t<argVersion_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator != args->end());
		assertNode_t<argOutputFile_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator != args->end());
		assertNode_t<argHelp_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator == args->end());
		suite.assertNull(args->find(argType_t::unrecognised));
	}

	void testUnknown(testsuite &suite)
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
		suite.assertEqual(node->parameter().size(), stringFile.size());
		suite.assertEqual(node->parameter().data(), stringFile.data(), stringFile.size());

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

	void testInvalid(testsuite &suite)
	{
		args = {};
		suite.assertTrue(parseArguments(invalidAssignedArgs.size(), invalidAssignedArgs.data(), assignedOptions));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 2);
		auto iterator = args->begin();
		const std::remove_pointer_t<decltype(iterator->get())> *arg{nullptr};
		const argUnrecognised_t *node{nullptr};

		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringValue.size());
		suite.assertEqual(node->argument().data(), stringValue.data(), stringValue.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringFile.size());
		suite.assertEqual(node->argument().data(), stringFile.data(), stringFile.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator == args->end());
		suite.assertNull(args->find(argType_t::outputFile));

		args = {};
		suite.assertFalse(parseArguments(invalidEqualsArgs.size(), invalidEqualsArgs.data(), nullptr, nullptr));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
		suite.assertTrue(args->begin() == args->end());
	}
} // namespace parser
