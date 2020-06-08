#include <thread>
#include <chrono>
#include <substrate/utility>
#include <threadPool.hxx>
#include "testAffinity.hxx"

using pcat::affinity_t;
using pcat::threadPool_t;
using namespace std::literals::chrono_literals;

namespace threadPool
{
	bool dummyWork() { return true; }

	void testUnused(testsuite &suite)
	{
		affinity_t affinity{};
		auto pool = substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork);
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		suite.assertEqual(pool->numProcessors(), affinity.numProcessors());
		suite.assertFalse(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
		suite.assertEqual(pool->numProcessors(), affinity.numProcessors());
	}

	void testOnce(testsuite &suite)
	{
		auto pool = substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork);
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		while (!pool->ready())
		{
			std::this_thread::yield();
			std::this_thread::sleep_for(1us);
		}
		suite.assertFalse(pool->queue());
		suite.assertTrue(pool->valid());
		suite.assertTrue(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
	}
}
