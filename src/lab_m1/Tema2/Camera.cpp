#pragma once

#include "lab_m1/Tema2/Camera.h"

Camera::Camera()
{
	forward = glm::vec3(0, 0, -1);
	position = glm::vec3(0, 0, 0);
	up = glm::vec3(0, 1, 0);
	right = glm::vec3(1, 0, 0);
	distanceToTarget = 0.9f;
}

Camera::Camera(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
{
	Set(position, center, up);
}

// Update camera
void Camera::Set(const glm::vec3& position, const glm::vec3& center, const glm::vec3& up)
{
	this->position = position;
	forward = glm::normalize(center - position);
	right	= glm::cross(forward, up);
	this->up = glm::cross(right, forward);
}

void Camera::SetPosition(glm::vec3 position)
{
	this->position = position;
}

void Camera::SetTargetDistance(float distance)
{
	distanceToTarget = distance;
}

float Camera::GetTargetDistance() const
{
	return distanceToTarget;
}

float Camera::GetXOZAngle() const
{
	glm::vec3 normalizedRight = glm::normalize(right);

	glm::vec3 rightVector = glm::vec3(1, 0, 0);
	float x = glm::dot(normalizedRight, rightVector);
	float crossY = glm::cross(rightVector, normalizedRight).y;

	return atan2(crossY, x);
}

void Camera::MoveUpward(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(0, up.y, 0));
	position += dir * distance;
}

void Camera::MoveForward(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	position += dir * distance;
}

void Camera::MoveRight(float distance)
{
	glm::vec3 dir = glm::normalize(glm::vec3(right.x, 0, right.z));
	position += dir * distance;
}

void Camera::TranslateForward(float distance)
{
	// Translate the camera using the "forward" vector
	position += glm::normalize(forward) * distance;
}

void Camera::TranslateUpward(float distance)
{
	// Translate the camera using the up vector
	position += glm::normalize(up) * distance;
}

void Camera::TranslateRight(float distance)
{
	// Translate the camera using the "right" vector
	// Usually translation using camera "right' is not very useful because if the camera is rotated around the "forward" vector 
	// translation over the right direction will have an undesired effect; the camera will get closer or farther from the ground
	// Using the projected right vector (onto the ground plane) makes more sense because we will keep the same distance from the ground plane]
	position += glm::normalize(right) * distance;
}

void Camera::RotateFirstPerson_OX(float angle)
{
	// Compute the new "forward" and "up" vectors
	// Attention! Don't forget to normalize the vectors
	// Use glm::rotate()
	glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, right) * glm::vec4(forward, 0);
	forward = glm::normalize(glm::vec3(aux));
	up = glm::cross(right, forward);
}

void Camera::RotateFirstPerson_OY(float angle)
{
	// Compute the new "forward", "up" and "right" vectors
	// Don't forget to normalize the vectors
	// Use glm::rotate()
	glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 0);
	forward = glm::normalize(glm::vec3(aux));

	aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 0);
	right = glm::normalize(glm::vec3(aux));

	up = glm::cross(right, forward);
}

void Camera::RotateFirstPerson_OZ(float angle)
{
	// Compute the new Right and Up, Forward stays the same
	// Don't forget to normalize the vectors
	glm::vec4 aux = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
	right = glm::normalize(glm::vec3(aux));

	aux = glm::rotate(glm::mat4(1.f), angle, forward) * glm::vec4(up, 0);
	forward = glm::normalize(glm::vec3(aux));

	up = glm::cross(right, forward);
}

void Camera::RotateThirdPerson_OX(float angle)
{
	// Rotate the camera in Third Person mode - OX axis
	// Use distanceToTarget as translation distance
	TranslateForward(distanceToTarget);
	RotateFirstPerson_OX(angle);
	TranslateForward(-distanceToTarget);
}

void Camera::RotateThirdPerson_OY(float angle)
{
	// Rotate the camera in Third Person mode - OY axis
	TranslateForward(distanceToTarget);
	RotateFirstPerson_OY(angle);
	TranslateForward(-distanceToTarget);
}

void Camera::RotateThirdPerson_OZ(float angle)
{
	// Rotate the camera in Third Person mode - OZ axis
	TranslateForward(distanceToTarget);
	RotateFirstPerson_OZ(angle);
	TranslateForward(-distanceToTarget);
}

glm::mat4 Camera::GetViewMatrix() const
{
	// Returns the View Matrix
	return glm::lookAt(position, position + forward, up);
}

glm::vec3 Camera::GetTargetPosition() const
{
	return position + forward * distanceToTarget;
}

glm::vec3 Camera::GetPosition() const
{
	return position;
}

glm::vec3 Camera::GetRightVector() const
{
	return right;
}

glm::vec3 Camera::GetUpVector() const
{
	return up;
}

glm::vec3 Camera::GetForwardXOZ() const
{
	return glm::vec3(forward.x, 0, forward.z);
}

glm::vec3 Camera::GetForwardVector() const
{
	return forward;
}

