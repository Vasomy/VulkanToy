#pragma once
#include"glm/glm.hpp"
#include"vulkan/vulkan.hpp"
#include"GLFW/glfw3.h"
#include<memory>
#include<optional>
#include"SwapChain.h"
#include"Shader.h"
#include"Tools.h"
#include"RenderProcess.h"
#include"Renderer.h"
#include"CommandManager.hpp"


namespace vkContext
{
	class Context final
	{
	public:


		GLFWwindow* m_window;

		static void Init(std::vector<const char*>&extensions,GLFWwindow*window,int w,int h);
		static void Quit();
		static Context& GetInstance();
		static Renderer& GetRenderer();
		
		~Context();
		int width, height;
		vk::Instance instance;
		vk::PhysicalDevice phyDevice;
		vk::Device device;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		std::unique_ptr<Swapchain> swapchain;
		std::unique_ptr<RenderProcess> renderProcess;
		std::unique_ptr<Shader>shader;
		std::unique_ptr<Renderer> renderer;

		VkSurfaceKHR surface;
		
		std::unique_ptr<CommandManager>commandManager;
		vk::CommandPool cmdPool;// comment
		

	private:
		Context(std::vector<const char*>& extensions, GLFWwindow* window);
		void Init();
		
		static std::unique_ptr<Context>instance_;

		void CreateInstance(std::vector<const char*>& extensions);//vkinstance
		void PickUpPhyicsDevice();
		void QueryQueueFamilyIndices();
		void CreateDevice();
		void GetQueue();

		// call in static init
		void InitSwapchain(int w, int h);
		void InitRenderer();

		void InitCommandPool();

		//destroy
		void DestroySwapchain();

		struct QueueFamilyIndices
		{
			std::optional<uint32_t>graphicsQueue;
			std::optional<uint32_t>presentQueue;

			operator bool()const
			{
				return graphicsQueue.has_value() && presentQueue.has_value();
			}
		};
	public:
		QueueFamilyIndices queueFamilyIndices;

	};
}