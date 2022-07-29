#include "LIWSystem_CameraControl.h"
#include "LIWCore.h"
#include "Framework/LIWECSFunctional.h"

void LIW_FT_LIWSystem_CameraControl_Update::Execute(LIW::LIWFiberWorkerPointer thisFiber)
{
	using namespace LIW;

	auto& cameraControllerManager = LIW_ECS_GetComponentManager(LIWComponent_CameraController);
	auto& cameraControllers = cameraControllerManager.m_components;
	for (auto itr = cameraControllers.get_beg(); itr < cameraControllers.get_end(); itr++) {
		auto& cameraController = *itr;
		if (cameraController.m_isSelected) {
			auto entity = cameraController.GetEntity();
			liw_objhdl_type transformHdl = LIW_ECS_GetComponentFromEntity(LIWComponent_Transform, entity);
			if (transformHdl == liw_c_nullobjhdl)
				throw std::runtime_error("Missing transform. ");

			LIWComponent_Transform& transform = LIW_ECS_GetComponent(LIWComponent_Transform, transformHdl);

			//
			// Movement
			//
			auto& keyboard = *LIWGlobal::GetEnvironment()->m_window->GetKeyboard();
			auto& mouse = *LIWGlobal::GetEnvironment()->m_window->GetMouse();

			if (mouse.ButtonHeld(LIW::MOUSE_BUTTON_2)) {
				//const float mouseRelX = mouse.GetRelativePosition().x;
				//const float mouseRelY = mouse.GetRelativePosition().y;
				//glm::vec3 right = transform.GetRight();
				//glm::quat rotPitch = glm::rotate(glm::identity<glm::quat>(), glm::radians(-mouseRelY * cameraController.m_rotateScale), right);
				//glm::quat rotYaw = glm::rotate(glm::identity<glm::quat>(), glm::radians(-mouseRelX * cameraController.m_rotateScale), glm::vec3(0.0, 1.0f, 0.0f));
				//glm::quat rot = glm::normalize(rotYaw * rotPitch);

				//transform.m_rotation = rot * transform.m_rotation;

				//glm::vec3 eulars = glm::eulerAngles(transform.m_rotation);
				glm::vec3 eulars = Maths::QuatToEulerXZY(transform.m_rotation); //Note that this can on
				float pitch = eulars.x;
				float yaw = eulars.y;
				const float mouseRelX = mouse.GetRelativePosition().x;
				const float mouseRelY = mouse.GetRelativePosition().y;
				//printf("MosueX: %f MouseY: %f", mouseRelX, mouseRelY);
				pitch -= mouseRelY * cameraController.m_rotateScale;
				yaw -= mouseRelX * cameraController.m_rotateScale;

				pitch = max(min(pitch, glm::radians(89.9f)), glm::radians(-89.9f));
				while (yaw <= - Maths::PI) {
					yaw += 2.0f * Maths::PI;
				}
				while (yaw > Maths::PI) {
					yaw -= 2.0f * Maths::PI;
				}

				//glm::quat rotPitch = glm::rotate(glm::identity<glm::quat>(), pitch, glm::vec3(1.0f, 0.0f, 0.0f));
				//glm::quat rotYaw = glm::rotate(glm::identity<glm::quat>(), yaw, glm::vec3(0.0, 1.0f, 0.0f));

				//transform.m_rotation = rotYaw * rotPitch;
				//transform.m_rotation = glm::normalize(rotYaw * rotPitch);
				
				eulars.x = pitch;
				eulars.y = yaw;

				transform.m_rotation = Maths::EulerToQuatXZY(eulars);
				glm::vec3 eulars1 = Maths::QuatToEulerXZY(transform.m_rotation);
				glm::bvec3 bools = glm::epsilonEqual(eulars1, eulars, 1e-1f);
				if (!bools.x || !bools.y || !bools.z)
				{
					glm::quat q1 = glm::quat(eulars);
					glm::vec3 eular2 = Maths::QuatToEulerXZY(q1);
				}
			}
			

			const glm::vec3 forward = transform.GetForward();
			const glm::vec3 right = transform.GetRight();
			const glm::vec3 up = transform.GetUp();

			float speed = cameraController.m_speed;

			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_LSHIFT)) {
				speed *= cameraController.m_boostScale;
			}

			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_W)) {
				transform.m_position += forward * speed;
			}
			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_S)) {
				transform.m_position -= forward * speed;
			}
			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_A)) {
				transform.m_position -= right * speed;
			}
			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_D)) {
				transform.m_position += right * speed;
			}
			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_Q)) {
				transform.m_position -= up * speed;
			}
			if (LIWGlobal::GetEnvironment()->m_window->GetKeyboard()->KeyHeld(LIW::KEYBOARD_E)) {
				transform.m_position += up * speed;
			}
		}

	}
	LIWFiberExecutor::m_executor.DecreaseSyncCounter(LIW_SYNC_COUNTER_RESERVE_SYSTEM_UPDATE, 1);
}
