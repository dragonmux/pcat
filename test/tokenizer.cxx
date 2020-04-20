#include <string_view>
#include <substrate/utility>
#include <args/tokenizer.hxx>
#include <crunch++.h>
#include "testTokenizer.hxx"

using std::literals::string_view_literals::operator ""sv;
using pcat::args::tokenizer::tokenizer_t;
using pcat::args::tokenizer::tokenType_t;

constexpr auto stringHelp{"--help"sv};
constexpr auto simpleArgs = substrate::make_array<const char *>({stringHelp.data()});
constexpr auto assignedArgs = substrate::make_array<const char *>({"--argument=value"});

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
		suite.assertNull(token.value().data());
		suite.assertEqual(token.value().size(), 0);
	}
}
