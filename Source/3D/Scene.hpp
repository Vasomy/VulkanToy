#pragma once
#include"vulkan/vulkan.hpp"
#include"RawMesh.hpp"
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

	public:
		RawMesh* mesh = nullptr;
		Scene();
		~Scene();
		void Init();
		void CreateRectMesh();
		std::shared_ptr<vkContext::UploadBuffer> mvpMat;
		void UpdateSets();
	private:
		
	};
}