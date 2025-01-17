#include"vulkan/vulkan.hpp"
#include"../Context.h"
namespace VulkanRHI
{
	using namespace vkContext;
	inline static uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
	{
		auto& phyDevice = Context::GetInstance().phyDevice;
		vk::PhysicalDeviceMemoryProperties memProperties;
		phyDevice.getMemoryProperties(&memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");

	}

	inline static bool CheckVKResult(vk::Result res)
	{
		return res == vk::Result::eSuccess;
	}

	inline static vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlagBits features)
	{
		auto& phyDevice = Context::GetInstance().phyDevice;
		for (vk::Format format : candidates)
		{
			vk::FormatProperties property;
			phyDevice.getFormatProperties(format, &property);

			if (tiling == vk::ImageTiling::eLinear && (property.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == vk::ImageTiling::eOptimal && (property.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	inline static vk::Format FindDepthFormat()
	{
		return FindSupportedFormat(
			{vk::Format::eD32Sfloat,vk::Format::eD32SfloatS8Uint,vk::Format::eD24UnormS8Uint},
			vk::ImageTiling::eOptimal,
			vk::FormatFeatureFlagBits::eDepthStencilAttachment
		);
	}

	inline static bool FormatHasStencilComponent(vk::Format format)
	{
		return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
	}

}