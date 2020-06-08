#include "testMappingOffset.hxx"

class testMappingOffset final : public testsuite
{
private:
	void testDefaultConstruct() { mappingOffset::testDefaultConstruct(*this); }
	void testOffsetConstruct() { mappingOffset::testOffsetConstruct(*this); }
	//void testQueueWait() { mappingOffset::testQueueWait(*this); }

public:
	testMappingOffset() = default;
	testMappingOffset(const testMappingOffset &) = delete;
	testMappingOffset(testMappingOffset &&) = delete;
	~testMappingOffset() final = default;
	testMappingOffset &operator =(const testMappingOffset &) = delete;
	testMappingOffset &operator =(testMappingOffset &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testDefaultConstruct)
		CRUNCHpp_TEST(testOffsetConstruct)
		//CRUNCHpp_TEST(testQueueWait)
	}
};

CRUNCHpp_TESTS(testMappingOffset)
