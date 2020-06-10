#include <string_view>
#include "testMMap.hxx"

using namespace std::literals::string_view_literals;

class testMMap final : public testsuite
{
private:
	fd_t testData;

	void prepare() noexcept
	{
		if (!testData.valid())
			return;
		testData.write("mmap"sv);
	}

	void testDefaultConstruct() { memoryMap::testDefaultConstruct(*this); }
	void testMapEntireFile() { memoryMap::testMapEntireFile(*this, testData); }

public:
	testMMap() : testData{"mmap.test", O_CREAT | O_RDWR, substrate::normalMode} { prepare(); }
	testMMap(const testMMap &) = delete;
	testMMap(testMMap &&) = delete;
	testMMap &operator =(const testMMap &) = delete;
	testMMap &operator =(testMMap &&) = delete;

	~testMMap() final
	{
		if (testData.valid())
		{
			testData = {};
			unlink("mmap.test");
		}
	}

	void registerTests() final
	{
		if (!testData.valid())
			skip("Could not create test data for mmap_t tests");
		CRUNCHpp_TEST(testDefaultConstruct)
		CRUNCHpp_TEST(testMapEntireFile)
	}
};

CRUNCHpp_TESTS(testMMap)
