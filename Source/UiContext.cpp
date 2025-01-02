#include"UiContext.hpp"
#include"Context.h"
#include"DescriptorManager.hpp"
#include"QEM.h"
#include"Debug/LogInfo.hpp"
namespace vkContext {

	static void check_vk_result(VkResult err)
	{
		if (err == 0)
			return;
		fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
		if (err < 0)
			abort();
	}

	std::unique_ptr<UiContext>UiContext::instance_ = nullptr;

	void vkContext::UiContext::Init()
	{
		instance_.reset(new UiContext());
		instance_->InitImGui();
	}

	void vkContext::UiContext::Quit()
	{
		instance_.reset();
	}

	UiContext::UiContext()
	{
	}

	UiContext::~UiContext()
	{
		auto& vulkanInstance = Context::GetInstance().instance;
		auto& device = Context::GetInstance().device;
		vkDestroyDescriptorPool(device, descPool, nullptr);
		auto err = vkDeviceWaitIdle(device);
		check_vk_result(err);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		ImGui_ImplVulkanH_DestroyWindow(vulkanInstance, device, &imguiVulkanWindow, nullptr);
	}

	void vkContext::UiContext::InitImGui()
	{
		auto& context = Context::GetInstance();
		auto& phyDevice = Context::GetInstance().phyDevice;
		auto& device = Context::GetInstance().device;
		auto& queueFamily = Context::GetInstance().queueFamilyIndices.graphicsQueue;
		auto& vulkanInstance = Context::GetInstance().instance;
		auto& graphicsQueue = Context::GetInstance().graphicsQueue;
		int w, h;
		glfwGetFramebufferSize(context.m_window,&w,&h);
		
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		CreateDescriptorPool();

		auto& surface = Context::GetInstance().surface;
		if (!surface)
		{
			std::cout << "NULL surface!";
			assert(0);
		}
		SetupImGuiVulkanWindow(&imguiVulkanWindow, surface, w, h);
		ImGui_ImplVulkanH_Window* wd = &imguiVulkanWindow;

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(context.m_window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = context.instance;
		init_info.PhysicalDevice = phyDevice;
		init_info.Device = device;
		init_info.QueueFamily = queueFamily.value();
		init_info.Queue = graphicsQueue;
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = descPool;
		init_info.Allocator = VK_NULL_HANDLE;
		init_info.MinImageCount = 2;
		init_info.ImageCount = wd->ImageCount;
		ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);


		// Upload Fonts
		{
			// Use any command queue
			VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
			VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

			auto err = vkResetCommandPool(device, command_pool, 0);
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(command_buffer, &begin_info);

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			err = vkEndCommandBuffer(command_buffer);
			//check_vk_result(err);
			err = vkQueueSubmit(graphicsQueue, 1, &end_info, VK_NULL_HANDLE);
			//check_vk_result(err);

			err = vkDeviceWaitIdle(device);
			//check_vk_result(err);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}


	}
	void UiContext::UiRender()
	{

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		auto& context = Context::GetInstance();
		auto& phyDevice = Context::GetInstance().phyDevice;
		auto& device = Context::GetInstance().device;
		auto& queueFamily = Context::GetInstance().queueFamilyIndices.graphicsQueue;
		auto& vulkanInstance = Context::GetInstance().instance;
		auto& graphicsQueue = Context::GetInstance().graphicsQueue;
		auto& swapchain = Context::GetInstance().swapchain;
		auto currentFrameIndex = Context::GetInstance().renderer->currentFrameIndex;
		if (ImGui::Begin("ViewPort"))
		{
			/*vk::DescriptorSetLayout set;
			vk::DescriptorSetLayoutCreateInfo ci;
			vk::DescriptorSetLayoutBinding dsb;
			dsb
				.setBinding(0)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setDescriptorCount(1)
				.setStageFlags(vk::ShaderStageFlagBits::eFragment)
				;
			ci.setBindings(dsb);
			set = device.createDescriptorSetLayout(ci);*/
			auto id = ImGui_ImplVulkan_AddTexture(Context::GetInstance().sampler,
				swapchain->imageViews[currentFrameIndex], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			ImGui::Image((ImTextureID)id, { 600,400 });
		}
		ImGui::End();

		if (ImGui::Begin("QEM settings"))
		{
			static int numsFaces = 2000;
			auto& renderer = context.renderer;
			auto& scene = renderer->scene;
			ImGui::SliderInt("Face Nums : ", &numsFaces, 1000, 10000);
			if (ImGui::Button("Do QEM"))
			{
				device.waitIdle();
				scene->go.get()->meshes.back().reset();
				scene->go.get()->meshes.pop_back();
				
				auto* polyMesh = qem_simplification(numsFaces);
				auto vertices = polyMesh->GetVertices();
				auto indices = polyMesh->GetIndices();
				auto* ptr = scene->go.get();

				std::shared_ptr<JRender::RawMesh>mesh;
				mesh.reset(new JRender::RawMesh(
					vertices.size(),indices.size(),vertices.data(),indices.data()
				));
				//ptr->FreeAllMesh();
				mesh->position = { 0,0,0 };
				ptr->meshes.push_back(mesh);
				delete polyMesh;
				
			
			}
			ImGui::End();
		}

		device.waitIdle();

		ImGui::Render();
		auto drawData = ImGui::GetDrawData();
		auto* wd = &imguiVulkanWindow;
		VkResult err;

		VkSemaphore image_acquired_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		err = vkAcquireNextImageKHR(device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
		{
			//g_SwapChainRebuild = true;
			return;
		}
		//check_vk_result(err);

		ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
		{
			err = vkWaitForFences(device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
			check_vk_result(err);

			err = vkResetFences(device, 1, &fd->Fence);
			check_vk_result(err);
		}
		{
			err = vkResetCommandPool(device, fd->CommandPool, 0);
			check_vk_result(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
			check_vk_result(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = wd->RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = wd->Width;
			info.renderArea.extent.height = wd->Height;
			info.clearValueCount = 1;
			info.pClearValues = &wd->ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
			
		}

		// Record dear imgui primitives into command buffer
		ImGui_ImplVulkan_RenderDrawData(drawData, fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			check_vk_result(err);
			err = vkQueueSubmit(graphicsQueue, 1, &info, fd->Fence);
			check_vk_result(err);
		}
		ImGuiIO& io = ImGui::GetIO(); (void)io;
													
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		UiFramePresent();
	}
	void UiContext::SetupImGuiVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
	{
		wd->Surface = surface;
		auto& phyDevice = Context::GetInstance().phyDevice;
		auto& device = Context::GetInstance().device;
		auto& queueFamily = Context::GetInstance().queueFamilyIndices.graphicsQueue;
		auto& vulkanInstance = Context::GetInstance().instance;
		// Check for WSI support
		VkBool32 res;
		vkGetPhysicalDeviceSurfaceSupportKHR(phyDevice, queueFamily.value(), wd->Surface, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(phyDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);

		// Select Present Mode
#ifdef IMGUI_UNLIMITED_FRAME_RATE
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
		VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
		wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(phyDevice, wd->Surface, &present_modes[0], IM_ARRAYSIZE(present_modes));
		//printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		
		ImGui_ImplVulkanH_CreateOrResizeWindow(vulkanInstance, phyDevice, device, wd, queueFamily.value(), nullptr, width, height, 2);


	}
	void UiContext::CreateDescriptorPool()
	{
		auto& device = Context::GetInstance().device;
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		auto err = vkCreateDescriptorPool(device, &pool_info, nullptr, &descPool);
		
	}
	void UiContext::UiFramePresent()
	{
		auto* wd = &imguiVulkanWindow;
		auto& graphicsQueue = Context::GetInstance().graphicsQueue;
		/*if (g_SwapChainRebuild)
			return;*/
		VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &wd->Swapchain;
		info.pImageIndices = &wd->FrameIndex;
		VkResult err = vkQueuePresentKHR(graphicsQueue, &info);
		if (err == VK_ERROR_OUT_OF_DATE_KHR)
		{
			//g_SwapChainRebuild = true;
			return;
		}
		check_vk_result(err);
		wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->ImageCount; // Now we can use the next set of semaphores
	}
	void UiContext::CreateViewPortSet()
	{
		viewportSet = DescriptorSetManager::GetInstance().AllocImageSet();
	}
	void UiContext::UpdateViewPortSet()
	{
		auto& swapchain = Context::GetInstance().swapchain;
		auto currentFrameIndex = Context::GetInstance().renderer->currentFrameIndex;
		vk::WriteDescriptorSet writer;
		vk::DescriptorImageInfo imageInfo;
		imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(swapchain->imageViews[currentFrameIndex])
			.setSampler(Context::GetInstance().sampler);
		writer.setImageInfo(imageInfo)
			.setDstBinding(0)
			.setDstArrayElement(0)
			.setDstSet(viewportSet.set)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
		Context::GetInstance().device.updateDescriptorSets(writer, {});

	}
}