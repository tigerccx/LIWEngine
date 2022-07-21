/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#include "Matrix2.h"
#include "Maths.h"

using namespace NCL;
using namespace NCL::Maths;

Matrix2::Matrix2(void)	{
	array[0] = 1.0f;
	array[1] = 0.0f;
	array[2] = 0.0f;
	array[3] = 1.0f;
}

Matrix2::Matrix2(const float elements[4]) {
	array[0] = elements[0];
	array[1] = elements[1];
	array[2] = elements[2];
	array[3] = elements[3];
}

Matrix2::Matrix2(const Vector2& a, const Vector2& b)
{
	array[0] = a.x;
	array[1] = b.x;
	array[2] = a.y;
	array[3] = b.y;
}

Matrix2::~Matrix2(void)	{
}

void Matrix2::ToZero() {
	array[0] = 0.0f;
	array[1] = 0.0f;
	array[2] = 0.0f;
	array[3] = 0.0f;
}

void Matrix2::Invert()
{
	float determinant = (array[0] * array[3]) - (array[1] * array[2]);
	float invDet = 1.0f / determinant; //Turn our divides into multiplies!

	array[0] = array[3] * invDet;
	array[1] = -array[2] * invDet;
	array[2] = -array[1] * invDet;
	array[3] = array[0] * invDet;
}

Matrix2 Matrix2::Inverse() const
{
	Matrix2 newMatrix = *this;
	newMatrix.Invert();
	return newMatrix;
}

Matrix2 Matrix2::Rotation(float degrees)	{
	Matrix2 mat;

	float radians = LIW::Maths::DegreesToRadians(degrees);
	float s = sin(radians);
	float c = cos(radians);

	mat.array[0] = c;
	mat.array[1] = s;
	mat.array[2] = -s;
	mat.array[3] = c;

	return mat;
}