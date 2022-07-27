#pragma once
#include "Maths/LIWMaths.h"

void test() {
	glm::vec3 pos = glm::vec3(0, 0, 5);
	glm::quat rot = glm::quat(1, 0, 0, 0);
	glm::vec3 scal = glm::vec3(1, 1, 1);
	const glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), pos);
	const glm::mat4 rotation = glm::toMat4(rot);
	const glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), scal);
	const glm::mat4 mat = translate * rotation * scale;

	constexpr float eularX = glm::radians(45.0f);
	//const float eularX1 = eularX + glm::pi<float>();
	constexpr float eularY = glm::radians(10.0f);
	constexpr float eularZ = glm::radians(20.0f);
	glm::quat quat = glm::quat(glm::vec3(eularX, eularY, eularZ));
	glm::vec3 eular = glm::eulerAngles(quat);


	glm::quat q1 = LIW::Maths::EulerToQuatXZY(glm::radians(glm::vec3(10.0f, 20.0f, 30.0f)));
	glm::vec3 v0(0, 0, -1);
	glm::vec4 v0_(0, 0, -1, 0);
	glm::vec3 v1 = glm::rotate(q1, v0_);
	
	auto matRotX = glm::rotate(glm::identity<glm::mat4>(), glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	auto matRotY = glm::rotate(glm::identity<glm::mat4>(), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto matRotZ = glm::rotate(glm::identity<glm::mat4>(), glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 vXZY = (matRotY * matRotZ * matRotX) * v0_;
	//glm::vec3 vXYZ = matRotX * matRotZ * matRotX * v0_;

	if ((matRotX * matRotZ * matRotY) * v0_ != (matRotX * (matRotZ * (matRotY * v0_))))
		throw "WAS";

	glm::vec3 eular3 = LIW::Maths::QuatToEulerXZY(q1);
	glm::vec3 rads = glm::radians(glm::vec3(10.0f, 20.0f, 30.0f));
}