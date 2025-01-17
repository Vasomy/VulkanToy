#pragma once
#include"glm/glm.hpp"
#include"vulkan/vulkan.hpp"
#include"GLFW/glfw3.h"
namespace vkContext {
	class Swapchain
	{
		struct SwapchainInfo
		{
			vk::Extent2D imageExtent;
			uint32_t imageCount;
			vk::SurfaceFormatKHR format;
			vk::SurfaceTransformFlagBitsKHR transform;
			vk::PresentModeKHR presentMod;
		};
	public:
		vk::SwapchainKHR swapchain;

		Swapchain(int w,int h);
		~Swapchain();

		void QueryInfo(int w,int h);

		void GetImages();
		void CreateImageViews();
		void CreateFramebuffers();

		void PrepareSwapchain();

		int width;
		int height;
		SwapchainInfo info;
		std::vector<vk::Image>images;
		std::vector<vk::ImageView>imageViews;
		std::vector<vk::Framebuffer>framebuffers;

		int currentIndex = 0;
		
	};
}
