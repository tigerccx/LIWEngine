#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"

namespace LIW {
	enum LIWCameraType {
		LIWCamera_Orthographic,
		LIWCamera_Perspective,
		LIWCamera_Max
	};

	struct LIWCameraParam_Orthographic {
		float m_left = 0;
		float m_right = 0;
		float m_top = 0;
		float m_bottom = 0;
	};

	struct LIWCameraParam_Perspective {
		float fovY = 0;
		float aspectRatio = 0; // W/H
	};

	class LIWComponent_Camera :public LIWComponent {
	public:
		union CameraParameter {
			LIWCameraParam_Perspective m_perspective;
			LIWCameraParam_Orthographic m_orthographic;
		} m_param = { 0,0 };
		LIWCameraType m_cameraType{ LIWCamera_Perspective };
		float m_near = 0.1f;
		float m_far = 1000.0f;
	public:
		void SetPerspective(LIWCameraParam_Perspective param) {
			m_cameraType = LIWCamera_Perspective;
			m_param.m_perspective = param;
		}
		void SetOrthographic(LIWCameraParam_Orthographic param) {
			m_cameraType = LIWCamera_Orthographic;
			m_param.m_orthographic = param;
		}
	};
}