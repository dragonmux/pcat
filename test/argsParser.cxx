#include <type_traits>
#include <string_view>
#include <substrate/utility>
#include <args.hxx>
#include "testArgsParser.hxx"

using namespace std::literals::string_view_literals;
using pcat::args::option_t;
using pcat::args::argNode_t;
using pcat::args::argOfType_t;
using pcat::args::argHelp_t;
using pcat::args::argVersion_t;
using pcat::args::argOutputFile_t;
using pcat::args::argAsync_t;
using pcat::args::argThreads_t;
using pcat::args::argPinning_t;
using pcat::args::argAlgorithm_t;
using pcat::args::argUnrecognised_t;
using pcat::args::algorithm_t;

constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }

constexpr auto emptyArgs{substrate::make_array<const char *>({"test"})};
constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--output"sv};
constexpr static auto stringFile{"file"sv};
constexpr static auto stringVersion{"--version"sv};
constexpr static auto stringValue{"--value"sv};
constexpr static auto stringAsync{"--async"sv};
constexpr static auto stringThreads{"--threads"sv};
constexpr static auto stringThreadCount{"4"sv};
constexpr static auto stringCorePins{"--core-pins"sv};
constexpr static auto stringCoreNumbers{"0,1,4,5"sv};
constexpr static auto stringAlgorithm{"--algorithm"sv};
constexpr static auto stringBlockLinear{"blockLinear"sv};
constexpr static auto corePins{substrate::make_array<std::size_t>({0_uz, 1_uz, 4_uz, 5_uz})};
constexpr static auto simpleArgs{substrate::make_array<const char *>({"test", "--help"})};
constexpr static auto assignedArgs{substrate::make_array<const char *>({"test", "--output=file"})};
constexpr static auto delimitedArgs{substrate::make_array<const char *>({"test", "--output", "file"})};
constexpr static auto multipleArgs{substrate::make_array<const char *>(
{
	"test",
	"--version",
	"--output=file",
	"--help",
	"--async",
	"--threads=4",
	"--core-pins",
	"0,1,4,5",
	"--algorithm",
	"blockLinear"
})};
constexpr static auto invalidAssignedArgs{substrate::make_array<const char *>({"test", "--value=", "file"})};
constexpr static auto invalidEqualsArgs{substrate::make_array<const char *>({"test", "="})};
constexpr static auto badOutputFileArgs{substrate::make_array<const char *>({"test", "--output"})};
constexpr static auto badThreadsArgs{substrate::make_array<const char *>({"test", "--threads"})};
constexpr static auto invalidThreadsArgs{substrate::make_array<const char *>({"test", "--threads", "0"})};
constexpr static auto shortThreadsArgs{substrate::make_array<const char *>({"test", "--threads="})};
constexpr static auto negativeThreadsArgs{substrate::make_array<const char *>({"test", "--threads", "-1"})};
constexpr static auto nonNumericThreadsArgs{substrate::make_array<const char *>({"test", "--threads", "a1"})};
constexpr static auto badPinningArgs{substrate::make_array<const char *>({"test", "--core-pins"})};
constexpr static auto invalidPinningArgs{substrate::make_array<const char *>({"test", "--core-pins", "0,"})};
constexpr static auto shortPinningArgs{substrate::make_array<const char *>({"test", "--core-pins="})};
constexpr static auto negativePinningArgs{substrate::make_array<const char *>({"test", "--core-pins", "-1"})};
constexpr static auto nonNumericPinningArgs{substrate::make_array<const char *>({"test", "--core-pins", "a1"})};
constexpr static auto badlyDelimitedPinningArgs{substrate::make_array<const char *>({"test", "--core-pins", "0;"})};
constexpr static auto badAlgorithmArgs{substrate::make_array<const char *>({"test", "--algorithm"})};
constexpr static auto invalidAlgorithmArgs{substrate::make_array<const char *>({"test", "--algorithm", "flump"})};
constexpr static auto shortAlgorithmArgs{substrate::make_array<const char *>({"test", "--algorithm="})};
constexpr static auto simpleOptions{substrate::make_array<option_t>({{"--help"sv, argType_t::help}})};
constexpr static auto assignedOptions{substrate::make_array<option_t>({{"--output"sv, argType_t::outputFile}})};
constexpr static auto multipleOptions{substrate::make_array<option_t>(
{
	{"--help"sv, argType_t::help},
	{"--version"sv, argType_t::version},
	{"--output"sv, argType_t::outputFile},
	{"--async"sv, argType_t::async},
	{"--threads"sv, argType_t::threads},
	{"--core-pins"sv, argType_t::pinning},
	{"--algorithm"sv, argType_t::algorithm}
})};
constexpr static auto badFileOption{substrate::make_array<option_t>({{"--output"sv, argType_t::outputFile}})};
constexpr static auto badThreadsOption{substrate::make_array<option_t>({{"--threads"sv, argType_t::threads}})};
constexpr static auto badPinningOption{substrate::make_array<option_t>({{"--core-pins"sv, argType_t::pinning}})};
constexpr static auto badAlgorithmOption{substrate::make_array<option_t>({{"--algorithm"sv, argType_t::algorithm}})};

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

			auto *const search = args->find(type);
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
			auto *const node = dynamic_cast<argOutputFile_t *>(arg.get());
			suite.assertEqual(node->fileName().size(), stringFile.size());
			suite.assertEqual(node->fileName().data(), stringFile.data(), stringFile.size());

			auto *const search = args->find(argType_t::outputFile);
			suite.assertNotNull(search);
			suite.assertEqual(search, arg.get());
		}
	};

	template<> struct assertNode_t<argThreads_t>
	{
		void operator()(testsuite &suite, const std::unique_ptr<argNode_t> &arg)
		{
			suite.assertNotNull(arg);
			suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::threads));
			auto *const node = dynamic_cast<argThreads_t *>(arg.get());
			suite.assertEqual(node->threads(), 4);
		}
	};

	template<> struct assertNode_t<argPinning_t>
	{
		void operator()(testsuite &suite, const std::unique_ptr<argNode_t> &arg)
		{
			suite.assertNotNull(arg);
			suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::pinning));
			auto *const node = dynamic_cast<argPinning_t *>(arg.get());
			suite.assertEqual(node->count(), 4);
			suite.assertFalse(node->empty());
			suite.assertTrue(std::equal(node->begin(), node->end(), corePins.begin(),
				[](const std::size_t a, const std::size_t b) noexcept -> bool { return a == b; }));
		}
	};

	template<> struct assertNode_t<argAlgorithm_t>
	{
		void operator()(testsuite &suite, const std::unique_ptr<argNode_t> &arg)
		{
			suite.assertNotNull(arg);
			suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::algorithm));
			auto *const node = dynamic_cast<argAlgorithm_t *>(arg.get());
			suite.assertTrue(node->valid());
			suite.assertEqual(static_cast<uint8_t>(node->algorithm()),
				static_cast<uint8_t>(algorithm_t::blockLinear));
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
		suite.assertEqual(args->count(), 7);
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
		suite.assertTrue(iterator != args->end());
		assertNode_t<argAsync_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator != args->end());
		assertNode_t<argThreads_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator != args->end());
		assertNode_t<argPinning_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator != args->end());
		assertNode_t<argAlgorithm_t>{}(suite, *iterator);
		++iterator;
		suite.assertTrue(iterator == args->end());
		suite.assertNull(args->find(argType_t::unrecognised));
	}

	void testUnknown(testsuite &suite)
	{
		args = {};
		suite.assertTrue(parseArguments(multipleArgs.size(), multipleArgs.data(), nullptr, nullptr));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 9);
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
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringAsync.size());
		suite.assertEqual(node->argument().data(), stringAsync.data(), stringAsync.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringThreads.size());
		suite.assertEqual(node->argument().data(), stringThreads.data(), stringThreads.size());
		suite.assertEqual(node->parameter().size(), stringThreadCount.size());
		suite.assertEqual(node->parameter().data(), stringThreadCount.data(), stringThreadCount.size());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringCorePins.size());
		suite.assertEqual(node->argument().data(), stringCorePins.data(), stringCorePins.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringCoreNumbers.size());
		suite.assertEqual(node->argument().data(), stringCoreNumbers.data(), stringCoreNumbers.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringAlgorithm.size());
		suite.assertEqual(node->argument().data(), stringAlgorithm.data(), stringAlgorithm.size());
		suite.assertNull(node->parameter().data());

		++iterator;
		suite.assertTrue(iterator != args->end());
		arg = iterator->get();
		suite.assertNotNull(arg);
		suite.assertEqual(static_cast<uint8_t>(arg->type()), static_cast<uint8_t>(argType_t::unrecognised));
		node = dynamic_cast<decltype(node)>(arg);
		suite.assertEqual(node->argument().size(), stringBlockLinear.size());
		suite.assertEqual(node->argument().data(), stringBlockLinear.data(), stringBlockLinear.size());
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

	void testBadOutputFile(testsuite &suite)
	{
		args = {};
		suite.assertFalse(parseArguments(badOutputFileArgs.size(), badOutputFileArgs.data(), badFileOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
	}

	void testBadThreads(testsuite &suite)
	{
		args = {};
		suite.assertFalse(parseArguments(badThreadsArgs.size(), badThreadsArgs.data(), badThreadsOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(invalidThreadsArgs.size(), invalidThreadsArgs.data(), badThreadsOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(shortThreadsArgs.size(), shortThreadsArgs.data(), badThreadsOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(negativeThreadsArgs.size(), negativeThreadsArgs.data(), badThreadsOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(
			parseArguments(nonNumericThreadsArgs.size(), nonNumericThreadsArgs.data(), badThreadsOption)
		);
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
	}

	void testBadPinning(testsuite &suite)
	{
		args = {};
		suite.assertFalse(parseArguments(badPinningArgs.size(), badPinningArgs.data(), badPinningOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(invalidPinningArgs.size(), invalidPinningArgs.data(), badPinningOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(shortPinningArgs.size(), shortPinningArgs.data(), badPinningOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(negativePinningArgs.size(), negativePinningArgs.data(), badPinningOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(
			parseArguments(nonNumericPinningArgs.size(), nonNumericPinningArgs.data(), badPinningOption)
		);
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(
			parseArguments(badlyDelimitedPinningArgs.size(), badlyDelimitedPinningArgs.data(), badPinningOption)
		);
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
	}

	void testBadAlgorithm(testsuite &suite)
	{
		args = {};
		suite.assertFalse(parseArguments(badAlgorithmArgs.size(), badAlgorithmArgs.data(), badAlgorithmOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(
			parseArguments(invalidAlgorithmArgs.size(), invalidAlgorithmArgs.data(), badAlgorithmOption)
		);
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);

		args = {};
		suite.assertFalse(parseArguments(shortAlgorithmArgs.size(), shortAlgorithmArgs.data(), badAlgorithmOption));
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
	}
} // namespace parser
