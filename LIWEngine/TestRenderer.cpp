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
	Vector4 clear_color = Vector4(0.45f, 0.55f, 0.60f, 1.00f);
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

void FT_TestRenderUpdate(LIW_FIBER_RUNNER_PARAM)
{
	using namespace LIW;

	TestRenderData* ptrData = (TestRenderData*)liw_maddr_frame((liw_hdl_type)param);
	LIWFrameData* ptrFrameData = (LIWFrameData*)liw_maddr_frame(ptrData->m_hdlFrameData);
	ptrData->m_renderer->Update(ptrFrameData->m_timeDelta);

	LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ FT_TestRenderRender , param });
}

void FT_TestRenderRender(LIW_FIBER_RUNNER_PARAM)
{
	using namespace LIW;

	TestRenderData* ptrData = (TestRenderData*)liw_maddr_frame((liw_hdl_type)param);
	LIWFrameData* ptrFrameData = (LIWFrameData*)liw_maddr_frame(ptrData->m_hdlFrameData);
	ptrData->m_renderer->RenderScene();

	liw_delete_frame<TestRenderData>((liw_hdl_type)param);

	LIWCore::s_ins.m_fiberThreadPool.Submit(new LIWFiberTask{ LIWCore::LIW_FT_FrameEnd , param });
}