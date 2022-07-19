#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <list>
#include <array>

#include "Containers/LIWThreadSafeQueueSized.h"
#include "LIWFiberTask.h"
#include "LIWFiberMain.h"
#include "LIWFiberWorker.h"
#include "LIWThread.h"
#include "Memory/LIWMemory.h"


namespace LIW {
	template<uint64_t FiberCount, 
			uint64_t AwakeFiberCount, 
			uint64_t TaskCount,
			uint32_t SyncCounterCount>
	class LIWFiberThreadPoolSized
	{
	public:
		typedef Util::LIWThreadSafeQueueSized<LIWFiberWorkerPointer, FiberCount> fiber_queue_type;
		typedef std::array<LIWFiberWorkerPointer, FiberCount> fiber_array_type;
		typedef Util::LIWThreadSafeQueueSized<LIWFiberWorkerPointer, AwakeFiberCount> awake_fiber_queue_type;
		//typedef Util::LIWThreadSafeQueueSized<LIWFiberTaskPointer, TaskCount> task_queue_type;
		typedef Util::LIWThreadSafeQueueSized<LIWFiberTaskPointer, TaskCount> task_queue_type;
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
			//std::list<LIWFiberWorkerPointer> m_dependents;
			LIWFiberWorkerPointer m_dependent;
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

			m_fibers.m_cvCondNonEmptyNotify = &m_cvRequireExecution;
			m_fibersAwakeList.m_cvCondNonEmptyNotify = &m_cvRequireExecution;

			for (size_type i = 0; i < FiberCount; ++i) {
				LIWFiberWorkerPointer worker = liw_new_static<LIWFiberWorker>((int)i);
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
		inline bool Submit(LIWFiberTaskPointer task) {
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
			std::this_thread::sleep_for(1ms); //Wait for everyone to go in condition wait
			m_tasks.notify_stop();
			m_fibersAwakeList.notify_stop();

			m_cvRequireExecution.notify_all();

			for (int i = 0; i < m_workers.size(); ++i) {
				m_workers[i].join();
			}
		}
		/// <summary>
		/// Stop after execution of currently executing tasks. Ignore others enqueued. 
		/// </summary>
		void Stop() {
			m_isRunning = false;

			LIWFiberTaskPointer task;
			while (!m_tasks.empty()) {
				task = nullptr;
				m_tasks.pop(task);
				delete task;
			}

			for (auto& fiber : m_fibersRegistered) {
				fiber->Stop();
			}
			using namespace std::chrono;
			std::this_thread::sleep_for(1ms); //Wait for everyone to go in condition wait
			m_tasks.notify_stop();
			m_fibersAwakeList.notify_stop();

			m_cvRequireExecution.notify_all();

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
		//inline bool AddDependencyToSyncCounter(counter_size_type idxCounter, LIWFiberWorkerPointer worker) {
		//	LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];
		//	lkgd_type lk(counter.m_mtx);
		//	counter.m_dependents.emplace_back(worker);
		//	return true;
		//}
		
		/// <summary>
		/// Wait for a sync counter to hit 0;
		/// </summary>
		/// <param name="idxCounter"> index of the sync counter </param>
		/// <param name="worker"> dependent fiber worker </param>
		/// <returns> is operation successful? </returns>
		inline bool WaitOnSyncCounter(counter_size_type idxCounter, LIWFiberWorkerPointer worker) {
			LIWFiberSyncCounter& counter = m_syncCounters[idxCounter];

			counter.m_mtx.lock();
			if (counter.m_counter > 0) {
				counter.m_dependent = worker;
				worker->m_fiberMain->m_counter = idxCounter;

				// counter.m_mtx is not unlocked here because...
				// SOMETHING BAD CAN HAPPEN HERE!!!
				// Another thread can decrease counter and start invoking this fiber, causing fiber execution stack corruption. 
				// So to make adding to wait and yielding into an atomic op, unlock is moved to mainFiber. 
				//TODO: Figure out a way to do this more elegantly. 
				worker->YieldToMain();
			}
			else
			{
				counter.m_mtx.unlock();
			}
			return true;
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
				if (!counter.m_dependent.is_null()) {
					m_fibersAwakeList.push_now(counter.m_dependent);
					counter.m_dependent.set_null();
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
		/// <summary>
		/// Loop function to process task. 
		/// </summary>
		static void PoolThread(LIWFiberThreadPoolSized* thisTP, const ThreadParam& param) {
			using namespace std::chrono;
			
			// Register thread
			LIWThreadRegisterID(param.m_threadID);
			// Init thread
			LIWThreadInit();


			// Start running fibers
			const int threadID = param.m_threadID;
			LIWFiberMain* fiberMain = LIWFiberMain::InitThreadMainFiber(threadID);
			LIWFiberTaskPointer task{liw_c_nullhdl};
			LIWFiberWorkerPointer fiber{liw_c_nullhdl};

			while (true) {
				fiber.set_null();
				task.set_null();

				// Try a few times before going into conditional wait
				int trial = 0;
				const int trialMax = 10; 
				for (trial = 0; trial < trialMax; trial++) {
					if (thisTP->m_fibersAwakeList.pop_now(fiber) || //Any awoken fiber?
						thisTP->m_tasks.pop_now(task) || //Any task?
						!thisTP->m_isRunning) //Or I am breaking?
						break;
				}
				if (trial == trialMax) { // Fine... All trials are failed. Let's wait...
					std::unique_lock<std::mutex> lk(thisTP->m_mtxData);
					while (fiber.is_null() && task.is_null()) {
						thisTP->m_cvRequireExecution.wait_for(lk, 1ms);
						if (thisTP->m_fibersAwakeList.pop_now(fiber) || //Any awoken fiber?
							thisTP->m_tasks.pop_now(task) || //Any task?
							!thisTP->m_isRunning) //Or I am breaking?
							break;
					}
					lk.unlock();
				}
				
				//NOTE: At this point of execution, either fiber or task must be not null. 
				//		Or it means it's breaking because it's exiting. 

				if (!fiber.is_null()) { // Acquired fiber from awake fiber list. 
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
				else if (!task.is_null()) {
					thisTP->m_fibers.pop(fiber); // Acquire fiber from idle fiber list. 

					fiber->SetMainFiber(fiberMain);
					fiber->SetRunTask(task);

					//printf("NewStart fb%d thd%d\n", fiber->m_id, threadID);
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
				else {
					printf("Worker thread %d Exit.. \n", threadID);
					break;
				}
			}

			// Cleanup thread
			LIWThreadCleanup();
		}

	private:
		bool m_isRunning;
		bool m_isInit;
		//TODO: Probably combine LIWThreadSafeQueueSized into this so there will be less conditional variables. 
		std::mutex m_mtxData;
		std::condition_variable m_cvRequireExecution;
	};
}



