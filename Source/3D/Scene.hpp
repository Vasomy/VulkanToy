#pragma once
#include"vulkan/vulkan.hpp"
#include"RawMesh.hpp"
#include"GameObject/GameObject.hpp"
#include"Camera.hpp"
#include<memory>
namespace JRender
{
	struct ObjectConstants
	{
		glm::mat4 view;
		glm::mat4 proj;
		const ObjectConstants* data() const
		{
			return this;
		}
	};
	class Scene
	{
	public:
		glm::mat4 d_view;
		glm::mat4 d_proj;
		RenderCamera camera;
	public:
		std::shared_ptr<JGameObject> go = nullptr;

		std::vector<std::shared_ptr<JGameObject>> gameObjects;

		Scene();
		~Scene();
		void Init();
		void Tick(float dt);
		void CreateRectMesh();
		std::shared_ptr<vkContext::UploadBuffer> mvpMat;
		void UpdateSets();
	private:
		
	};
}