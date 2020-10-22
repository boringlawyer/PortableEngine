#include "TransformSystem.h"
#include <glm/gtx/euler_angles.hpp>
void TransformSystem::MoveRelative(glm::vec3 unrotated, Transform* transform)
{
	glm::vec4 unrotatedVec(unrotated.x, unrotated.y, unrotated.z, 0);
	glm::mat4 rotationMat = glm::yawPitchRoll(transform->rotation.y, transform->rotation.x, transform->rotation.z);
	glm::vec3 rotatedVec = rotationMat * unrotatedVec;
	transform->position = transform->position + rotatedVec;
}

void TransformSystem::Rotate(glm::vec3 rotation, Transform* transform)
{
	transform->rotation += rotation;
	CalculateWorldMatrix(transform);
}

glm::vec3 TransformSystem::CalculateForward(Transform* transform)
{
	return glm::vec3(transform->worldMatrix[2].x, transform->worldMatrix[2].y, transform->worldMatrix[2].z);
}

void TransformSystem::CalculateWorldMatrix(Transform* transform)
{
	glm::mat4 translationMat = glm::translate(glm::mat4(1.0), transform->position);
	glm::mat4 rotationMat = glm::yawPitchRoll(transform->rotation.y, transform->rotation.x, transform->rotation.z);
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), transform->scale);
	transform->worldMatrix = scaleMat * rotationMat * translationMat;
}