#pragma once
#include<vector>
#include"vulkan/vulkan.hpp"
namespace vkContext
{
	class Framebuffers // in 3d world
	{
	public:
		Framebuffers(int width, int height, vk::RenderPass renderPass);
		~Framebuffers();
		Framebuffers(Framebuffers& rhs) = delete;
		void ResizeFramebufferSize(int width,int height);
		
		vk::ImageView& GetView() { return this->imageView; }
		vk::Framebuffer& GetFramebuffer() { return this->framebuffer; }
		vk::Extent2D GetExtent() { return {static_cast<uint32_t>(width),static_cast<uint32_t>(height)}; }

	private:

		vk::RenderPass renderPass;

		vk::Image image;
		vk::DeviceMemory memory;// image dst memory
		vk::ImageView imageView;

		vk::Image depthImage;
		vk::DeviceMemory depthMemory;
		vk::ImageView depthImageView;

		vk::Framebuffer framebuffer;
		vk::CommandBuffer cmdBuffer;

		int32_t width;
		int32_t height;

		void CreateImages();
		void CreateFramebuffers();
		void CreateImageViews();
		void CreateDepthResources();

		
	};
}