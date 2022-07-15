#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Application/Window.h"
#include "Application/Environment.h"
#include "TestGame.h"
#include "TestRenderer.h"
#include "Editor/LIWEditorUIHelper.h"

#include "LIWConstants.h"
#include "Fiber/LIWThread.h"
#include "Memory/LIWMemory.h"
#include "Fiber/LIWFiberThreadPoolSized.h"
#include "Fiber/LIWThreadWorker.h"
#include "LIWFrame.h"

#include "TestRenderer.h"

void TT_TestUIDraw(LIW_THREADWORKER_RUNNER_PARAM);


#define LIW_FIBER_COUNT size_t(1) << 8
#define LIW_FIBER_AWAKE_COUNT size_t(1) << 10
#define LIW_FIBER_TASK_COUNT size_t(1) << 16
#define LIW_FIBER_SYNC_COUNTER_COUNT size_t(1) << 10

#define LIW_WORKER_MAIN_TASK_COUNT size_t(1) << 16

namespace LIW {
	class LIWCore {
	public:
		typedef std::mutex mtx_type;
		typedef std::unique_lock<std::mutex> uniqlk_type;
		typedef std::condition_variable condvar_type;
		typedef LIW::LIWFiberThreadPoolSized<LIW_FIBER_COUNT, LIW_FIBER_AWAKE_COUNT, LIW_FIBER_TASK_COUNT, LIW_FIBER_SYNC_COUNTER_COUNT> fiber_thdpool_type;
		typedef LIW::LIWThreadWorker<LIW_WORKER_MAIN_TASK_COUNT> main_thdwkr_type;

	public:
		int Boot() {
			// Time start from this point
			m_timeStart = m_timeFrame = std::chrono::system_clock::now();
			
			// Decide how many threads to use
			int countWorkerThreads = std::thread::hardware_concurrency();
			if (countWorkerThreads == 0)
				countWorkerThreads = 16;
			
			countWorkerThreads = 2;

			int countThreads = countWorkerThreads + 1;

			LIWThreads::s_ins.m_threadCount = countThreads;
			LIWThreadRegisterID(LIW_THREAD_IDX_MAIN);

			//
			// Init memory management
			// 

			// Init mem
			liw_minit_def(countThreads);
			liw_minit_static(countThreads);
			liw_minit_frame(countThreads);
			liw_minit_dframe(countThreads);

			// Count threads
			int idxThread = LIW_THREAD_IDX_MAIN;

			// Init mem for main thread
			LIWThreadInit();

			//
			// Init main thread worker
			//
			m_mainThreadWorker.Init();
			if (!m_mainThreadWorker.IsInit()) {
				throw "Main ThreadWorker init failed. ";
			}
			idxThread++;

			//
			// Init fiber thread pool
			//
			m_fiberThreadPool.Init(countWorkerThreads, idxThread);
			if (!m_fiberThreadPool.IsInit()) {
				throw "Fiber ThreadPool init failed. ";
			}
			idxThread += countWorkerThreads;

			// Init GLFW
			glfwSetErrorCallback(glfw_error_callback);
			if (!glfwInit()) {
				std::cout << "GLFW init failed!" << std::endl;
				glfwTerminate();
				return -1;
			}

			// Init Environment
			/* Window */
			m_window = liw_new_static<LIW::App::Window>("My First GLFW Window", 1280, 720, false);
			auto ptrWindow = m_window;
			if (!ptrWindow->Initialised()) {
				return -1;
			}
			/* Make the window's context current */
			ptrWindow->SetCurrent();
			/* Create environment */
			m_environment.m_window = ptrWindow; //TODO: warning! use handle...


			// Init GLEW
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				/* Problem: glewInit failed, something is seriously wrong. */
				std::cout << "GLEW init failed!" << std::endl;
				fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
				glfwTerminate();
				return -1;
			}

			// Init game
			
			m_game.InitGame(&m_environment);
			int codeGameInit = m_game.Initialise();
			if (codeGameInit)
				return codeGameInit;

			//
			// Setup ImGUI
			//
			// Setup ImGUI context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			//Setup ImGUI Style
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(ptrWindow->GetHandle(), true);
			ImGui_ImplOpenGL3_Init("#version 130");
			//State
			bool show_demo_window = true;
			bool show_another_window = true;




