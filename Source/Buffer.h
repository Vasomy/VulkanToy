#pragma once

#include"vulkan/vulkan.hpp"
namespace vkContext
{
	class Buffer final
	{
	public:	
		void* map = nullptr;
		vk::Buffer buffer;// in Cpu

		vk::DeviceMemory memory;
		size_t size;

		Buffer(size_t size,vk::BufferUsageFlags usage,vk::MemoryPropertyFlags property);
		~Buffer();
	private:
		struct MemoryInfo final
		{
			size_t size;
			uint32_t index;
		};
		void CreateBuffer(size_t,vk::BufferUsageFlags);
		void AllocateMemory(MemoryInfo info);
		void BindingMemoryToBuffer();
		MemoryInfo QueryMemoryInfo(vk::MemoryPropertyFlags);
	};
	std::uint32_t QueryBufferMemTypeIndex(std::uint32_t type, vk::MemoryPropertyFlags flag);
}