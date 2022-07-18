#pragma once
#include <thread>
#include <mutex>
#include <functional>
#include <vector>
#include <list>
#include <array>

#include "Containers/LIWThreadSafeQueueSized.h"
#include "LIWFiberTask.h"
#include "LIWFiberMain.h"
#include "LIWFiberWorker.h"
#include "LIWThread.h"


namespace LIW {
	template<uint64_t FiberCount, 
			uint64_t AwakeFiberCount, 
			uint64_t TaskCount,
			uint32_t SyncCounterCount>
	class LIWFiberThreadPoolSized
	{
	public:
		typedef Util::LIWThreadSafeQueueSized<LIWFiberWorker*, FiberCount> fiber_queue_type;
		typedef std::array<LIWFiberWorker*, FiberCount> fiber_array_type;
		typedef Util::LIWThreadSafeQueueSized<LIWFiberWorker*, AwakeFiberCount> awake_fiber_queue_type;
		//typedef Util::LIWThreadSafeQueueSized<LIWFiberTask*, TaskCount> task_queue_type;
		typedef Util::LIWThreadSafeQueueSized<LIWFiberTask*, TaskCount> task_queue_type;
		typedef typename fiber_queue_type::size_type size_type;
		typedef uint32_t counter_size_type;
		typedef int counter_type;
		typedef std::atomic<counter_type> atomic_counter_type;
		typedef std::mutex mtx_type;
		typedef std::lock_guard<mtx_type> lkgd_type;

		typedef void(*ThreadInitializer)(void* param);
		typedef void(*ThreadCleaner)(void* param);
		struct ThreadParam
		{
			int m_threadID{ -1 };
		};
		

	private:
		struct LIWFiberSyncCounter {
			friend class LIWFiberThreadPoolSized;
		public:
			typename atomic_counter_type m_counter;
			std::mutex m_mtx;
			//std::list<LIWFiberWorker*> m_dependents;
			LIWFiberWorker* m_dependent;
		};
	public:
		typedef std::array<LIWFiberSyncCounter, SyncCounterCount> sync_counter_array_type;

	public:
		LIWFiberThreadPoolSized() :
			m_isRunning(false), m_isInit(false) {}
		virtual ~LIWFiberThreadPoolSized() {}

		/// <summary>
		/// Initialize. 
		/// </summary>
		/// <param name="numWorkers"> number of workers (threads) </param>
		/// <param name="numFibers"> number of fibers (shared among threads) </param>
		void Init(int numWorkers, int threadIDStart) {
			Init(numWorkers, numWorkers, threadIDStart);
		}
		void Init(int minWorkers, int maxWorkers, int threadIDStart) {
			//TODO: Make this adaptive somehow
			m_isRunning = true;

			for (size_type i = 0; i < FiberCount; ++i) {
				LIWFiberWorker* worker = new LIWFiberWorker((int)i);
				m_fibers.push_now(worker);
				m_fibersRegistered[i] = worker;
			}

			for (int i = 0; i < minWorkers; ++i) {
				ThreadParam param{ i + threadIDStart };
				m_workers.emplace_back(PoolThread, this, param);
			}

			m_isInit = true;
		}

		/// <summary>
		/// Is thread pool initialized? 
		/// </summary>
		/// <returns> is initialized </returns>
		inline bool IsInit() const { return m_isInit; }
		/// <summary>
		/// Is thread pool still running? 
		/// </summary>
		/// <returns> is running </returns>
		inline bool IsRunning() const { return m_isRunning; }

		inline size_type GetTaskCount() const { return m_tasks.size(); }

		/// <summary>
		/// Submit task for the thread pool to execute. 
		/// </summary>
		/// <param name="task"> task to execute </param>
		/// <returns> is operation successful? </returns>
		inline bool Submit(LIWFiberTask* task) {
			if (!m_tasks.push_now(task))
				throw "task queue size too small. Decrease task submission rate/Increase task queue size. ";
			return true;
		}

