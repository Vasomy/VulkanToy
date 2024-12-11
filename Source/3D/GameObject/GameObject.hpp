#pragma once

#include"vulkan/vulkan.hpp"
#include"../../UploadBuffer.hpp"
#include"../RawMesh.hpp"
#include"../../Texture.hpp"
#include<string>
#include<vector>
#include<memory>
namespace JRender
{
	class JGameObject
	{
	public:
		
		JGameObject(const std::string& name);
		JGameObject(JGameObject& rhs) = default;
		
		uint32_t id;
		std::string name;
		std::vector<std::shared_ptr<RawMesh>> meshes;
		std::unique_ptr<vkContext::Texture> texture;
		void LoadTexture(const char* filename);
		vkContext::Texture& GetTexture() { return *texture; }
	};
}