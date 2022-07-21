#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix2.h"
#include "Matrix3.h"
#include "Matrix4.h"

namespace LIW {
	namespace Maths {
		typedef NCL::Maths::Vector2 LIWVector2;
		typedef NCL::Maths::Vector3 LIWVector3;
		typedef NCL::Maths::Vector4 LIWVector4;
		
		typedef NCL::Maths::Quaternion LIWQuaternion;
		
		typedef NCL::Maths::Matrix2 LIWMatrix2;
		typedef NCL::Maths::Matrix3 LIWMatrix3;
		typedef NCL::Maths::Matrix4 LIWMatrix4;
	}
}