		/// <summary>
		/// Wait for all the submited tasked to be executed before stopping. 
		/// </summary>
		void WaitAndStop() {
			m_isRunning = false;
			m_tasks.block_till_empty();
			m_fibersAwakeList.block_till_empty();
			for (auto& fiber : m_fibersRegistered) {
				fiber->Stop();
			}
			using namespace std::chrono;
			std::this_thread::sleep_for(1ms);
			m_tasks.notify_stop();
			m_fibersAwakeList.notify_stop();

			for (int i = 0; i < m_workers.size(); ++i) {
				m_workers[i].join();
			}
		}
		/// <summary>
		/// Stop after execution of currently executing tasks. Ignore others enqueued. 
		/// </summary>
		void Stop() {
			m_isRunning = false;

			LIWFiberTask* task;
			while (!m_tasks.empty()) {
				task = nullptr;
				m_tasks.pop(task);
				delete task;
			}

			for (auto& fiber : m_fibersRegistered) {
				fiber->Stop();
			}
			using namespace std::chrono;
			std::this_thread::sleep_for(1ms);
			m_tasks.notify_stop();

			for (int i = 0; i < m_workers.size(); ++i) {
				m_workers[i].join();
			}
		}


		/*
		* Fiber task waiting
		*/
		
		///// <summary>
		///// Add a fiber worker as the dependency of a sync counter. 
		///// </summary>
		///// <param name="idxCounter"> index of the sync counter </param>
		///// <param name="worker"> dependent fiber worker </param>
		///// <returns> is operation successful? </returns>
		//inline bool AddDependencyToSyncCounter(counter_size_type idxCounter, LIWFiberWorker* worker) {
		//	LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];
		//	lkgd_type lk(counter.m_mtx);
		//	counter.m_dependents.emplace_back(worker);
		//	return true;
		//}
		inline bool WaitOnSyncCounter(counter_size_type idxCounter, LIWFiberWorker* worker) {
			LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];

			counter.m_mtx.lock();
			if (counter.m_counter > 0) {
				//TODO: I think this is the issue!!!!!!!
				// thread A fiber can add dependency->unlock->
				// thread B fiber decreaseSyncCounter->add to awake list
				// thread C fetch from awakelist->finish->set to idle
				// thread A yeild to main->check idle
				//counter.m_dependents.emplace_back(worker);
				counter.m_dependent = worker;
				worker->m_fiberMain->m_counter = idxCounter;

				// SOMETHING BAD CAN HAPPEN HERE!!!
				// Another thread can decrease counter and start invoking this fiber
				worker->YieldToMain();
			}
			else
			{
				counter.m_mtx.unlock();
			}
			return true;

