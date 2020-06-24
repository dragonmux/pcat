#include <thread>
#include <chrono>
#include <substrate/utility>
#include <threadPool.hxx>
#include "testThreadPool.hxx"

using pcat::affinity_t;
using pcat::threadPool_t;
using namespace std::literals::chrono_literals;

constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }
constexpr static auto totalLoopIterations = 1000000_uz;

namespace threadPool
{
	bool dummyWork() { return true; }
	bool busyWork(const std::size_t iterations)
	{
		volatile std::size_t counter{};
		for (size_t i{}; i < iterations; ++i)
		{
			for (size_t j{}; j < totalLoopIterations; ++j)
				++counter;
		}
		std::this_thread::sleep_for(25ms);
		return counter == size_t(iterations * totalLoopIterations);
	}

	void testUnused(testsuite &suite)
	{
		affinity_t affinity{};
		auto pool{substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork)};
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
		auto pool{substrate::make_unique_nothrow<threadPool_t<decltype(dummyWork)>>(dummyWork)};
		suite.assertNotNull(pool);
		suite.assertTrue(pool->valid());
		suite.assertTrue(pool->ready());
		suite.assertFalse(pool->queue());
		suite.assertTrue(pool->valid());
		suite.assertTrue(pool->finish());
		suite.assertFalse(pool->valid());
		suite.assertFalse(pool->finish());
	}

	void testQueueWait(testsuite &suite)
	{
		threadPool_t pool{busyWork};
		suite.assertTrue(pool.valid());
		suite.assertTrue(pool.ready());
		const auto threads{pool.numProcessors()};
		suite.assertNotEqual(threads, 0);
		for (std::size_t i{}; i < threads; ++i)
			[[maybe_unused]] const auto result = pool.queue(threads - i);
		//suite.assertFalse(pool.ready());
		[[maybe_unused]] const auto result = pool.queue(threads);
		std::this_thread::sleep_for(100ms);
		suite.assertTrue(pool.queue(threads));
		suite.assertTrue(pool.finish());
		suite.assertFalse(pool.valid());
		suite.assertFalse(pool.finish());
	}
} // namespace threadPool
