#include <string_view>
#include <substrate/utility>
#include "testArgsTokenizer.hxx"

using namespace std::literals::string_view_literals;
using pcat::args::tokenizer::tokenizer_t;
using pcat::args::tokenizer::tokenType_t;

constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--argument"sv};
constexpr static auto stringEquals{"="sv};
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

namespace tokenizer
{
	void testSimple(testsuite &suite)
	{
		tokenizer_t lexer{simpleArgs.size(), simpleArgs.data()};
		const auto &token{lexer.token()};

		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringHelp.size());
		suite.assertEqual(token.value().data(), stringHelp.data(), stringHelp.size());

		suite.assertFalse(lexer.next().valid());
		suite.assertFalse(token.valid());
		suite.assertEqual(token.value().size(), 0);
		suite.assertNull(token.value().data());
	}

	void testAssigned(testsuite &suite)
	{
		tokenizer_t lexer{assignedArgs.size(), assignedArgs.data()};
		const auto &token{lexer.token()};

		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringArgument.size());
		suite.assertEqual(token.value().data(), stringArgument.data(), stringArgument.size());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::equals));
		suite.assertEqual(token.value().size(), stringEquals.size());
		suite.assertEqual(token.value().data(), stringEquals.data(), stringEquals.size());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringValue.size());
		suite.assertEqual(token.value().data(), stringValue.data(), stringValue.size());

		suite.assertFalse(lexer.next().valid());
		suite.assertFalse(token.valid());
		suite.assertEqual(token.value().size(), 0);
		suite.assertNull(token.value().data());
	}

	void testMultiple(testsuite &suite)
	{
		tokenizer_t lexer{multipleArgs.size(), multipleArgs.data()};
		const auto &token{lexer.token()};

		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringVersion.size());
		suite.assertEqual(token.value().data(), stringVersion.data(), stringVersion.size());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::space));
		suite.assertEqual(token.value().size(), 0);
		suite.assertNull(token.value().data());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringArgument.size());
		suite.assertEqual(token.value().data(), stringArgument.data(), stringArgument.size());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::equals));
		suite.assertEqual(token.value().size(), stringEquals.size());
		suite.assertEqual(token.value().data(), stringEquals.data(), stringEquals.size());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringValue.size());
		suite.assertEqual(token.value().data(), stringValue.data(), stringValue.size());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::space));
		suite.assertEqual(token.value().size(), 0);
		suite.assertNull(token.value().data());

		suite.assertTrue(lexer.next().valid());
		suite.assertTrue(token.valid());
		suite.assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::arg));
		suite.assertEqual(token.value().size(), stringHelp.size());
		suite.assertEqual(token.value().data(), stringHelp.data(), stringHelp.size());

		suite.assertFalse(lexer.next().valid());
		suite.assertFalse(token.valid());
		suite.assertEqual(token.value().size(), 0);
		suite.assertNull(token.value().data());
	}
} // namespace tokenizer
