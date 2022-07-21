/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Maths.h"

namespace LIW {
	namespace Maths {
		void ScreenBoxOfTri(const LIWVector3& v0, const LIWVector3& v1, const LIWVector3& v2, LIWVector2& topLeft, LIWVector2& bottomRight) {
			topLeft.x = std::min(v0.x, std::min(v1.x, v2.x));
			topLeft.y = std::min(v0.y, std::min(v1.y, v2.y));

			bottomRight.x = std::max(v0.x, std::max(v1.x, v2.x));
			bottomRight.y = std::max(v0.y, std::max(v1.y, v2.y));
		}

		int ScreenAreaOfTri(const LIWVector3 &a, const LIWVector3 &b, const LIWVector3 & c) {
			int area =(int) (((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y)));
			return (area >> 1);
		}

		float FloatAreaOfTri(const LIWVector3 &a, const LIWVector3 &b, const LIWVector3 & c) {
			float area = ((a.x * b.y) + (b.x * c.y) + (c.x * a.y)) -
				((b.x * a.y) + (c.x * b.y) + (a.x * c.y));
			return (area * 0.5f);
		}

		float CrossAreaOfTri(const LIWVector3 &a, const LIWVector3 &b, const LIWVector3 & c) {
			LIWVector3 area = LIWVector3::Cross(a - b, a - c);
			return area.Length() * 0.5f;
		}

		bool ApproxEqual(const LIWVector3& a, const LIWVector3& b, float eps)
		{
			return (std::abs(a.x - b.x) < eps && std::abs(a.y - b.y) < eps && std::abs(a.z - b.z) < eps);
		}
	

		LIWVector3 Clamp(const LIWVector3& a, const LIWVector3&mins, const LIWVector3& maxs) {
			return LIWVector3(
				Clamp(a.x, mins.x, maxs.x),
				Clamp(a.y, mins.y, maxs.y),
				Clamp(a.z, mins.z, maxs.z)
			);
		}
	}
}