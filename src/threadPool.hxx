#ifndef THREAD_POOL__HXX
#define THREAD_POOL__HXX

#include <cstdint>
#include <array>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <tuple>
#include <utility>
#include "affinity.hxx"
#include "threadedQueue.hxx"

namespace pcat
{
	template<typename workFunc_t> struct threadPool_t;

	template<typename result_t, typename... args_t> struct threadPool_t<result_t(args_t...)> final
	{
	private:
		using workFunc_t = result_t (*)(args_t...);
		std::atomic<size_t> waitingThreads{};
		std::mutex workMutex{};
		std::condition_variable haveWork{};
		std::tuple<args_t...> workItem{args_t{}...};
		bool workValid{false};
		std::atomic<bool> finished{false};
		threadedQueue_t<int32_t> results{};
		std::vector<std::thread> threads{};
		affinity_t affinity{};
		workFunc_t workerFunction;

		std::pair<bool, std::tuple<args_t...>> waitWork() noexcept
		{
			std::unique_lock<std::mutex> lock{workMutex};
			++waitingThreads;
			// wait, but protect ourselves from accidental wake-ups..
			haveWork.wait(lock, [this]() noexcept -> bool { return finished || workValid; });
			--waitingThreads;
			if (workValid)
			{
				workValid = false;
				return {true, workItem};
			}
			return {false, {}};
		}

		template<size_t... indicies> result_t invoke(std::tuple<args_t...> &&args, std::index_sequence<indicies...>)
			{ return workerFunction(std::get<indicies>(std::move(args))...); }

		void workerThread(const int32_t processor)
		{
			affinity.pinThreadTo(processor);
			while (!finished)
			{
				auto [valid, args] = waitWork();
				// This checks for both if we don't have something to do and if we're supposed to be finsihing up
				if (finished && !valid)
					break;
				auto result = invoke(std::move(args), std::make_index_sequence<sizeof...(args_t)>());
				results.push(std::move(result));
			}
		}

	public:
		threadPool_t(const workFunc_t function) : workerFunction{function}
		{
			for (const uint32_t processor : affinity)
				threads.emplace_back(std::thread{[this](const int32_t processor) -> void
					{ workerThread(processor); }, processor});
		}
		threadPool_t(const threadPool_t &) = delete;
		threadPool_t(threadPool_t &&) = delete;
		~threadPool_t() noexcept { [[maybe_unused]] const auto result = finish(); }
		threadPool_t &operator =(const threadPool_t &) = delete;
		threadPool_t &operator =(threadPool_t &&) = delete;

		[[nodiscard]] size_t numProcessors() const noexcept { return affinity.numProcessors(); }

		[[nodiscard]] result_t queue(args_t &...args)
			{ return queue(std::move(args)...); }
		[[nodiscard]] result_t queue(args_t &&...args)
		{
			result_t result{};
			if (!waitingThreads)
				result = results.pop();
			std::lock_guard<std::mutex> lock{workMutex};
			workItem = {std::forward<args_t>(args)...};
			workValid = true;
			haveWork.notify_one();
			return result;
		}

		[[nodiscard]] result_t finish()
		{
			if (threads.empty())
				return {};
			finished = true;
			haveWork.notify_all();
			for (auto &thread : threads)
				thread.join();
			threads.clear();

			result_t result{};
			while (!results.empty())
			{
				const auto thisResult = results.pop();
				if (!result)
					result = thisResult;
			}
			return result;
		}
	};

	template<typename result_t, typename... args_t>
		threadPool_t(result_t (*)(args_t...)) -> threadPool_t<result_t(args_t...)>;
} // namespace pcat

#endif /*THREAD_POOL__HXX*/
