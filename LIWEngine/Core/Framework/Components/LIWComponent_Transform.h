#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"


namespace LIW {
	class LIWComponent_Transform : public LIWComponent {
	public:
		Maths::LIWVector3 m_location;
		Maths::LIWQuaternion m_rotation;
		Maths::LIWVector3 m_scale;
	public:
		static inline Maths::LIWMatrix4 GetMatrix(const LIWComponent_Transform& transform) {
			return	Maths::LIWMatrix4::Translation(transform.m_location) *
					Maths::LIWMatrix4(transform.m_rotation) *
					Maths::LIWMatrix4::Scale(transform.m_scale);
		}
		static inline Maths::LIWVector3 GetForward(const LIWComponent_Transform& transform) {
			return (Maths::LIWMatrix4(transform.m_rotation) * Maths::LIWVector4(0.0f, 0.0f, -1.0f, 0.0f));
		}
		static inline Maths::LIWVector3 GetRight(const LIWComponent_Transform& transform) {
			return (Maths::LIWMatrix4(transform.m_rotation) * Maths::LIWVector4(1.0f, 0.0f, 0.0f, 0.0f));
		}
		static inline Maths::LIWVector3 GetUp(const LIWComponent_Transform& transform) {
			return (Maths::LIWMatrix4(transform.m_rotation) * Maths::LIWVector4(0.0f, 1.0f, 0.0f, 0.0f));
		}
		static LIWComponent_Transform& Translate(LIWComponent_Transform& transform, const Maths::LIWVector3& offset) {
			transform.m_location += offset;
			return transform;
		}

		static LIWComponent_Transform& Rotate(LIWComponent_Transform& transform, const Maths::LIWQuaternion& rotation) {
			transform.m_rotation = rotation * transform.m_rotation;
			return transform;
		}

		static LIWComponent_Transform& Scale(LIWComponent_Transform& transform, const Maths::LIWVector3& scale) {
			transform.m_scale *= scale;
			return transform;
		}
	};
}