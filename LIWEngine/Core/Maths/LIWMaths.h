#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace LIW {
	namespace Maths {
		//
		// Sign
		//
		inline int Sign(float x) { if (x > 0)return 1; else if (x == 0) return 0; else return -1; }
		inline int SignNoZero(float x) { if (x >= 0)return 1; else return -1; }

		//
		// Triangle
		//
		void ScreenBoxOfTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, glm::vec2& topLeft, glm::vec2& bottomRight);

		int ScreenAreaOfTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
		float FloatAreaOfTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		float CrossAreaOfTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);
	}
}