#pragma once
#include "Maths/LIWMaths.h"
#include "Framework/LIWComponent.h"


namespace LIW {
	class LIWComponent_Transform final: public LIWComponent {
	public:
		glm::vec3 m_position{ 0.0f,0.0f,0.0f };
		glm::quat m_rotation{ 1.0f,0.0f,0.0f,0.0f };
		glm::vec3 m_scale{ 1.0f,1.0f,1.0f };
	public:
		inline glm::mat4 GetMatrix() {
			const glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), m_position);
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
			m_position += offset;
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

	// Editor
	public:
		inline void EditorDrawUI() override {
			ImGui::DragFloat3("Position", glm::value_ptr(m_position), 0.1f);
			glm::vec3 rots = Maths::QuatToEulerXZY(m_rotation);
			rots = glm::vec3(Maths::RadToDeg(rots.x), Maths::RadToDeg(rots.y), Maths::RadToDeg(rots.z));
			ImGui::DragFloat3("Rotations", glm::value_ptr(rots), 0.1f);
			m_rotation = Maths::EulerToQuatXZY(glm::radians(rots));
			ImGui::DragFloat3("Scales", glm::value_ptr(m_scale), 0.1f);
		}
	};
}