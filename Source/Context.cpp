#include"Context.h"
#include"GLFW/glfw3.h"
#include<iostream>
namespace vkContext
{
	std::unique_ptr<Context>Context::instance_ = nullptr;

	void Context::Init(std::vector<const char*>& extensions, GLFWwindow* window, int w, int h)
	{
		instance_.reset(new Context(extensions, window));
		instance_->Init();
		//instance_->InitSwapchain(w, h);
		//instance_->shader.reset(new Shader(tools::ReadBinFile("asset/vertex.spv"), tools::ReadBinFile("asset/frag.spv")));
		//instance_->renderProcess->InitRenderPass();
		//instance_->renderProcess->CreateSetLayout();
		//instance_->renderProcess->InitLayout();
		//instance_->renderProcess->InitPipeline(w,h);
		//instance_->swapchain->CreateFramebuffers();
		//instance_->InitRenderer();
	}
	void Context::Quit()
	{
		instance_->device.waitIdle();
		instance_->renderProcess.reset();
		instance_->shader.reset();
		instance_->renderer.reset();
		instance_->DestroySwapchain();
		instance_.reset();
	}

	Context& Context::GetInstance()
	{
		return*instance_;
	}

	Renderer& Context::GetRenderer()
	{
		return *(instance_->renderer);
	}

	Context::Context(std::vector<const char*>& extensions, GLFWwindow* window)
	{
		glfwGetWindowSize(window, &width, &height);
		std::cout << width << ' ' << height << '\n';
		//InitSwapchain();

		m_window = window;
		CreateInstance(extensions);
	
		PickUpPhyicsDevice();
		auto res = glfwCreateWindowSurface(instance, window, NULL, &surface);
		if (res != VK_SUCCESS)
			std::cout << "Create surface failed!\n";
		if (surface == VK_NULL_HANDLE)
			std::cout << "Surface is null handle\n";

		QueryQueueFamilyIndices();
		CreateDevice();
		GetQueue();
	}

	void Context::Init()
	{
		InitSwapchain(width,height);
		shader.reset(new Shader(tools::ReadBinFile("asset/vertex.spv"), tools::ReadBinFile("asset/frag.spv")));
		renderProcess.reset(new RenderProcess);
		swapchain->CreateFramebuffers();
		InitCommandPool();
		InitRenderer();

	}

	void Context::CreateInstance(std::vector<const char*>& extensions)
	{
		vk::InstanceCreateInfo createInfo;
		std::vector<const char*> enabled_layer_name = { "VK_LAYER_KHRONOS_validation" };
		/*auto layers = vk::enumerateInstanceLayerProperties();
		for (auto layer : layers)
		{
			std::cout << layer.layerName << '\n';
		}*/

		vk::ApplicationInfo applicationInfo;
		applicationInfo.setApiVersion(vk::ApiVersion13);
		createInfo.setPApplicationInfo(&applicationInfo)
			.setPEnabledLayerNames(enabled_layer_name);

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		instance = vk::createInstance(createInfo);
	}

	void Context::PickUpPhyicsDevice()
	{
		auto devices = instance.enumeratePhysicalDevices();
		phyDevice = devices[0];
		std::cout <<"Physics Device Name:"<< phyDevice.getProperties().deviceName << '\n';
	}

	void Context::CreateDevice()
	{
		std::array extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::DeviceCreateInfo createInfo;
		std::vector<vk::DeviceQueueCreateInfo>queueCreateInfos;
		vk::DeviceQueueCreateInfo queueCreateInfo;
		float priorities = 1.0f;

		if (queueFamilyIndices.graphicsQueue.value() == queueFamilyIndices.presentQueue.value())
		{
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
			queueCreateInfos.push_back(queueCreateInfo);
		}
		else
		{
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndices.graphicsQueue.value());
			queueCreateInfos.push_back(queueCreateInfo);
			queueCreateInfo.setPQueuePriorities(&priorities)
				.setQueueCount(1)
				.setQueueFamilyIndex(queueFamilyIndices.presentQueue.value());
			queueCreateInfos.push_back(queueCreateInfo);
		}


		createInfo.setQueueCreateInfos(queueCreateInfos)
			.setPEnabledExtensionNames(extensions)
			;
		device = phyDevice.createDevice(createInfo);
	}

	void Context::QueryQueueFamilyIndices()
	{
		auto properties = phyDevice.getQueueFamilyProperties();
		for (int i = 0; i < properties.size(); ++i)
		{
			const auto& property = properties[i];
			if (property.queueFlags & vk::QueueFlagBits::eGraphics)
			{
				queueFamilyIndices.graphicsQueue = i;
				std::cout << "Physic Device :" << phyDevice.getProperties().deviceName << " support queue flag " << "Graphics!\n";
				
			}
			VkBool32 presentSupport = false;

			vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, i, static_cast<VkSurfaceKHR>(surface), &presentSupport);
			if (presentSupport)
			{
				queueFamilyIndices.presentQueue = i;
				std::cout << "Physic Device :" << phyDevice.getProperties().deviceName << " support queue flag " << "surface!\n";
			}
			if (queueFamilyIndices)
			{
				break;
			}

		}
	}

	void Context::GetQueue()
	{
		graphicsQueue = device.getQueue(queueFamilyIndices.graphicsQueue.value(), 0);
		presentQueue = device.getQueue(queueFamilyIndices.presentQueue.value(), 0);
	}

	//call in static init
	void Context::InitSwapchain(int w,int h)
	{
		swapchain.reset(new Swapchain(w, h));
	}

	void Context::InitRenderer()
	{
		renderer.reset(new Renderer);
	}

	void Context::InitCommandPool()
	{
		vk::CommandPoolCreateInfo createInfo;
		createInfo
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			;
		cmdPool = Context::GetInstance().device.createCommandPool(createInfo);
		
	}

	//destroy
	void Context::DestroySwapchain()
	{
		swapchain.reset();
	}

	Context::~Context()
	{
	
		device.destroy();
		delete(surface);
		instance.destroy();
	}

}