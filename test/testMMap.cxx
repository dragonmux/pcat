#include "testMMap.hxx"

class testMMap final : public testsuite
{
private:
	void testDefaultConstruct() { memoryMap::testDefaultConstruct(*this); }

public:
	testMMap() = default;
	testMMap(const testMMap &) = delete;
	testMMap(testMMap &&) = delete;
	~testMMap() final = default;
	testMMap &operator =(const testMMap &) = delete;
	testMMap &operator =(testMMap &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testDefaultConstruct)
	}
};

CRUNCHpp_TESTS(testMMap)
