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
#include "LIWFrame.h"

#include "TestRenderer.h"

namespace LIW {
	const size_t LIW_FIBER_COUNT = 1 << 8;
	const size_t LIW_FIBER_AWAKE_COUNT = 1 << 10;
	const size_t LIW_FIBER_TASK_COUNT = 1 << 16;
	const size_t LIW_FIBER_SYNC_COUNTER_COUNT = 1 << 10;


	class LIWCore {
	public:
		typedef std::mutex mtx_type;
		typedef std::unique_lock<std::mutex> uniqlk_type;
		typedef std::condition_variable condvar_type;
		typedef LIW::LIWFiberThreadPoolSized<LIW_FIBER_COUNT, LIW_FIBER_AWAKE_COUNT, LIW_FIBER_TASK_COUNT, LIW_FIBER_SYNC_COUNTER_COUNT> fiber_thd_pool_type;

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

			int idxThread = LIW_THREAD_IDX_MAIN + 1;

			//
			// Init memory management
			// 

			// Init mem
			liw_minit_def(countThreads);
			liw_minit_static(countThreads);
			liw_minit_frame(countThreads);
			liw_minit_dframe(countThreads);

			// Init mem for main thread
			LIWThreadInit();


			//
			// Init fiber thread pool
			//
			m_fiberThreadPool.Init(countWorkerThreads, idxThread);
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
			auto ptrWindow = (LIW::App::Window*)liw_maddr_static(m_window);
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

		void MainThreadLoop() {
			using namespace std::chrono;
			while (m_isRunning.load()) {
				//TODO: DO things main thread should do
				std::this_thread::sleep_for(5ms);
			}
		}

		void NotifyShutdown() {
			m_isRunning.store(false);
		}

		void Shutdown() {

			m_game.CleanUp();

			glfwTerminate();


			//
			// Shutdown fiber therad pool
			//
			m_fiberThreadPool.WaitAndStop();


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
		static void LIW_FT_FrameBeg(LIW_FIBER_RUNNER_PARAM){
			std::chrono::system_clock::time_point timeNow = std::chrono::system_clock::now();
			const double timeFromStart = double((std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - s_ins.m_timeStart)).count()) * 1e-9;
			const double timeFrame = double((std::chrono::duration_cast<std::chrono::nanoseconds>(timeNow - s_ins.m_timeFrame)).count()) * 1e-9;
			s_ins.m_timeFrame = timeNow;
			s_ins.m_frameCount++;

			printf("FrameBeg %llu\n", s_ins.m_frameCount);

			liw_hdl_type hdlFrameData = liw_new_frame<LIWFrameData>(LIWFrameData{ timeNow, timeFrame, timeFromStart });
			liw_hdl_type hdlTestRenderData = liw_new_frame<TestRenderData>(TestRenderData{ hdlFrameData, s_ins.m_game.m_renderer });
			
			s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_TestRenderRender, (void*)hdlTestRenderData });
			
			//liw_hdl_type hdlGameData = liw_new_frame<GameData>(GameData{ hdlFrameData, &s_ins.m_game });
			//
			//s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_TestRenderRender, (void*)hdlGameData });
		}

		static void LIW_FT_FrameEnd(LIW_FIBER_RUNNER_PARAM) {

			LIWCore::s_ins.m_fiberThreadPool.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_FRAMEEND, thisFiber);

			const liw_hdl_type handleFrameData = (liw_hdl_type)param;
			LIWFrameData* ptrFrameData = (LIWFrameData*)liw_maddr_frame(handleFrameData);

			
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

			LIWCore::s_ins.NotifyShutdown();
		}

		static void LIW_FT_FrameMemoryThdUpdate(LIW_FIBER_RUNNER_PARAM) {
			int idxThread = (int)((size_t)param);
			liw_mupdate_def_thd(idxThread);
			liw_mupdate_static_thd(idxThread);
			liw_mupdate_frame_thd(idxThread);
			liw_mupdate_dframe_thd(idxThread);
			LIWCore::s_ins.m_fiberThreadPool.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_MEMORY_UPDATE, 1);
		}

	public:
		LIW::App::Environment m_environment;
		liw_hdl_type m_window{ liw_c_nullhdl };
		TestGame m_game;
		fiber_thd_pool_type m_fiberThreadPool;

	private:
		std::atomic<bool> m_isRunning;
		std::chrono::system_clock::time_point m_timeStart;
		std::chrono::system_clock::time_point m_timeFrame;
		size_t m_frameCount;
	public:
		static LIWCore s_ins;
	};
}

