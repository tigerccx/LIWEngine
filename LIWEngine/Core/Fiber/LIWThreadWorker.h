#pragma once

#include <mutex>
#include <atomic>

#include "LIWThreadWorkerTask.h"
#include "Containers/LIWThreadSafeQueueSized.h"

namespace LIW {
	template<size_t TaskCount>
	class LIWThreadWorker final{
	public:
		typedef Util::LIWThreadSafeQueueSized<LIWThreadWorkerTask*, TaskCount> task_queue_type;
	public:
		LIWThreadWorker() {}
		~LIWThreadWorker() {}

		/// <summary>
		/// Initialize. 
		/// </summary>
		void Init() {
			m_isRunning = true;
			m_isInit = true;
		}

		/// <summary>
		/// Is worker initialized? 
		/// </summary>
		/// <returns> is initialized </returns>
		inline bool IsInit() const { return m_isInit; }
		/// <summary>
		/// Is worker still running? 
		/// </summary>
		/// <returns> is running </returns>
		inline bool IsRunning() const { return m_isRunning; }
		/// <summary>
		/// Get the capacity of the task queue. 
		/// </summary>
		/// <returns> capacity of the task queue </returns>
		inline constexpr uint64_t GetCapacity() const { return TaskCount; }

		/// <summary>
		/// Submit task for the thread pool to execute when task queue is not full. 
		/// </summary>
		/// <param name="task"> task to execute </param>
		/// <returns> operation successful? </returns>
		inline bool Submit(LIWThreadWorkerTask* task) {
			return m_tasks.push(task);
		}

		/// <summary>
		/// Submit task for the thread pool to execute immediately. 
		/// </summary>
		/// <param name="task"> task to execute </param>
		/// <returns> operation successful? </returns>
		inline bool SubmitNow(LIWThreadWorkerTask* task) {
			return m_tasks.push_now(task);
		}

		/// <summary>
		/// Wait for all the submited tasked to be executed before stopping. 
		/// </summary>
		void WaitAndStop() {
			NotifyStop();

			m_tasks.block_till_empty();
			using namespace std::chrono;
			std::this_thread::sleep_for(1ms);
			m_tasks.notify_stop();
		}

		/// <summary>
		/// Stop after execution of currently executing tasks. Ignore others enqueued. 
		/// </summary>
		void Stop() {
			NotifyStop();

			LIWThreadWorkerTask* task;
			while (!m_tasks.empty()) { //TODO: Could lead to memory leak
									   //	   When task param is a dynamic var allocated at another point. 
				task = nullptr;
				m_tasks.pop(task);
				delete task;
			}
			using namespace std::chrono;
			std::this_thread::sleep_for(1ms);
			m_tasks.notify_stop();
		}

		/// <summary>
		/// Notify thread worker to stop. (Actual stop will happen when m_tasks is empty. )
		/// </summary>
		inline void NotifyStop() {
			m_isRunning = false;
		}
		inline void WaitTillEmpty() {
			m_tasks.block_till_empty();
		}


		/// <summary>
		/// Loop function to process task. 
		/// </summary>
		void Run() {
			while (!m_tasks.empty() || m_isRunning.load()) {
				LIWThreadWorkerTask* task = nullptr;
				if (m_tasks.pop(task)) {
					task->Execute();
					delete task;
				}
			}
		}

	private:
		task_queue_type m_tasks;

		std::atomic<bool> m_isRunning{ false };
		bool m_isInit{ false };
	};
}