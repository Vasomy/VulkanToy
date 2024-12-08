#include"Buffer.h"
#include"Context.h"
namespace vkContext
{
	void Buffer::CreateBuffer(size_t size,vk::BufferUsageFlags usage)
	{
		vk::BufferCreateInfo createInfo;
		createInfo
			.setSize(size)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive)
			;
		buffer = Context::GetInstance().device.createBuffer(createInfo);
	}
	void Buffer::AllocateMemory(MemoryInfo info)
	{
		vk::MemoryAllocateInfo allocInfo;
		allocInfo
			.setAllocationSize(info.size)
			.setMemoryTypeIndex(info.index)
			;
		memory = Context::GetInstance().device.allocateMemory(allocInfo);
	}
	void Buffer::BindingMemoryToBuffer()
	{
		Context::GetInstance().device.bindBufferMemory(buffer, memory, 0);
	}
	Buffer::Buffer(size_t size,vk::BufferUsageFlags usage, vk::MemoryPropertyFlags property)
		:size(size)
	{
		CreateBuffer(size, usage);
		auto info = QueryMemoryInfo(property);
		AllocateMemory(info);
		BindingMemoryToBuffer();
	}

	Buffer::MemoryInfo Buffer::QueryMemoryInfo(vk::MemoryPropertyFlags property)
	{
		MemoryInfo info;
		auto requirements = Context::GetInstance().device.getBufferMemoryRequirements(buffer);
		info.size = requirements.size;

		auto properties = Context::GetInstance().phyDevice.getMemoryProperties();
		for (int i = 0; i < properties.memoryTypeCount; ++i)
		{
			if (((1 << i) & requirements.memoryTypeBits) &&
				(properties.memoryTypes[i].propertyFlags & property))
			{
				info.index = i;
				break;
			}
		}
		return info;
	}



	Buffer::~Buffer()
	{
		auto& context = Context::GetInstance();
		context.device.destroyBuffer(buffer);
		context.device.freeMemory(memory);
	}
}