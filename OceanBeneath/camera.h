#pragma once
#include<glm/glm.hpp>
#include<glm/matrix.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/quaternion.hpp>


class Camera
{
	float cameraWidth=1;
	float cameraHeight=1;

	float destYaw = 0;
	float destPitch = 30;

	float yaw = 0;
	float pitch = 0;

	float fov=60;
	float nearPlane=0.1;
	float farPlane=10000;

	float velocity=10;

	float sensitivity = 0.1;

	bool moveFlag[4] = {false,false ,false,false };

public:

	enum CameraMoveDir
	{
		FORWARD,
		BACKWARD,
		LEFT,
		RIGHT,
	};

	glm::vec3 cameraPos;
	void CameraMove(CameraMoveDir dir,bool move)
	{
		moveFlag[dir] = move;
	}

	glm::vec3 Forward()
	{
		glm::quat rot = glm::quat(glm::vec3(glm::radians(pitch), glm::radians(yaw), 0));
		return  glm::vec3(mat3_cast(rot)*glm::vec3(0, 0, 1));
	}

	glm::mat4 View()
	{
		return lookAt(cameraPos, Forward() + cameraPos, glm::vec3(0, 1, 0));
	}

	glm::mat4 Projection()
	{
		return glm::perspective(glm::radians(fov), cameraWidth / cameraHeight, nearPlane, farPlane);
	}

	void ResizeCamera(float w, float h)
	{
		cameraWidth = w;
		cameraHeight = h;
	}

	void Update(float dt)
	{
		float w = glm::min(1.0f, 10 * dt);
		yaw = yaw*(1 - w) + destYaw*w;
		pitch = pitch*(1 - w) + destPitch*w;

		for (int i=0;i<4;++i)
		{
			if (moveFlag[i])
			{
				glm::vec3 forward = Forward();
				glm::vec3 right = glm::cross(forward, vec3(0, 1, 0));
				right = normalize(right);
				switch ((CameraMoveDir)i)
				{
				case FORWARD:
					cameraPos += forward*dt*velocity;
					break;
				case BACKWARD:
					cameraPos -= forward*dt*velocity;
					break;
				case LEFT:
					cameraPos -= right*dt*velocity;
					break;
				case RIGHT:
					cameraPos += right*dt*velocity;
					break;
				}
			}
		}

	}

	void RotateUpDown(float angle)
	{
		destPitch += sensitivity*angle;
		destPitch = clamp(destPitch, -89.0f, 89.0f);
	}

	void RotateLeftRight(float angle)
	{
		destYaw -= sensitivity*angle;
	}
};