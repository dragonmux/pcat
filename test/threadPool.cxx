#include <thread>
#include <chrono>
#include <substrate/utility>
#include <threadPool.hxx>
#include "testAffinity.hxx"

using pcat::threadPool_t;
using namespace std::literals::chrono_literals;

namespace threadPool
{
	bool dummyWork() { return true; }

	void testUnused(testsuite &suite)
	{
		auto pool = substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork);
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		suite.assertFalse(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
	}

	void testOnce(testsuite &suite)
	{
		auto pool = substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork);
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		suite.assertFalse(pool->queue());
		while (!pool->ready())
			std::this_thread::sleep_for(1us);
		suite.assertTrue(pool->valid());
		suite.assertTrue(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
	}
}
