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
}