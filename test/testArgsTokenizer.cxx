#include <string_view>
#include "testArgsTokenizer.hxx"

using namespace std::literals::string_view_literals;
using pcat::args::tokenizer::tokenizer_t;
using pcat::args::tokenizer::tokenType_t;

class testTokenizer final : public testsuite
{
private:
	void testConstruct()
	{
		tokenizer_t lexer{0, nullptr};
		const auto &token{lexer.token()};
		assertFalse(token.valid());
		assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::unknown));
	}

	void testSimple() { tokenizer::testSimple(*this); }
	void testAssigned() { tokenizer::testAssigned(*this); }
	void testMultiple() { tokenizer::testMultiple(*this); }

	void testTypeToName()
	{
		assertEqual(typeToName(tokenType_t::unknown), "unknown"sv);
		assertEqual(typeToName(tokenType_t::arg), "argument"sv);
		assertEqual(typeToName(tokenType_t::space), "implicit space"sv);
		assertEqual(typeToName(tokenType_t::equals), "equals sign"sv);
	}

public:
	testTokenizer() = default;
	testTokenizer(const testTokenizer &) = delete;
	testTokenizer(testTokenizer &&) = delete;
	~testTokenizer() final = default;
	testTokenizer &operator =(const testTokenizer &) = delete;
	testTokenizer &operator =(testTokenizer &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testConstruct)
		CRUNCHpp_TEST(testSimple)
		CRUNCHpp_TEST(testAssigned)
		CRUNCHpp_TEST(testMultiple)
		CRUNCHpp_TEST(testTypeToName)
	}
};

CRUNCHpp_TESTS(testTokenizer)
