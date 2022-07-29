#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"

namespace LIW {
	enum LIWLightType {
		LIWLightType_Directional,
		LIWLightType_Point,
		LIWLightType_Spot,
		LIWLightType_Max
	};

	struct LIWPointLightParameters {
		float m_radius; //radius of light
	};

	struct LIWSpotLightParameters {
		float m_radius; //radius of light
		float m_angle; //full angle of light
	};

	class LIWComponent_Light final : public LIWComponent {
	public:
		LIWLightType m_lightType{ LIWLightType_Directional };
		union LightParameter {
			LIWPointLightParameters m_pointLight;
			LIWSpotLightParameters m_spotLight;
		} m_param = { 0.0f };
		glm::vec4 m_colourAndIntensity;

	public:
		void SetDirectional() {
			m_lightType = LIWLightType_Directional;
		}
		void SetPoint(LIWPointLightParameters param) {
			m_lightType = LIWLightType_Point;
			m_param.m_pointLight = param;
		}
		void SetSpot(LIWSpotLightParameters param) {
			m_lightType = LIWLightType_Spot;
			m_param.m_spotLight = param;
		}
	};
}