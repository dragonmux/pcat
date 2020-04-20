#include <string_view>
#include <substrate/utility>
#include <args/tokenizer.hxx>
#include <crunch++.h>
#include "testTokenizer.hxx"

using std::literals::string_view_literals::operator ""sv;
using pcat::args::tokenizer::tokenizer_t;
using pcat::args::tokenizer::tokenType_t;

constexpr static auto stringHelp{"--help"sv};
constexpr static auto stringArgument{"--argument"sv};
constexpr static auto stringEquals{"="sv};
constexpr static auto stringValue{"value"sv};
const static auto simpleArgs = substrate::make_array<const char *>({stringHelp.data()});
constexpr static auto assignedArgs = substrate::make_array<const char *>({"--argument=value"});

namespace tokenizer
{
	void testSimple(testsuit &suite)
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

	void testAssigned(testsuit &suite)
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
}
