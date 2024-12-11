#include"Camera.hpp"
#include"../Context.h"
#include"../Debug/LogInfo.hpp"
using namespace vkContext;
namespace JRender
{
    RenderCamera::RenderCamera()
    {
        auto&context = Context::GetInstance();
        renderWidth = context.width;
        renderHeight = context.height;
    }

    RenderCamera::RenderCamera(glm::vec3 cameraPosition, uint32_t renderWidth, uint32_t renderHeight)
    {
        auto& context = Context::GetInstance();

        position = cameraPosition;

        if (renderWidth == 0)
        {
            this->renderWidth = context.width;
        }
        else
        {
            this->renderWidth = renderWidth;
        }
        if (renderHeight == 0)
        {
            this->renderHeight = context.height;
        }
        else
        {
            this->renderHeight = renderHeight;
        }
        UpdateCameraVectors();
    }

    void RenderCamera::RotateCamera(float xOffset, float yOffset)
    {
        xOffset *= mouseSensitivity;
        yOffset *= mouseSensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch >= 90.f)
            pitch = 90.0f;
        if (pitch <= -90.f)
            pitch = -90.0f;
        UpdateCameraVectors();

        //Debug::Log(front);
    }
    void RenderCamera::MoveForward(float dt)
    {
        MoveCamera(dt,front);
    }
    void RenderCamera::MoveBackward(float dt)
    {
        MoveCamera(dt, -front);
    }
    void RenderCamera::MoveLeft(float dt)
    {
        MoveCamera(dt, -right);
    }
    void RenderCamera::MoveRight(float dt)
    {
        MoveCamera(dt, right);
    }
    void RenderCamera::MoveCamera(float dt,glm::vec3 direction)
    {
        position += direction * moveSpeed * dt;
    }

    void RenderCamera::UpdateCameraVectors()
	{
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->front = glm::normalize(front);

        right = glm::normalize(glm::cross(front, up));
        up = glm::normalize(glm::cross(right, front));
	}
}