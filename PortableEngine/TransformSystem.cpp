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

void TransformSystem::CalculatePosition(Transform& transform)
{
	transform.position = glm::vec3(transform.worldMatrix[3].x, transform.worldMatrix[3].y, transform.worldMatrix[3].z);
}

void TransformSystem::CalculateEulerAngles(Transform& transform)
{
	glm::extractEulerAngleXYZ(transform.worldMatrix, transform.rotation.x, transform.rotation.y, transform.rotation.z);
}

void TransformSystem::CalculateScale(Transform& transform)
{
	transform.scale.x = glm::sqrt(glm::pow(transform.worldMatrix[0].x, 2) + glm::pow(transform.worldMatrix[1].x, 2) + glm::pow(transform.worldMatrix[2].x, 2));
	transform.scale.y = glm::sqrt(glm::pow(transform.worldMatrix[0].y, 2) + glm::pow(transform.worldMatrix[1].y, 2) + glm::pow(transform.worldMatrix[2].y, 2));
	transform.scale.z = glm::sqrt(glm::pow(transform.worldMatrix[0].z, 2) + glm::pow(transform.worldMatrix[1].z, 2) + glm::pow(transform.worldMatrix[2].z, 2));
}

void TransformSystem::CalculateWorldMatrix(Transform* transform)
{
	glm::mat4 translationMat = glm::translate(glm::mat4(1.0), transform->position);
	glm::mat4 rotationMat = glm::yawPitchRoll(transform->rotation.y, transform->rotation.x, transform->rotation.z);
	glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), transform->scale);
	transform->worldMatrix = translationMat * scaleMat * rotationMat;
}
