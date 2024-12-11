#include"SwapChain.h"
#include"Context.h"
#include<iostream>
namespace vkContext
{
	Swapchain::Swapchain(int w,int h)
	{
		width = w;
		height = h;

		QueryInfo(w, h);
		vk::SwapchainCreateInfoKHR createInfo;
		createInfo
			.setClipped(true)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
			.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
			.setSurface(Context::GetInstance().surface)
			.setImageColorSpace(info.format.colorSpace)
			.setImageFormat(info.format.format)
			.setImageExtent(info.imageExtent)
			.setMinImageCount(info.imageCount)
			.setPreTransform(info.transform)
			.setPresentMode(info.presentMod)
			;

		auto& queueIndices = Context::GetInstance().queueFamilyIndices;
		if(queueIndices.graphicsQueue.value() == queueIndices.presentQueue.value())
		{
			createInfo.setQueueFamilyIndices(queueIndices.graphicsQueue.value())
				.setImageSharingMode(vk::SharingMode::eExclusive);
		}
		else
		{
			std::array indices = {
				queueIndices.graphicsQueue.value(),
				queueIndices.presentQueue.value()
			};
			createInfo.setQueueFamilyIndices(indices)
				.setImageSharingMode(vk::SharingMode::eConcurrent);
		}
		swapchain = Context::GetInstance().device.createSwapchainKHR(createInfo);

		GetImages();
		CreateImageViews();
	}
	void Swapchain::QueryInfo(int w,int h)
	{
		auto& phyDevice = Context::GetInstance().phyDevice;
		auto& surface = Context::GetInstance().surface;

		auto formats = phyDevice.getSurfaceFormatsKHR(surface);

		info.format = formats[0];
		for (const auto& format : formats)
		{
			if (format.format == vk::Format::eR8G8B8A8Srgb&&
				format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
			{
				info.format = format;
			}
		}
		auto capbilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
		info.imageCount = std::clamp<uint32_t>(2,capbilities.minImageCount,capbilities.maxImageCount);

		info.imageExtent.width = std::clamp<uint32_t>(w, capbilities.minImageExtent.width, capbilities.maxImageExtent.width);
		info.imageExtent.height = std::clamp<uint32_t>(h, capbilities.minImageExtent.height, capbilities.maxImageExtent.height);

		info.transform = capbilities.currentTransform;

		auto presents = phyDevice.getSurfacePresentModesKHR(surface);
		info.presentMod = presents[0];
		for (auto present : presents)
		{
			if (present == vk::PresentModeKHR::eMailbox)
			{
				info.presentMod = present;
				break;
			}
		}

	}

	void Swapchain::GetImages()
	{
		images = Context::GetInstance().device.getSwapchainImagesKHR(swapchain);
	}
	void Swapchain::CreateImageViews()
	{
		imageViews.resize(images.size());

		for (int i =0;i<imageViews.size();++i)
		{
			vk::ImageViewCreateInfo createInfo;
			vk::ComponentMapping mapping;
			vk::ImageSubresourceRange range;
			range
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				;
			createInfo
				.setImage(images[i])
				.setViewType(vk::ImageViewType::e2D)
				.setComponents(mapping)
				.setFormat(info.format.format)
				.setSubresourceRange(range)
				
				;
			imageViews[i] = Context::GetInstance().device.createImageView(createInfo);
		}
	}

	void Swapchain::CreateFramebuffers()
	{
		framebuffers.resize(images.size());
		for (int i =0;i<framebuffers.size();++i)
		{
			vk::FramebufferCreateInfo createInfo;
			createInfo
				.setAttachments(imageViews[i])
				.setWidth(width)
				.setHeight(height)
				.setRenderPass(Context::GetInstance().renderProcess->renderPass)
				.setLayers(1)
				;
			framebuffers[i] = Context::GetInstance().device.createFramebuffer(createInfo);
	
		}
	}

	Swapchain::~Swapchain()
	{
		for (auto& framebuffer : framebuffers)
		{
			Context::GetInstance().device.destroyFramebuffer(framebuffer);
		}
		for (auto& imageView : imageViews)
		{
			Context::GetInstance().device.destroyImageView(imageView);
		}
		Context::GetInstance().device.destroySwapchainKHR(swapchain);
	}

}