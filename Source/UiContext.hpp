#pragma once
#include"imgui-docking/imgui.h"
#include"imgui-docking/backends/imgui_impl_glfw.h"
#include"imgui-docking/backends/imgui_impl_vulkan.h"
#include"vulkan/vulkan.hpp"
#include"DescriptorManager.hpp"
#include<memory>
namespace vkContext
{
	class UiContext
	{
	public:
		static std::unique_ptr<UiContext> instance_;
		static UiContext& GetInstance() { return *instance_; }
		static void Init();
		static void Quit();

		UiContext();
		~UiContext();

		void InitImGui();
		void UiRender();

	private:
		ImGui_ImplVulkanH_Window imguiVulkanWindow;
		VkDescriptorPool descPool;
		void SetupImGuiVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
		void CreateDescriptorPool();
		void UiFramePresent();

		DescriptorSetManager::SetInfo viewportSet;
		void CreateViewPortSet();
		void UpdateViewPortSet();
		
	};
}