#pragma once
#include"GLFW/glfw3.h"
#include"vulkan/vulkan.hpp"

namespace vkContext {
	class Shader
	{
	public:
		Shader(const std::string& vertexSource, const std::string& fragSource);
		~Shader();

		vk::ShaderModule vertexModule;
		vk::ShaderModule fragmentModule;

		std::vector<vk::PipelineShaderStageCreateInfo> GetStage();
		vk::PushConstantRange GetPushConstantsRange();
		const std::vector<vk::DescriptorSetLayout>& GetLayouts() const { return this->layouts; }

	private:
		std::vector<vk::PipelineShaderStageCreateInfo> stage_;
		std::vector<vk::DescriptorSetLayout> layouts;
		void InitStage();
		void InitDescriptorSetLayouts();
	};
}