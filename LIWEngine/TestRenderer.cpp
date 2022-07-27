#include "TestRenderer.h"

using namespace LIW;

void TestRenderer::Update(float msec)
{
	if (currentWindow->GetKeyboard()->KeyTriggered(KEYBOARD_E)) {
		std::cout << "E Triggered" << std::endl;
	}
	if (currentWindow->GetKeyboard()->KeyDown(KEYBOARD_E)) {
		std::cout << "E Down" << std::endl;
	}
	if (currentWindow->GetKeyboard()->KeyHeld(KEYBOARD_E)) {
		std::cout << "E Held" << std::endl;
	}
	if (currentWindow->GetKeyboard()->KeyReleased(KEYBOARD_E)) {
		std::cout << "E Released" << std::endl;
	}

	if (currentWindow->GetMouse()->ButtonClicked(MOUSE_BUTTON_LEFT)) {
		std::cout << "ML Clicked" << std::endl;
	}
	if (currentWindow->GetMouse()->DoubleClicked(MOUSE_BUTTON_LEFT)) {
		std::cout << "ML DoubleClicked" << std::endl;
	}
	if (currentWindow->GetMouse()->ButtonDown(MOUSE_BUTTON_LEFT)) {
		std::cout << "ML Down" << std::endl;
	}
	if (currentWindow->GetMouse()->ButtonHeld(MOUSE_BUTTON_LEFT)) {
		std::cout << "ML Held" << std::endl;
	}
	if (currentWindow->GetMouse()->ButtonReleased(MOUSE_BUTTON_LEFT)) {
		std::cout << "ML Released" << std::endl;
	}
}

void TestRenderer::RenderScene()
{
	glm::vec4 clear_color = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);
	int display_w, display_h;
	glfwGetFramebufferSize(currentWindow->GetHandle(), &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);

	// Render a triangle
	glBegin(GL_TRIANGLES);
	glVertex2d(0.5f, 0.5f);
	glVertex2d(-0.5f, -0.5f);
	glVertex2d(0.5f, -0.5f);

	glEnd();
}

#include "LIWCore.h"
#include "TestGlobal.h"

void FT_TestRenderUpdate::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	TestGlobal::s_renderer->Update(m_ptrFrameData->m_timeDelta);

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, 1);
}

void FT_TestRenderRender::Execute(LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	LIWFiberExecutor::m_executor.IncreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
	auto ptrTT_TestRenderRender = new TT_TestRenderRender();
	LIWMainThreadExecutor::m_executor.Submit(ptrTT_TestRenderRender);
	LIWFiberExecutor::m_executor.WaitOnSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, thisFiber);

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(TEST_SYNC_COUNTER_SYSTEM, 1);
}

void TT_TestRenderRender::Execute()
{
	TestGlobal::s_renderer->RenderScene();

	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_RENDER, 1);
}
