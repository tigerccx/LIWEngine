#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"


namespace LIW {
	class LIWComponent_Transform final: public LIWComponent {
	public:
		glm::vec3 m_location{ 0.0f,0.0f,0.0f };
		glm::quat m_rotation{ 1.0f,0.0f,0.0f,0.0f };
		glm::vec3 m_scale{ 1.0f,1.0f,1.0f };
	public:
		inline glm::mat4 GetMatrix() {
			const glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), m_location);
			const glm::mat4 rotation = glm::toMat4(m_rotation);
			const glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), m_scale);
			return translate * rotation * scale;
		}
		inline glm::vec3 GetForward() {
			return (glm::mat4(m_rotation) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
		}
		inline glm::vec3 GetRight() {
			return (glm::mat4(m_rotation) * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		}
		inline glm::vec3 GetUp() {
			return (glm::mat4(m_rotation) * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
		}
		inline LIWComponent_Transform& Translate(const glm::vec3& offset) {
			m_location += offset;
			return *this;
		}
		inline LIWComponent_Transform& Rotate(const glm::quat& rotation) {
			m_rotation = rotation * m_rotation;
			return *this;
		}
		inline LIWComponent_Transform& Scale(const glm::vec3& scale) {
			m_scale *= scale;
			return *this;
		}
	};
}