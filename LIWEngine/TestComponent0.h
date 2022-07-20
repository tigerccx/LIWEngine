#pragma once

#include "Maths/Vector2.h"
#include "Maths/Vector3.h"
#include "Maths/Vector4.h"

#include "Framework/LIWComponent.h"

using namespace LIW;

class TestComponent0 : public LIWComponent {
public:
	float m_float0;
	float m_float1 = 1.0f;
	int m_int0 = 0;
	Vector2 m_vec2_0;
	Vector3 m_vec3_0;
	Vector4 m_vec4_0;
	std::string m_str0 = "What is going on!";
};

