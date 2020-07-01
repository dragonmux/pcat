#include "testIndexSequence.hxx"

class testIndexSequence final : public testsuite
{
private:
	void testIteration() { indexSequence::testIteration(*this); }
	void testIterator() { indexSequence::testIterator(*this); }

public:
	testIndexSequence() noexcept = default;
	testIndexSequence(const testIndexSequence &) = delete;
	testIndexSequence(testIndexSequence &&) = delete;
	~testIndexSequence() final = default;
	testIndexSequence &operator =(const testIndexSequence &) = delete;
	testIndexSequence &operator =(testIndexSequence &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testIteration)
		CRUNCHpp_TEST(testIterator)
	}
};

CRUNCHpp_TESTS(testIndexSequence)
