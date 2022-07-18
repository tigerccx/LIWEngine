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

#include "LIWConstants.h"
#include "Time/LIWTime.h"
#include "Threading/LIWThread.h"
#include "Memory/LIWMemory.h"
#include "Threading/LIWFiberExecutor.h"
#include "Threading/LIWMainThreadExecutor.h"
#include "LIWFrame.h"





namespace LIW {
	class LIWCore {
	public:
		typedef std::mutex mtx_type;
		typedef std::unique_lock<std::mutex> uniqlk_type;
		typedef std::condition_variable condvar_type;

	public:
		int Boot() {
			// Time start from this point
			LIWTime::m_timeStart = LIWTime::m_timeFrame = std::chrono::system_clock::now();
			
			// Decide how many threads to use
			int countWorkerThreads = std::thread::hardware_concurrency();
			if (countWorkerThreads == 0)
				countWorkerThreads = 16;
			
			countWorkerThreads = 10;

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
			LIWMainThreadExecutor::m_executor.Init();
			if (!LIWMainThreadExecutor::m_executor.IsInit()) {
				throw "Main ThreadWorker init failed. ";
			}
			idxThread++;

			//
			// Init fiber thread pool
			//
			LIWFiberExecutor::m_executor.Init(countWorkerThreads, idxThread);
			if (!LIWFiberExecutor::m_executor.IsInit()) {
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
			// Kick off the first fiber task
			auto ptrFT_FrameBeg = liw_new_def<LIW_FT_FrameBeg>();
			LIWFiberExecutor::m_executor.Submit(ptrFT_FrameBeg);

			return 0;
		}

		void MainThreadRun() {
			LIWMainThreadExecutor::m_executor.Run();
		}

		void NotifyShutdown() {
			LIWMainThreadExecutor::m_executor.WaitAndStop();
		}

		void Shutdown() {

			//m_game.CleanUp();

			glfwTerminate();


			//
			// Shutdown fiber therad pool
			//
			LIWFiberExecutor::m_executor.WaitAndStop();
			
			//
			// Shutdown main thread worker
			//
			LIWMainThreadExecutor::m_executor.WaitTillEmpty();

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
		LIW::App::Environment m_environment;
		LIWPointer<LIW::App::Window, LIWMem_Static> m_window{ liw_c_nullhdl };
		TestGame m_game;

	private:
		std::atomic<bool> m_isRunning;
	public:
		static LIWCore s_ins;
	};

	/// <summary>
	/// Notify to stop everything
	/// </summary>
	class LIW_FT_NotifyStop final :
		public LIWFiberTask
	{
	public:
		void Execute(LIWFiberWorkerPointer thisFiber) override;
	};
}

