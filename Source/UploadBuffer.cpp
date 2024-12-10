#include"UploadBuffer.hpp"
#include"Context.h"
namespace vkContext
{

	UploadBuffer::UploadBuffer(size_t size,UploadBufferType type)
	{
		
		vk::BufferUsageFlagBits bufferType;
		switch (type)
		{
		case vkContext::UBT_VertexBuffer:
			bufferType = vk::BufferUsageFlagBits::eVertexBuffer;
			break;
		case vkContext::UBT_IndexBuffer:
			bufferType = vk::BufferUsageFlagBits::eIndexBuffer;
			break;
		case vkContext::UBT_UniformBuffer:
			bufferType = vk::BufferUsageFlagBits::eUniformBuffer;
			break;
		default:
			break;
		}
		auto& context = Context::GetInstance();
		hostBuffer.reset(new Buffer(
			size,
			vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		));
		deviceBuffer.reset(new Buffer(
			size,
			vk::BufferUsageFlagBits::eTransferDst | bufferType,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		));
		//map = context.device.mapMemory(hostBuffer->memory, 0, hostBuffer->size);
	}

	void UploadBuffer::SetData(const void* data, size_t size, int offset)
	{
		if (size == 0)
			size = deviceBuffer->size;

		auto& context = Context::GetInstance();
		
		memcpy(hostBuffer->map, data, size);
		// create one commandBuffer
		
		auto cmdBuf = context.commandManager->CreateOneCommandBuffer();
		context.commandManager->ExecuteCommand(context.graphicsQueue
			, [&](vk::CommandBuffer& cmdBuf)
			{
				vk::BufferCopy region;
				region
					.setSize(hostBuffer->size)
					.setSrcOffset(0)
					.setDstOffset(0)
					;
				cmdBuf.copyBuffer(hostBuffer->buffer, deviceBuffer->buffer, region);

			});
		return;
		vk::CommandBufferBeginInfo beginInfo;
		cmdBuf.begin(beginInfo);
		vk::BufferCopy region;
		region
			.setSize(hostBuffer->size)
			.setSrcOffset(0)
			.setDstOffset(0)
			;
		cmdBuf.copyBuffer(hostBuffer->buffer, deviceBuffer->buffer, region);

		cmdBuf.end();

		vk::SubmitInfo submit;
		submit
			.setCommandBuffers(cmdBuf)
			;
		context.graphicsQueue.submit(submit);

		context.device.waitIdle();

		//context.device.freeCommandBuffers(cp, cmdBuf);
	}

	UploadBuffer::~UploadBuffer()
	{
		auto& context = Context::GetInstance();
		context.device.unmapMemory(hostBuffer->memory);
		hostBuffer.reset();
		deviceBuffer.reset();
	}


}