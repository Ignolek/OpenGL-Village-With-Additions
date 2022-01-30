#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Caterpilar_Movement
{
	FORWARD_CAT,
	BACKWARD_CAT,
	LEFT_CAT,
	RIGHT_CAT
};

// Default caterpilar values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.0f;

class CaterpilarMovement
{
public:
	// caterpilar attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	// euler Angles
	float Yaw;
	float Pitch;

	// caterpilar options
	float MovementSpeed;

	CaterpilarMovement(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCaterpilarVectors();
	}
	// constructor with scalar values
	CaterpilarMovement(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCaterpilarVectors();
	}

	glm::mat4 GetViewMatrix(glm::mat4 matrixToUpdate)
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	// TU trzeba pokombinowac
	void ProcessKeyboardCat(Caterpilar_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD_CAT)
			Position += Front * velocity;
		if (direction == BACKWARD_CAT)
			Position -= Front * velocity;
		if (direction == LEFT_CAT)
			Position -= Right * velocity;
		if (direction == RIGHT_CAT)
			Position += Right * velocity;
	}

	void ProcessRotateMovement(float xOffset, float yOffset)
	{
		Yaw += xOffset;
		Pitch += yOffset;

		updateCaterpilarVectors();
	}

private:
	void updateCaterpilarVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
};