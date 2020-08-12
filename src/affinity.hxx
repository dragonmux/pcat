#ifndef AFFINITY__HXX
#define AFFINITY__HXX

#include <vector>
#include <thread>
#include <algorithm>
#include <system_error>
#include <stdexcept>
#ifndef _WINDOWS
#	include <sched.h>
#else
#	include <utility>
#	define WIND32_LEAN_AND_MEAN
#	include <windows.h>
#	undef WIN32_LEAN_AND_MEAN
#	undef max
#	undef min
#	include <substrate/fixed_vector>
#endif
#include "args.hxx"
#include "indexSequence.hxx"

namespace pcat
{
#ifndef _WINDOWS
	struct affinity_t final
	{
	private:
		std::vector<uint32_t> processors;

		void pinTo(const pthread_t thread, const std::size_t index) const
		{
			if (index >= processors.size())
				throw std::out_of_range{"index into thread affinity object too large"};
			cpu_set_t affinity{};
			CPU_SET(processors[index], &affinity);
			pthread_setaffinity_np(thread, sizeof(cpu_set_t), &affinity);
		}

	public:
		affinity_t() : processors{}
		{
			const auto *const pinning{dynamic_cast<args::argPinning_t *>(::args->find(argType_t::pinning))};
			const auto *const threadCount{dynamic_cast<args::argThreads_t *>(::args->find(argType_t::threads))};
			cpu_set_t affinity{};
			if (sched_getaffinity(0, sizeof(cpu_set_t), &affinity) != 0)
				throw std::system_error{errno, std::system_category()};
			for (uint32_t i{0}; i < CPU_SETSIZE; ++i)
			{
				if (CPU_ISSET(i, &affinity))
				{
					if ((!threadCount || processors.size() < threadCount->threads()) &&
						(!pinning || std::find(pinning->begin(), pinning->end(), i) != pinning->end()))
						processors.push_back(i);
					CPU_CLR(i, &affinity);
				}
				else if (!CPU_COUNT(&affinity))
					break;
			}
		}

		[[nodiscard]] auto numProcessors() const noexcept { return processors.size(); }
		[[nodiscard]] auto begin() const noexcept { return processors.begin(); }
		[[nodiscard]] auto end() const noexcept { return processors.end(); }
		[[nodiscard]] auto indexSequence() const noexcept { return indexSequence_t{0, numProcessors()}; }

		void pinThreadTo(std::thread &thread, const std::size_t index) const
			{ pinTo(thread.native_handle(), index); }
		void pinThreadTo(const std::size_t index) const { pinTo(pthread_self(), index); }
	};
#else
	using substrate::fixedVector_t;
	using ulong_t = unsigned long;

	struct affinity_t final
	{
	private:
		std::vector<std::pair<uint16_t, uint8_t>> processors;

		void pinTo(const HANDLE thread, const std::size_t index) const
		{
			if (index >= processors.size())
				throw std::out_of_range{"index into thread affinity object too large"};
			const auto &[group, processor] = processors[index];
			const auto groupMask = UINT64_C(1) << processor;
			const GROUP_AFFINITY affinity{groupMask, group, {}};
			SetThreadGroupAffinity(thread, &affinity, nullptr);
		}

		[[nodiscard]] auto retrieveProcessorInfo() const
		{
			fixedVector_t<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX> processorInfo{};
			ulong_t returnLength{};

			while (!GetLogicalProcessorInformationEx(RelationGroup, processorInfo.data(), &returnLength))
			{
				const auto error{GetLastError()};
				if (error == ERROR_INSUFFICIENT_BUFFER)
				{
					constexpr auto structLength{sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)};
					// Compute how many structs worth of information the kernel has to return..
					auto count{returnLength / structLength};
					count += returnLength - (count * structLength) ? 1 : 0;
					// Then allocate a fixedVector_t large enough for that.
					processorInfo = {count};
				}
				else
					throw std::system_error{static_cast<int>(error), std::system_category()};
			}
			return processorInfo;
		}

	public:
		affinity_t() : processors{}
		{
			const auto processorInfo{retrieveProcessorInfo()};

			for (const auto &processor : processorInfo)
			{
				if (processor.Relationship != RelationGroup)
					continue;
				for (uint16_t groupIndex{}; groupIndex < processor.Group.ActiveGroupCount; ++groupIndex)
				{
					const auto &group{processor.Group.GroupInfo[groupIndex]};
					auto mask{group.ActiveProcessorMask};
					for (uint8_t i{}; i < sizeof(KAFFINITY) * 8; ++i)
					{
						if (mask & 1)
							processors.emplace_back(groupIndex, i);
						mask >>= 1;
						if (!mask)
							break;
					}
				}
			}
		}

		[[nodiscard]] auto numProcessors() const noexcept { return processors.size(); }
		[[nodiscard]] auto begin() const noexcept { return processors.begin(); }
		[[nodiscard]] auto end() const noexcept { return processors.end(); }
		[[nodiscard]] auto indexSequence() const noexcept { return indexSequence_t{0, numProcessors()}; }

		void pinThreadTo(std::thread &thread, const std::size_t index) const
			{ pinTo(thread.native_handle(), index); }
		void pinThreadTo(const std::size_t index) const { pinTo(GetCurrentThread(), index); }
	};
#endif /*_WINDOWS*/
} // namespace pcat

#endif /*AFFINITY__HXX*/
