#include <future>
#include <memory>
#include <string_view>
#include <substrate/utility>
#include <affinity.hxx>
#include <args.hxx>
#include "testAffinity.hxx"

using namespace std::literals::string_view_literals;
using pcat::affinity_t;
using pcat::args::argThreads_t;

namespace affinity
{
	std::unique_ptr<affinity_t> affinity{};

	void testConstruct(testsuite &suite)
	{
		suite.assertNull(affinity);
		affinity = substrate::make_unique_nothrow<affinity_t>();
		suite.assertNotNull(affinity);
	}

	void testProcessorCount(testsuite &suite)
	{
		suite.assertNotNull(affinity);
		cpu_set_t affinitySet{};
		suite.assertEqual(sched_getaffinity(0, sizeof(cpu_set_t), &affinitySet), 0);

		const auto procCount{CPU_COUNT(&affinitySet)};
		suite.assertEqual(affinity->numProcessors(), procCount);
	}

	void testIteration(testsuite &suite)
	{
		suite.assertNotNull(affinity);
		cpu_set_t affinitySet{};
		suite.assertEqual(sched_getaffinity(0, sizeof(cpu_set_t), &affinitySet), 0);

		suite.assertTrue(affinity->begin() != affinity->end());
		std::size_t count{0};
		for (const auto processor : *affinity)
		{
			++count;
			suite.assertTrue(CPU_ISSET(processor, &affinitySet));
		}
		suite.assertEqual(count, affinity->numProcessors());
	}

	void testPinning(testsuite &suite)
	{
		suite.assertNotNull(affinity);
		for (const auto processor : *affinity)
		{
			const auto result = std::async(std::launch::async, [](const uint32_t processor) noexcept -> int32_t
			{
				affinity->pinThreadTo(processor);
				return sched_getcpu();
			}, processor).get();
			suite.assertNotEqual(result, -1);
			suite.assertEqual(result, processor);
		}

		suite.assertTrue(std::async(std::launch::async, []() noexcept -> bool
		{
			try { affinity->pinThreadTo(affinity->numProcessors()); }
			catch (const std::out_of_range &) { return true; }
			return false;
		}).get());
	}

	void testThreadCap(testsuite &suite)
	{
		const auto processor
		{
			[&]() noexcept -> uint32_t
			{
				cpu_set_t affinitySet{};
				suite.assertEqual(sched_getaffinity(0, sizeof(cpu_set_t), &affinitySet), 0);
				for (uint32_t i{0}; i < CPU_SETSIZE; ++i)
				{
					if (CPU_ISSET(i, &affinitySet))
						return i;
				}
				static_assert(CPU_SETSIZE < UINT32_MAX);
				return UINT32_MAX;
			}()
		};

		suite.assertNotEqual(processor, UINT32_MAX);
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
		suite.assertTrue(args->add(substrate::make_unique<argThreads_t>("1"sv)));
		suite.assertEqual(args->count(), 1);
		affinity = substrate::make_unique_nothrow<affinity_t>();

		suite.assertNotNull(affinity);
		suite.assertEqual(affinity->numProcessors(), 1);
		suite.assertTrue(affinity->begin() != affinity->end());
		suite.assertEqual(*affinity->begin(), processor);
	}
} // namespace affinity
