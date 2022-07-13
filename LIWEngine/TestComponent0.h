#pragma once

#include "Maths/Vector2.h"
#include "Maths/Vector3.h"
#include "Maths/Vector4.h"

#include "Framework/LIWComponent.h"

using namespace LIW;
class TestComponent0 :
	public LIWComponent {
public:
	LIW_SET_COMPONENT_NAME(TestComponent0)

	LIWReflectFloat(m_float0)
	LIWReflectFloatVal(m_float1, 1.0f)
	LIWReflectInt(m_int0)
	LIWReflectAttr(Vector2, m_vec2_0)
	LIWReflectAttr(Vector3, m_vec3_0)
	LIWReflectAttr(Vector4, m_vec4_0)
	LIWReflectAttrVal(std::string, m_str0, "What is going on!")
};

