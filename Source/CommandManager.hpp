#pragma once
#include"vulkan/vulkan.hpp"
#include<vector>
#include<functional>
namespace vkContext
{
	class CommandManager
	{
	public:
		CommandManager();
		~CommandManager();

		void ResetCommandPool();
		void FreeCommandBuffer(vk::CommandBuffer&buffer);

		std::vector<vk::CommandBuffer> CreateCommandBuffer(uint32_t count);
		vk::CommandBuffer CreateOneCommandBuffer();

		using RecordCmdFunc = std::function<void(vk::CommandBuffer&)>;
		void ExecuteCommand(vk::Queue&, RecordCmdFunc);

	private:
		vk::CommandPool cmdPool;
		
		void CreateCommandPool();
	};
}