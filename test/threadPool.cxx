#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <substrate/utility>
#include <threadPool.hxx>
#include "testThreadPool.hxx"

using pcat::affinity_t;
using pcat::threadPool_t;
using namespace std::literals::chrono_literals;

constexpr static std::size_t operator ""_uz(const unsigned long long value) noexcept { return value; }
constexpr static auto totalLoopIterations = 1000000_uz;

std::mutex workMutex;
std::condition_variable workCond;

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
		workCond.notify_all();
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
		puts("valid");
		suite.assertTrue(pool.valid());
		puts("ready");
		suite.assertTrue(pool.ready());
		puts("processors");
		const auto threads{pool.numProcessors()};
		suite.assertNotEqual(threads, 0);
		puts("burst queue");
		for (std::size_t i{}; i < threads; ++i)
			[[maybe_unused]] const auto result = pool.queue(threads - i);
		//suite.assertFalse(pool.ready());
		puts("queue threads+1");
		[[maybe_unused]] const auto result = pool.queue(threads);
		//std::this_thread::sleep_for(100ms);
		puts("queue after work completions");
		[]() noexcept
		{
			auto lock{std::unique_lock{workMutex}};
			workCond.wait(lock);
		}();
		suite.assertTrue(pool.queue(threads));
		puts("finish");
		suite.assertTrue(pool.finish());
		puts("invalid");
		suite.assertFalse(pool.valid());
		puts("already finished");
		suite.assertFalse(pool.finish());
	}
} // namespace threadPool
