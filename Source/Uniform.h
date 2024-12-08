#pragma once
#include"vulkan/vulkan.hpp"
namespace vkContext
{
	struct UColor final
	{
		union
		{
			struct 
			{
				float r, g, b;
			};
			struct
			{
				float x, y, z;
			};
		};
	};
	class Uniform final
	{
	public:
		UColor color;
		
		static vk::DescriptorSetLayoutBinding GetBinding()
		{
			vk::DescriptorSetLayoutBinding binding;
			binding
				.setBinding(0)
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				.setDescriptorCount(1)
				;
			return binding;
		}
	};
}