			//
			// Start!!!
			//
			// Make main thread wait
			m_isRunning.store(true);
			m_frameCount = -1;
			// Kick off the first fiber task
			m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_FrameBeg, 0 });

			return 0;
		}

		void MainThreadRun() {
			m_mainThreadWorker.Run();
		}

		void NotifyShutdown() {
			m_mainThreadWorker.WaitAndStop();
		}

		void Shutdown() {

			m_game.CleanUp();

			glfwTerminate();


			//
			// Shutdown fiber therad pool
			//
			m_fiberThreadPool.WaitAndStop();
			
			//
			// Shutdown main thread worker
			//
			m_mainThreadWorker.WaitTillEmpty();

			//
			// Shutdown memory management
			// 

			// Cleanup mem for main thread
			LIWThreadCleanup();

			// Cleanup mem
			liw_mclnup_dframe();
			liw_mclnup_frame();
			liw_mclnup_static();
			liw_mclnup_def();
		}

	private:
		static void glfw_error_callback(int error, const char* description)
		{
			fprintf(stderr, "Glfw Error %d: %s\n", error, description);
		}

	public:
		/// <summary>
		/// Begin a frame. 
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_FT_FrameBeg(LIW_FIBER_RUNNER_PARAM){
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			const double timeFromStart = double((std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - s_ins.m_timeStart)).count()) * 1e-9;
			const double timeFrame = double((std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - s_ins.m_timeFrame)).count()) * 1e-9;
			s_ins.m_timeFrame = timeNow;
			s_ins.m_frameCount++;

			printf("FrameBeg %llu -- LastFrame %lf(s) \n", s_ins.m_frameCount, timeFrame);
			
			auto ptrFrameData = liw_new_frame<LIWFrameData>(
				LIWFrameData{ 
					timeNow, 
					timeFrame, 
					timeFromStart,
					false
				});


			// Window update
			s_ins.m_fiberThreadPool.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, 2);
			s_ins.m_mainThreadWorker.Submit(new LIWThreadWorkerTask{ LIW_TT_WindowUpdate, (void*)ptrFrameData.get_handle() });
			s_ins.m_mainThreadWorker.Submit(new LIWThreadWorkerTask{ LIW_TT_EventPoll, nullptr });
			s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, thisFiber);

			if (ptrFrameData->m_singnalEnd) {
				s_ins.NotifyShutdown();
				return;
			}

			// Kick off the frame
			auto ptrTestRenderData = liw_new_frame<TestRenderData>(TestRenderData{ ptrFrameData, s_ins.m_game.m_renderer });
			
			s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_TestRenderRender, (void*)ptrTestRenderData.get_handle() });
			

			//TODO: properly kick off to game
			//liw_hdl_type hdlGameData = liw_new_frame<GameData>(GameData{ hdlFrameData, &s_ins.m_game });
			//
			//s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_TestRenderRender, (void*)hdlGameData });

		}

		/// <summary>
		/// Begin drawing editor UI
		/// </summary>
		/// <param name=""> LIWFrameData handle(frame) </param>
		static void LIW_FT_EDTR_UIDrawBeg(LIW_FIBER_RUNNER_PARAM) {
			s_ins.m_fiberThreadPool.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
			s_ins.m_mainThreadWorker.Submit(new LIWThreadWorkerTask{ LIW_TT_ImguiDrawBeg, nullptr });
			s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

			s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_EDTR_UIDraw, param });
		}

		/// <summary>
		/// Draw editor UI
		/// </summary>
		/// <param name=""> LIWFrameData handle(frame) </param>
		static void LIW_FT_EDTR_UIDraw(LIW_FIBER_RUNNER_PARAM) {
			LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
			LIWCore::s_ins.m_mainThreadWorker.Submit(new LIWThreadWorkerTask{ TT_TestUIDraw, nullptr });
			LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

			s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_EDTR_UIDrawEnd, param });
		}
		
		/// <summary>
		/// End drawing editor UI
		/// </summary>
		/// <param name=""> LIWFrameData handle(frame) </param>
		static void LIW_FT_EDTR_UIDrawEnd(LIW_FIBER_RUNNER_PARAM) {
			s_ins.m_fiberThreadPool.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
			s_ins.m_mainThreadWorker.Submit(new LIWThreadWorkerTask{ LIW_TT_ImguiDrawEndAndPresent, nullptr });
			s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, thisFiber);

			s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_FrameEnd, param });
		}

		/// <summary>
		/// End a frame
		/// </summary>
		/// <param name=""> LIWFrameData handle(frame)(delete) </param>
		static void LIW_FT_FrameEnd(LIW_FIBER_RUNNER_PARAM) {

			LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, thisFiber);

			const liw_hdl_type handleFrameData = (liw_hdl_type)param;
			LIWFrameData* ptrFrameData = (LIWFrameData*)liw_maddr_frame(handleFrameData);

			LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, 1);
			LIWCore::s_ins.m_mainThreadWorker.Submit(new LIWThreadWorkerTask{ LIW_TT_SwapBuffer, nullptr });
			LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, thisFiber);

			
			liw_delete_frame<LIWFrameData>(handleFrameData);

			// Note: do all memory operation before memory management frame udpate. 
			//
			// Update memory management
			//
			int threadCount = LIW::LIWThreads::s_ins.GetThreadCount();

			LIWCore::s_ins.m_fiberThreadPool.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, threadCount);
			for (size_t i = 0; i < threadCount; i++) {
				s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_FrameMemoryThdUpdate, (void*)i });
			}
			LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, thisFiber);

			printf("FrameEnd %llu\n", s_ins.m_frameCount);

			//LIWCore::s_ins.NotifyShutdown();
			s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_FrameBeg, 0 });
		}

		/// <summary>
		/// Run per frame memory management
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_FT_FrameMemoryThdUpdate(LIW_FIBER_RUNNER_PARAM) {
			int idxThread = (int)((size_t)param);
			liw_mupdate_def_thd(idxThread);
			liw_mupdate_static_thd(idxThread);
			liw_mupdate_frame_thd(idxThread);
			liw_mupdate_dframe_thd(idxThread);
			LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, 1);
		}

		/// <summary>
		/// Notify to stop everything
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_FT_NotifyStop(LIW_FIBER_RUNNER_PARAM) {
			s_ins.NotifyShutdown();
		}

		/// <summary>
		/// Update window
		/// </summary>
		/// <param name=""> LIWFrameData handle(frame) </param>
		static void LIW_TT_WindowUpdate(LIW_THREADWORKER_RUNNER_PARAM) {
			auto ptrFrameData = LIWPointer<LIWFrameData, LIWMem_Frame>((liw_hdl_type)param);
			auto ptrWindow = s_ins.m_window;
			if (!ptrWindow->UpdateWindow()) {
				s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIW_FT_NotifyStop, 0 });
				ptrFrameData->m_singnalEnd = true;
			}
			LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, 1);
		}

		/// <summary>
		/// Poll OS event
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_TT_EventPoll(LIW_THREADWORKER_RUNNER_PARAM) {
			glfwPollEvents();
			LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEBEG, 1);
		}

		/// <summary>
		/// Begin imgui drawing
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_TT_ImguiDrawBeg(LIW_THREADWORKER_RUNNER_PARAM) {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
		}
		/// <summary>
		/// End imgui drawing and present
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_TT_ImguiDrawEndAndPresent(LIW_THREADWORKER_RUNNER_PARAM) {
			// Prerender ImGUI
			ImGui::Render();
			// Render UI
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_EDTR_UIDRAW, 1);
		}

		/// <summary>
		/// Swap buffer
		/// </summary>
		/// <param name=""> None </param>
		static void LIW_TT_SwapBuffer(LIW_THREADWORKER_RUNNER_PARAM) {
			auto ptrWindow = s_ins.m_window;
			ptrWindow->SwapBuffer();

			LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, 1);
		}

	public:
		LIW::App::Environment m_environment;
		LIWPointer<LIW::App::Window, LIWMem_Static> m_window{ liw_c_nullhdl };
		TestGame m_game;
		fiber_thdpool_type m_fiberThreadPool;
		main_thdwkr_type m_mainThreadWorker;

	private:
		std::atomic<bool> m_isRunning;
		std::chrono::system_clock::time_point m_timeStart;
		std::chrono::system_clock::time_point m_timeFrame;
		size_t m_frameCount;
	public:
		static LIWCore s_ins;
	};
}

