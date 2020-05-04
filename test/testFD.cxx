#ifndef _WINDOWS
#include <unistd.h>
#else
#include <io.h>
#endif
#include <crunch++.h>
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
		CXX_TEST(testBadFD)
		CXX_TEST(testBadOpen)
		CXX_TEST(testWrite)
		CXX_TEST(testSeek)
		CXX_TEST(testRead)
	}
};

CRUNCH_API void registerCXXTests() noexcept;
void registerCXXTests() noexcept
{
	registerTestClasses<testFD>();
}
