#include <threadedQueue.hxx>
#include "testThreadedQueue.hxx"

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
		suite.assertEqual(queue.pop(), 5);
		suite.assertFalse(queue.empty());
		suite.assertEqual(queue.size(), 2);
		suite.assertEqual(queue.pop(), 10);
		suite.assertFalse(queue.empty());
		suite.assertEqual(queue.size(), 1);
	}
}
