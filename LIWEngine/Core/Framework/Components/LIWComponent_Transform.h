#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"


namespace LIW {
	class LIWComponent_Transform : public LIWComponent {
	public:
		glm::vec3 m_location;
		glm::quat m_rotation;
		glm::vec3 m_scale;
	public:
		static inline glm::mat4 GetMatrix(const LIWComponent_Transform& transform) {
			const glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), transform.m_location);
			const glm::mat4 rotation = glm::toMat4(transform.m_rotation);
			const glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), transform.m_scale);
			return translate * rotation * scale;
		}
		static inline glm::vec3 GetForward(const LIWComponent_Transform& transform) {
			return (glm::mat4(transform.m_rotation) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
		}
		static inline glm::vec3 GetRight(const LIWComponent_Transform& transform) {
			return (glm::mat4(transform.m_rotation) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		}
		static inline glm::vec3 GetUp(const LIWComponent_Transform& transform) {
			return (glm::mat4(transform.m_rotation) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
		}
		static LIWComponent_Transform& Translate(LIWComponent_Transform& transform, const glm::vec3& offset) {
			transform.m_location += offset;
			return transform;
		}

		static LIWComponent_Transform& Rotate(LIWComponent_Transform& transform, const glm::quat& rotation) {
			transform.m_rotation = rotation * transform.m_rotation;
			return transform;
		}

		static LIWComponent_Transform& Scale(LIWComponent_Transform& transform, const glm::vec3& scale) {
			transform.m_scale *= scale;
			return transform;
		}
	};
}