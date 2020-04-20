#include <crunch++.h>
#include <args/tokenizer.hxx>

using pcat::args::tokenizer::tokenizer_t;
using pcat::args::tokenizer::tokenType_t;

class testTokenizer : public testsuit
{
private:
	void testConstruct()
	{
		tokenizer_t lexer{0, nullptr};
		const auto &token{lexer.token()};
		assertFalse(token.valid());
		assertEqual(static_cast<uint8_t>(token.type()), static_cast<uint8_t>(tokenType_t::unknown));
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
		CXX_TEST(testConstruct)
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testTokenizer>();
}
