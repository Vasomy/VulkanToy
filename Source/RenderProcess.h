#pragma once
#include"vulkan/vulkan.hpp"
#include"Uniform.h"
namespace vkContext
{
	class RenderProcess
	{
	public:
		RenderProcess();
		vk::Pipeline pipeline;
		vk::PipelineLayout layout;
		vk::DescriptorSetLayout setLayout;
		vk::RenderPass renderPass;

		void InitPipeline(int width,int height);
		void InitRenderPass();
		vk::PipelineLayout InitLayout();
		vk::DescriptorSetLayout CreateSetLayout();
		~RenderProcess();
	private:
	};
}