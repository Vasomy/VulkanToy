#include"Framebuffers.hpp"
#include"RHIInterface.hpp"
#include"../Context.h"

vkContext::Framebuffers::Framebuffers(int width,int height,vk::RenderPass renderPass)
{
	this->width = width;
	this->height = height;
	this->renderPass = renderPass;
	CreateImages();
	CreateImageViews();
	CreateDepthResources();
	CreateFramebuffers();

}

vkContext::Framebuffers::~Framebuffers()
{
	auto& device = Context::GetInstance().device;
	auto& cmdManager = Context::GetInstance().commandManager;

	cmdManager->FreeCommandBuffer(cmdBuffer);

	device.destroyFramebuffer(framebuffer);

	device.destroyImageView(imageView);
	device.destroyImage(image);
	device.freeMemory(memory);

	device.destroyImageView(depthImageView);
	device.destroyImage(depthImage);
	device.freeMemory(depthMemory);
}

void vkContext::Framebuffers::ResizeFramebufferSize(int width, int height)
{
	
	
}

void vkContext::Framebuffers::CreateImages()
{
	auto& device = Context::GetInstance().device;
	auto& graphicsQueue = Context::GetInstance().graphicsQueue;
	auto& cmdManager = Context::GetInstance().commandManager;

	vk::ImageCreateInfo createInfo;
	createInfo.sType = vk::StructureType::eImageCreateInfo;
	createInfo.imageType = vk::ImageType::e2D;
	createInfo.format = vk::Format::eR8G8B8A8Srgb;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;
	createInfo.initialLayout = vk::ImageLayout::eUndefined;
	createInfo.samples = vk::SampleCountFlagBits::e1;
	createInfo.tiling = vk::ImageTiling::eLinear;
	createInfo.usage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled;
	
	image = device.createImage(createInfo);
	
	vk::MemoryRequirements memRequirements;
	vk::MemoryAllocateInfo memAllocInfo;

	memAllocInfo.sType = vk::StructureType::eMemoryAllocateInfo;
	// memory info
	memRequirements = device.getImageMemoryRequirements(image);
	memAllocInfo.allocationSize = memRequirements.size;
	// memory must be host visible to copy from
	memAllocInfo.memoryTypeIndex = VulkanRHI::FindMemoryType(memRequirements.memoryTypeBits, 
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	memory = device.allocateMemory(memAllocInfo);
	
	device.bindImageMemory(image, memory, 0);

	auto copyCmdBuffer = cmdManager->BeginSingleTimeCommands();
	
	// insert image memory barrier
	std::cout << "AAAA     ";
	cmdManager->ExecuteCommand(graphicsQueue,
		[&](vk::CommandBuffer& cmdBuffer)
		{
			vk::ImageMemoryBarrier barrier;
			barrier.sType = vk::StructureType::eImageMemoryBarrier;
			barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
			barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
			barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
			barrier.dstAccessMask = vk::AccessFlagBits::eMemoryRead;
			barrier.oldLayout = vk::ImageLayout::eUndefined;
			barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
			barrier.image = image;
			barrier.subresourceRange = vk::ImageSubresourceRange{ vk::ImageAspectFlagBits::eColor,0,1,0,1 };
			cmdBuffer.pipelineBarrier(
				vk::PipelineStageFlagBits::eTransfer,
				vk::PipelineStageFlagBits::eTransfer,
				(vk::DependencyFlags)0,
				0,nullptr,
				0,nullptr,
				1,&barrier
			);
		});
}

void vkContext::Framebuffers::CreateFramebuffers()
{
	auto& device = Context::GetInstance().device;

	std::array<vk::ImageView, 1> attachments = {
		imageView//,
		//depthImageView
	};
	
	vk::FramebufferCreateInfo createInfo{};
	createInfo.sType = vk::StructureType::eFramebufferCreateInfo;
	createInfo.renderPass = renderPass;
	createInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	createInfo.pAttachments = attachments.data();
	createInfo.width = width;
	createInfo.height = height;
	createInfo.layers = 1;
	auto res = device.createFramebuffer(&createInfo, nullptr, &framebuffer);
	if (!VulkanRHI::CheckVKResult(res))
	{
		throw std::runtime_error("failed to create framebuffer!");
	}

}

void vkContext::Framebuffers::CreateImageViews()
{
	imageView = CreateImageView(image, 
		vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void vkContext::Framebuffers::CreateDepthResources()
{
	vk::Format depthFormt = VulkanRHI::FindDepthFormat();

	CreateImage(width, height, depthFormt, 
		vk::ImageTiling::eOptimal, 
		vk::ImageUsageFlagBits::eDepthStencilAttachment,
		vk::MemoryPropertyFlagBits::eDeviceLocal, 
		depthImage, depthMemory);

	depthImageView = CreateImageView(depthImage,depthFormt,vk::ImageAspectFlagBits::eDepth);

	TransitionImageLayout(depthImage, depthFormt,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);

}