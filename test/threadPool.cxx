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
	bool busyWork(const std::size_t iterations)
	{
		volatile std::size_t counter{};
		for (size_t i{}; i < iterations; ++i)
		{
			for (size_t j{}; j < 1000000; ++j)
				++counter;
		}
		std::this_thread::sleep_for(25ms);
		return counter == size_t(iterations * 1000000U);
	}

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
			std::this_thread::sleep_for(1us);
		suite.assertFalse(pool->queue());
		suite.assertTrue(pool->valid());
		suite.assertTrue(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
	}

	void testQueueWait(testsuite &suite)
	{
		auto pool = substrate::make_unique_nothrow<threadPool_t<decltype(busyWork)>>(busyWork);
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		while (!pool->ready())
			std::this_thread::sleep_for(1us);
		const auto threads{pool->numProcessors()};
		suite.assertNotEqual(threads, 0);
		for (std::size_t i{}; i < threads; ++i)
			suite.assertFalse(pool->queue(threads - i));
		//suite.assertFalse(pool->ready()); // in theory, this should be false here..
		pool->queue(threads);
		suite.assertTrue(pool->finish());
		suite.assertFalse(pool->valid());
	}
}
