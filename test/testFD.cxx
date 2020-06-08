#ifndef _WINDOWS
#include <unistd.h>
#else
#include <io.h>
#endif
#include "testFD.hxx"

class testFD final : public testsuite
{
private:
	void testBadFD() { fd::testBadFD(*this); }
	void testBadOpen() { fd::testBadOpen(*this); }
	void testWrite() { fd::testWrite(*this); }
	void testSeek() { fd::testSeek(*this); }
	void testRead() { fd::testRead(*this); }

public:
	testFD() = default;
	testFD(const testFD &) = delete;
	testFD(testFD &&) = delete;
	~testFD() final { unlink("fd.test"); }
	testFD &operator =(const testFD &) = delete;
	testFD &operator =(testFD &&) = delete;

	void registerTests() final
	{
		CRUNCHpp_TEST(testBadFD)
		CRUNCHpp_TEST(testBadOpen)
		CRUNCHpp_TEST(testWrite)
		CRUNCHpp_TEST(testSeek)
		CRUNCHpp_TEST(testRead)
	}
};

CRUNCHpp_TESTS(testFD)
