#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"


namespace LIW {
	class LIWComponent_CameraController final: public LIWComponent {
	public:
		float m_speed = 0.08f;
		float m_boostScale = 5.0f;
		//float m_rotateScale = 0.6f;
		float m_rotateScale = 0.008f;
		bool m_isSelected = false;
	};
}