			//LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];
			//counter.m_mtx.lock();
			//if (counter.m_counter > 0) {
			//	worker->m_synCounter = idxCounter;
			//	counter.m_mtx.unlock();
			//}
			//else
			//{
			//	counter.m_mtx.unlock();
			//}
			//return true;
		}
		/// <summary>
		/// Increase a sync counter. 
		/// </summary>
		/// <param name="idxCounter"> index of the sync counter </param>
		/// <param name="increase"> amount to increase (>0) </param>
		/// <returns> sync counter after increment </returns>
		inline counter_type IncreaseSyncCounter(counter_size_type idxCounter, counter_type increase = 1) {
			LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];
			lkgd_type lock(counter.m_mtx);
			return m_syncCounters[idxCounter].m_counter.fetch_add(increase) + increase;
		}
		/// <summary>
		/// Decrease a sync counter (by 1).
		/// When sync counter reaches 0, the fiber worker will be awaken and put into awake list. 
		/// </summary>
		/// <param name="idxCounter"> index of the sync counter </param>
		/// <param name="decrease"> amount to decrease (>0) </param>
		/// <returns> sync counter after decrement </returns>
		counter_type DecreaseSyncCounter(counter_size_type idxCounter, counter_type decrease = 1) {
			LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];
			lkgd_type lock(counter.m_mtx);
			int val = counter.m_counter.fetch_sub(decrease) - decrease;
			if (val <= 0) { // Counter reach 0, move all dependents to awake list
				/*while (!counter.m_dependents.empty()) {
					m_fibersAwakeList.push_now(counter.m_dependents.front());
					counter.m_dependents.pop_front();
				}*/
				if (counter.m_dependent != nullptr) {
					m_fibersAwakeList.push_now(counter.m_dependent);
					counter.m_dependent = nullptr;
				}
			}
			return val;
		}
		/// <summary>
		/// Get the value of a sync counter. 
		/// </summary>
		/// <param name="idxCounter"> index of the sync counter </param>
		/// <returns> sync counter value </returns>
		inline counter_type GetSyncCounter(counter_size_type idxCounter) const {
			return m_syncCounters[idxCounter].m_counter.load(std::memory_order_relaxed);
		}

	private:
		// Fiber Management
		fiber_queue_type m_fibers;
		fiber_array_type m_fibersRegistered;
		// Fiber waiting Management
		awake_fiber_queue_type m_fibersAwakeList;
		sync_counter_array_type m_syncCounters;
		// Worker threads
		std::vector<std::thread> m_workers;
		// Task queue
		task_queue_type m_tasks;

	private:
		void* fibers[9] = {0,0,0,0,0,0,0,0,0};
		std::mutex mtx;

		/// <summary>
		/// Loop function to process task. 
		/// </summary>
		static void PoolThread(LIWFiberThreadPoolSized* thisTP, const ThreadParam& param) {
			// Register thread
			LIWThreadRegisterID(param.m_threadID);
			// Init thread
			LIWThreadInit();

			// Start running fibers
			const int threadID = param.m_threadID;
			LIWFiberMain* fiberMain = LIWFiberMain::InitThreadMainFiber(threadID);
			LIWFiberTask* task = nullptr;
			LIWFiberWorker* fiber = nullptr;
			while (true) {
				if (!thisTP->m_tasks.empty() ||
					!thisTP->m_fibersAwakeList.empty() ||
					thisTP->m_isRunning) {

					fiber = nullptr;
					if (!thisTP->m_fibersAwakeList.empty()) {
						if (thisTP->m_fibersAwakeList.pop_now(fiber)) { // Acquire fiber from awake fiber list. 
																		// Set fiber to perform task

							fiber->SetMainFiber(fiberMain);

							//printf("Resume fb%d thd%d\n", fiber->m_id, threadID);
							// Switch to fiber
							fiberMain->YieldTo(fiber);

							if (fiberMain->m_counter != UINT32_MAX) {
								auto& counter = thisTP->m_syncCounters[fiberMain->m_counter];
								fiberMain->m_counter = UINT32_MAX;
								counter.m_mtx.unlock();
							}
							else if (fiber->GetState() != LIWFiberState::Running) { // If fiber is not still running (meaning yielded manually), return for reuse. 
								thisTP->m_fibers.push_now(fiber);
							}
						}
					}
					else if (!thisTP->m_tasks.empty()) {
						if (thisTP->m_fibers.pop_now(fiber)) { // Acquire fiber from idle fiber list. //TODO: Currently this is spinning when empty. Make it wait. 
							if (thisTP->m_tasks.pop_now(task)) { // Acquire task
																// Set fiber to perform task

								fiber->SetMainFiber(fiberMain);
								fiber->SetRunTask(task);

								//printf("NewStart fb%d thd%d\n", fiber->m_id, threadID);
								// Switch to fiber
								fiberMain->YieldTo(fiber);
							}

							if (fiberMain->m_counter != UINT32_MAX) {
								auto& counter = thisTP->m_syncCounters[fiberMain->m_counter];
								fiberMain->m_counter = UINT32_MAX;
								counter.m_mtx.unlock();
							}
							else if (fiber->GetState() != LIWFiberState::Running) { // If fiber is not still running (meaning yielded manually), return for reuse. 
								thisTP->m_fibers.push_now(fiber);
							}
						}
					}

				}
				else
				{
					if (thisTP->m_isRunning)
						std::this_thread::yield();
					else
						break;
				}
			}

			// Cleanup thread
			LIWThreadCleanup();
		}

	private:
		bool m_isRunning;
		bool m_isInit;
	};
}



