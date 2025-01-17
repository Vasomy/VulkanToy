#pragma once
#include"RHIDynamic.hpp"
#include"VulkanHelper.hpp"
#include"../Context.h"

// ����using ������ʱȡ�� ����Ϊ��ʵ�ֵ�RHI��Դ��
// ĿǰRHI����ֻ��Vulkanʵ��

using RImage = vk::Image;
using RImageView = vk::ImageView;
using RFormat = vk::Format;
using RImageUsage = vk::ImageUsageFlagBits;
using RMemoryProperty = vk::MemoryPropertyFlagBits;
using RTiling = vk::ImageTiling;
// �������Ϊvulkan���и��������Image��������ʵ��
// -------------------------
//class RVulkanImage : public RImage
//{
//	vk::DeviceMemory memory;
//};
using RDeviceMemory = vk::DeviceMemory;
// -------------------------------------
inline static RImage CreateImage(
	int imageWidth,
	int imageHeight,
	RFormat format,
	RTiling tiling,
	RImageUsage usage,
	RMemoryProperty propeties,
	RImage& image,
	RDeviceMemory& memory 
)
{
	auto& context = vkContext::Context::GetInstance();
	auto& device = context.device;
	// ��������Ӧ����RHI�ĵ����е���
	vk::ImageCreateInfo createInfo;
	createInfo.sType = vk::StructureType::eImageCreateInfo;
	createInfo.imageType = vk::ImageType::e2D;
	createInfo.extent
		.setWidth(imageWidth)
		.setHeight(imageHeight)
		.setDepth(1)
		;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = vk::ImageLayout::eUndefined;
	createInfo.usage = usage;
	createInfo.samples = vk::SampleCountFlagBits::e1;
	createInfo.sharingMode = vk::SharingMode::eExclusive;

	if (device.createImage(&createInfo, nullptr, &image) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create image!");
	}

	vk::MemoryRequirements memRequirements;
	device.getImageMemoryRequirements(image, &memRequirements);

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanRHI::FindMemoryType(memRequirements.memoryTypeBits, propeties);
	memory = device.allocateMemory(allocInfo);

	device.bindImageMemory(image, memory, 0);

}

inline static RImageView CreateImageView(vk::Image& image,
	vk::Format format,
	vk::ImageAspectFlagBits aspectFlags)
{
	auto& context = vkContext::Context::GetInstance();
	auto& device = context.device;

	vk::ImageViewCreateInfo createInfo;
	createInfo.sType = vk::StructureType::eImageViewCreateInfo;
	createInfo.image = image;
	createInfo.viewType = vk::ImageViewType::e2D;
	createInfo.format = format;
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	vk::ImageView imageView;
	
	if (device.createImageView(&createInfo, nullptr, &imageView) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create texture image view");
	}

	return imageView;
}

inline static void TransitionImageLayout(
	vk::Image&image,
	vk::Format format,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout
)
{
	auto& context = vkContext::Context::GetInstance();
	auto& graphicsQueue = context.graphicsQueue;

	auto& cmdManager = vkContext::Context::GetInstance().commandManager;
	cmdManager->ExecuteCommand(graphicsQueue,
		[&](vk::CommandBuffer& cmdBuffer)
		{
			vk::ImageMemoryBarrier barrier;
			barrier.sType = vk::StructureType::eImageMemoryBarrier;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;
			barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
			barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
			barrier.image = image;
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			vk::PipelineStageFlags srcStage;
			vk::PipelineStageFlags dstStage;

			if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
			{
				barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

				if (VulkanRHI::FormatHasStencilComponent(format))
				{
					barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
				}

			}
			else
			{
				barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			}

			if (oldLayout == vk::ImageLayout::eUndefined 
				&& newLayout == vk::ImageLayout::eTransferDstOptimal)
			{
				barrier.srcAccessMask = vk::AccessFlagBits::eNone;
				barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

				srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
				dstStage = vk::PipelineStageFlagBits::eTransfer;
			}
			else if (oldLayout == vk::ImageLayout::eTransferDstOptimal
				&& newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			{
				barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

				srcStage = vk::PipelineStageFlagBits::eTransfer;
				dstStage = vk::PipelineStageFlagBits::eFragmentShader;
			}
			else if (oldLayout == vk::ImageLayout::eUndefined
				&& newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
			{
				barrier.srcAccessMask = vk::AccessFlagBits::eNone;
				barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead;

				srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
				dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
			}
			else

			{
				throw std::invalid_argument("unsupported layout transition!");
			}
			cmdBuffer.pipelineBarrier(srcStage, dstStage,
				(vk::DependencyFlags)0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
		}
	);
}