#pragma once

#include "utils/glm_utils.h"
#include "utils/math_utils.h"

class Camera
{
public:
	Camera();
	Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);
	// Update camera
	void Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up);
	void SetPosition(glm::vec3 position);
	void SetTargetDistance(float distance);
	float GetTargetDistance() const;
	float GetXOZAngle() const;
	void MoveForward(float distance);
	void MoveRight(float distance);
	void MoveUpward(float distance);

	void TranslateForward(float distance);
	void TranslateUpward(float distance);
	void TranslateRight(float distance);

	void RotateFirstPerson_OX(float angle);
	void RotateFirstPerson_OY(float angle);
	void RotateFirstPerson_OZ(float angle);
	void RotateThirdPerson_OX(float angle);
	void RotateThirdPerson_OY(float angle);
	void RotateThirdPerson_OZ(float angle);
	glm::mat4 GetViewMatrix() const;
	glm::vec3 GetTargetPosition() const;
	glm::vec3 GetPosition() const;
	glm::vec3 GetRightVector() const;
	glm::vec3 GetUpVector() const;
	glm::vec3 GetForwardXOZ() const;
	glm::vec3 GetForwardVector() const;
private:
	glm::vec3 position;
	glm::vec3 right;
	glm::vec3 forward;
	glm::vec3 up;
	float distanceToTarget;
};
