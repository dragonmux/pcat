#include "testIndexSequence.hxx"

class testIndexSequence final : public testsuite
{
private:
	void testIterator() { indexSequence::testIterator(*this); }
	void testIteration() { indexSequence::testIteration(*this); }

public:
	testIndexSequence() noexcept = default;
	testIndexSequence(const testIndexSequence &) = delete;
	testIndexSequence(testIndexSequence &&) = delete;
	~testIndexSequence() final = default;
	testIndexSequence &operator =(const testIndexSequence &) = delete;
	testIndexSequence &operator =(testIndexSequence &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testIterator)
		CRUNCHpp_TEST(testIteration)
	}
};

CRUNCHpp_TESTS(testIndexSequence)
