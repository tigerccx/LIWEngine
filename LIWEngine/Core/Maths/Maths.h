#pragma once
#include <algorithm>
#include "LIWMaths.h"

namespace LIW {
	namespace Maths {
		
		//
		// Pi
		//
		static const float		PI = 3.14159265358979323846f;
		static const float		PI_OVER_360 = PI / 360.0f;

		//Radians to degrees
		inline float RadiansToDegrees(float rads) { return rads * 180.0f / PI; }

		//Degrees to radians
		inline float DegreesToRadians(float degs) { return degs * PI / 180.0f; }


		//
		// Value
		//
		template<class T>
		inline T Clamp(T value, T min, T max) {
			return std::max(std::min(value, max), min);
			//if (value < min) {
			//	return min;
			//}
			//if (value > max) {
			//	return max;
			//}
			//return value;
		}
		LIWVector3 Clamp(const LIWVector3& a, const LIWVector3& mins, const LIWVector3& maxs);

		template<class T>
		inline T Lerp(const T& a, const T& b, float by) {
			return (a * (1.0f - by) + b * by);
		}

		inline int Sign(float x) { if (x > 0)return 1; else if (x == 0) return 0; else return -1; }
		inline int SignNoZero(float x) { if (x >= 0)return 1; else return -1; }
		bool ApproxEqual(const LIWVector3& a, const LIWVector3& b, float eps = 1e-6);

		//
		// Triangle
		//
		void ScreenBoxOfTri(const LIWVector3& v0, const LIWVector3& v1, const LIWVector3& v2, LIWVector2& topLeft, LIWVector2& bottomRight);

		int ScreenAreaOfTri(const LIWVector3& a, const LIWVector3& b, const LIWVector3& c);
		float FloatAreaOfTri(const LIWVector3& a, const LIWVector3& b, const LIWVector3& c);

		float CrossAreaOfTri(const LIWVector3& a, const LIWVector3& b, const LIWVector3& c);

		//
		// Random
		//
		inline float RandomFloat01() {
			return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		}
		inline float RandomFloatN1P1() {
			return RandomFloat01() * 2.0f - 1.0f;
		}
	}
}