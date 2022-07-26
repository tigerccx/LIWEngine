#include "LIWMaths.h"

namespace LIW {
	namespace Maths {
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
	}
}