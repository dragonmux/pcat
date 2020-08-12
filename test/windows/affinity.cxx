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
using pcat::args::argPinning_t;
using pcat::ulong_t;
using substrate::fixedVector_t;
using substrate::boundedIterator_t;

namespace affinity
{
	std::unique_ptr<affinity_t> affinity{};

	using processorVector_t = fixedVector_t<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>;
	using processorIterator_t = boundedIterator_t<const SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>;

	auto retrieveProcessorInfo(testsuite &suite)
	{
		fixedVector_t<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX> processorInfo{};
		ulong_t returnLength{};

		while (!GetLogicalProcessorInformationEx(RelationGroup, processorInfo.data(), &returnLength))
		{
			static_assert(sizeof(ulong_t) == sizeof(uint32_t));
			const auto error{static_cast<uint32_t>(GetLastError())};
			suite.assertEqual(error, ERROR_INSUFFICIENT_BUFFER);
			constexpr auto structLength{sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)};
			// Compute how many structs worth of information the kernel has to return..
			auto count{returnLength / structLength};
			count += returnLength - (count * structLength) ? 1 : 0;
			// Then allocate a fixedVector_t large enough for that.
			processorInfo = {count};
		}
		return processorInfo;
	}

	std::pair<uint16_t, uint8_t> nextProcessor(testsuite &suite, const processorVector_t &processorInfo,
		processorIterator_t &info, uint16_t &groupIndex, uint8_t &maskOffset)
	{
		while (info != processorInfo.end())
		{
			while (info->Relationship != RelationGroup)
			{
				++info;
				suite.assertTrue(info != processorInfo.end());
			}
			auto processor{*info};
			suite.assertLessThan(groupIndex, processor.Group.ActiveGroupCount);
			for (; groupIndex < processor.Group.ActiveGroupCount; ++groupIndex)
			{
				const auto &group{processor.Group.GroupInfo[groupIndex]};
				for (; maskOffset < sizeof(KAFFINITY) * 8; ++maskOffset)
				{
					const auto mask{group.ActiveProcessorMask >> maskOffset};
					if (mask & 1)
						return {groupIndex, maskOffset};
				}
				maskOffset = 0;
			}
			++info;
		}
		suite.fail("Could not find another valid group:processor combo while there are entries left in affinity_t");
		return {};
	}

	void testConstruct(testsuite &suite)
	{
		suite.assertNotNull(args);
		suite.assertNull(affinity);
		affinity = substrate::make_unique_nothrow<affinity_t>();
		suite.assertNotNull(affinity);
	}

	void testProcessorCount(testsuite &suite)
	{
		suite.assertNotNull(args);
		suite.assertNotNull(affinity);
		const auto procCount{GetActiveProcessorCount(ALL_PROCESSOR_GROUPS)};
		suite.assertGreaterThan(procCount, 0);
		suite.assertEqual(affinity->numProcessors(), procCount);
	}

	void testIteration(testsuite &suite)
	{
		suite.assertNotNull(args);
		suite.assertNotNull(affinity);
		const auto processorInfo{retrieveProcessorInfo(suite)};
		auto info{processorInfo.begin()};
		uint16_t groupIndex{};
		uint8_t maskOffset{};

		suite.assertTrue(affinity->begin() != affinity->end());
		suite.assertTrue(processorInfo.begin() != processorInfo.end());
		std::size_t count{0};
		for (const auto processor : *affinity)
		{
			suite.assertTrue(info != processorInfo.end());
			const auto core{nextProcessor(suite, processorInfo, info, groupIndex, maskOffset)};

			suite.assertEqual(processor.first, core.first);
			suite.assertEqual(processor.second, core.second);
			++maskOffset;
		}
		suite.assertEqual(count, affinity->numProcessors());
	}

	void testPinning(testsuite &suite)
	{
		size_t count{};
		suite.assertNotNull(args);
		suite.assertNotNull(affinity);
		for (const auto processor : *affinity)
		{
			const auto result = std::async(std::launch::async,
				[](testsuite &suite, const auto processor) noexcept -> GROUP_AFFINITY
				{
					affinity->pinThreadTo(processor);
					GROUP_AFFINITY result{};
					suite.assertTrue(GetThreadGroupAffinity(GetCurrentThread(), &result));
					return result;
				}, std::ref(suite), count++
			).get();
			suite.assertEqual(result.Mask, UINT64_C(1) << processor.second);
			suite.assertEqual(result.Group, processor.first);
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
			[&]() -> std::pair<uint16_t, uint8_t>
			{
				const auto processorInfo{retrieveProcessorInfo(suite)};
				auto info{processorInfo.begin()};
				uint16_t groupIndex{};
				uint8_t maskOffset{};
				return nextProcessor(suite, processorInfo, info, groupIndex, maskOffset);
			}()
		};

		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
		suite.assertTrue(args->add(substrate::make_unique<argThreads_t>("1"sv)));
		suite.assertEqual(args->count(), 1);
		affinity = substrate::make_unique_nothrow<affinity_t>();

		suite.assertNotNull(affinity);
		suite.assertEqual(affinity->numProcessors(), 1);
		suite.assertTrue(affinity->begin() != affinity->end());
		suite.assertEqual(affinity->begin()->first, processor.first);
		suite.assertEqual(affinity->begin()->second, processor.second);
	}

	void testUserPinning(testsuite &suite)
	{
		const auto processor
		{
			[&]() -> std::pair<uint16_t, uint8_t>
			{
				const auto processorInfo{retrieveProcessorInfo(suite)};
				auto info{processorInfo.begin()};
				uint16_t groupIndex{};
				uint8_t maskOffset{};
				return nextProcessor(suite, processorInfo, info, groupIndex, maskOffset);
			}()
		};

		args = substrate::make_unique_nothrow<pcat::args::argsTree_t>();
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
		suite.assertTrue(args->add(substrate::make_unique<argPinning_t>("0"sv)));
		suite.assertEqual(args->count(), 1);
		affinity = substrate::make_unique_nothrow<affinity_t>();

		suite.assertNotNull(affinity);
		suite.assertEqual(affinity->numProcessors(), 1);
		suite.assertTrue(affinity->begin() != affinity->end());
		suite.assertEqual(affinity->begin()->first, processor.first);
		suite.assertEqual(affinity->begin()->second, processor.second);
	}

	void testPinSecondCore(testsuite &suite)
	{
		const auto processor
		{
			[&]() -> std::pair<uint16_t, uint8_t>
			{
				if (GetActiveProcessorCount(ALL_PROCESSOR_GROUPS) < 2)
					suite.skip("This test can only be run on a multi-core machine with at "
						"least 2 cores in the allowed affinity set");
				const auto processorInfo{retrieveProcessorInfo(suite)};
				auto info{processorInfo.begin()};
				uint16_t groupIndex{};
				uint8_t maskOffset{};
				nextProcessor(suite, processorInfo, info, groupIndex, maskOffset);
				return nextProcessor(suite, processorInfo, info, groupIndex, maskOffset);
			}()
		};

		args = substrate::make_unique_nothrow<pcat::args::argsTree_t>();
		suite.assertNotNull(args);
		suite.assertEqual(args->count(), 0);
		suite.assertTrue(args->add(substrate::make_unique<argPinning_t>("1"sv)));
		suite.assertEqual(args->count(), 1);
		affinity = substrate::make_unique_nothrow<affinity_t>();

		suite.assertNotNull(affinity);
		suite.assertEqual(affinity->numProcessors(), 1);
		suite.assertTrue(affinity->begin() != affinity->end());
		suite.assertEqual(affinity->begin()->first, processor.first);
		suite.assertEqual(affinity->begin()->second, processor.second);
	}
} // namespace affinity
