#include "LIWMaths.h"

namespace LIW {
	namespace Maths {

		const float PI = glm::pi<float>();

		void ScreenBoxOfTri(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, glm::vec2& topLeft, glm::vec2& bottomRight)
		{
			topLeft.x = glm::min(v0.x, glm::min(v1.x, v2.x));
			topLeft.y = glm::min(v0.y, glm::min(v1.y, v2.y));

			bottomRight.x = glm::max(v0.x, glm::max(v1.x, v2.x));
			bottomRight.y = glm::max(v0.y, glm::max(v1.y, v2.y));
		}
		int ScreenAreaOfTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
		{
			int area = (int)(((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
			return (area >> 1);
		}
		float FloatAreaOfTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
		{
			float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
			return (area * 0.5f);
		}
		float CrossAreaOfTri(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
		{
			glm::vec3 area = glm::cross(a - b, a - c);
			return area.length() *0.5f;
		}
		glm::vec3 QuatToEulerXZY(glm::quat quat)
		{
			glm::vec3 euler;

			const float x = quat.x;
			const float y = quat.y;
			const float z = quat.z;
			const float w = quat.w;

			const float t = x * y + z * w;

			if (t > 0.4999) {
				euler.z = Maths::PI / 2.0f;
				euler.y = 2.0f * std::atan2(x, w);
				euler.x = 0.0f;

				return euler;
			}

			if (t < -0.4999) {
				euler.z = -Maths::PI / 2.0f;
				euler.y = -2.0f * std::atan2(x, w);
				euler.x = 0.0f;
				return euler;
			}

			const float sqx = x * x;
			const float sqy = y * y;
			const float sqz = z * z;

			euler.z = asin(2 * t);
			euler.y = atan2(2 * y * w - 2 * x * z, 1.0f - 2 * sqy - 2 * sqz);
			euler.x = atan2(2 * x * w - 2 * y * z, 1.0f - 2 * sqx - 2.0f * sqz);

			return euler;
		}
		glm::quat EulerToQuatXZY(glm::vec3 eulars) 
		{
			const float pitch = eulars.x;
			const float yaw = eulars.y;
			const float roll = eulars.z;
			
			const float cos1 = (float)cos(yaw * 0.5f);
			const float cos2 = (float)cos(roll * 0.5f);
			const float cos3 = (float)cos(pitch * 0.5f);

			const float sin1 = (float)sin(yaw * 0.5f);
			const float sin2 = (float)sin(roll * 0.5f);
			const float sin3 = (float)sin(pitch * 0.5f);

			glm::quat q;

			q.x = (sin1 * sin2 * cos3) + (cos1 * cos2 * sin3);
			q.y = (sin1 * cos2 * cos3) + (cos1 * sin2 * sin3);
			q.z = (cos1 * sin2 * cos3) - (sin1 * cos2 * sin3);
			q.w = (cos1 * cos2 * cos3) - (sin1 * sin2 * sin3);

			return q;
		}
	}
}