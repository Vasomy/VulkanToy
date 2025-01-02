#pragma once
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/quaternion.hpp"
namespace JRender
{
	class RenderCamera
	{
	public:
		RenderCamera();
		RenderCamera(glm::vec3 cameraPosition, uint32_t renderWidth = 0, uint32_t renderHeight = 0);
		~RenderCamera() = default;
		uint32_t renderWidth;
		uint32_t renderHeight;

		glm::vec3 position = glm::vec3(0,0,5);
		glm::vec3 front = glm::vec3(0,0,-1);
		glm::vec3 up = glm::vec3(0,1,0);
		glm::vec3 right = glm::vec3(0, 0, 1);
		float yaw = -90.0f;
		float pitch = 0.0f;

		float mouseSensitivity = 0.1f;
		float moveSpeed = 2.0f;

		float zoom = 45.0f;

		float zNear = 0.1f;
		float zFar = 100.0f;

		glm::mat4 GetView() { return glm::lookAt(position, position + front, up); }
		glm::mat4 GetProj() { 
			float aspect = static_cast<float>(renderWidth) / static_cast<float>(renderHeight);
			return glm::perspective(zoom, aspect, zNear, zFar);
		}

		void RotateCamera(float xOffset, float yOffset);
		void MoveCamera(float dt,glm::vec3 direction);
		void MoveForward(float dt);
		void MoveBackward(float dt);
		void MoveLeft(float dt);
		void MoveRight(float dt);
		void MoveUp(float dt);
		void MoveDown(float dt);
	private:
		void UpdateCameraVectors();

	};
}