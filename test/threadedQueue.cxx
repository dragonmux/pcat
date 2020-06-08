#include <thread>
#include <chrono>
#include <future>
#include <threadedQueue.hxx>
#include "testThreadedQueue.hxx"
#include "latch.hxx"

using namespace std::literals::chrono_literals;
using pcat::threadedQueue_t;

namespace threadedQueue
{
	threadedQueue_t<std::int32_t> queue;

	void testEmpty(testsuite &suite)
	{
		suite.assertTrue(queue.empty());
		suite.assertEqual(queue.size(), 0);
	}

	void testEmplace(testsuite &suite)
	{
		queue.emplace(5);
		suite.assertFalse(queue.empty());
		suite.assertEqual(queue.size(), 1);
	}

	void testPush(testsuite &suite)
	{
		queue.push(10);
		suite.assertFalse(queue.empty());
		suite.assertEqual(queue.size(), 2);
		queue.push(15);
	}

	void testPop(testsuite &suite)
	{
		suite.assertFalse(queue.empty());
		suite.assertEqual(queue.size(), 3);
		suite.assertEqual(queue.pop(), 5);
		suite.assertFalse(queue.empty());
		suite.assertEqual(queue.size(), 2);
		suite.assertEqual(queue.pop(), 10);
		suite.assertEqual(queue.pop(), 15);
		suite.assertTrue(queue.empty());
		suite.assertEqual(queue.size(), 0);
	}

	void testAsync(testsuite &suite)
	{
		pcat::latch_t barrier{2};
		suite.assertTrue(queue.empty());
		suite.assertEqual(queue.size(), 0);

		auto result = std::async(std::launch::async, [&]() noexcept -> std::int32_t
		{
			barrier.arriveAndWait();
			return queue.pop();
		});
		barrier.arriveAndWait();
		std::this_thread::sleep_for(25us);

		queue.push(-100);
		suite.assertEqual(result.get(), -100);
		suite.assertTrue(queue.empty());
		suite.assertEqual(queue.size(), 0);
	}
} // namespace threadedQueue
