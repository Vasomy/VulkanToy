#include"CommandManager.hpp"
#include"Context.h"
vkContext::CommandManager::CommandManager()
{
	CreateCommandPool();
}

vkContext::CommandManager::~CommandManager()
{
	Context::GetInstance().device.destroyCommandPool(cmdPool);
}

void vkContext::CommandManager::ResetCommandPool()
{
	auto& context = Context::GetInstance();
	context.device.resetCommandPool(cmdPool);
}

void vkContext::CommandManager::FreeCommandBuffer(vk::CommandBuffer&buffer)
{
	auto& context = Context::GetInstance();
	context.device.freeCommandBuffers(cmdPool, buffer);
}

std::vector<vk::CommandBuffer> vkContext::CommandManager::CreateCommandBuffer(uint32_t count)
{
	auto& context = Context::GetInstance();

	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(cmdPool)
		.setCommandBufferCount(1)
		.setLevel(vk::CommandBufferLevel::ePrimary);

	return context.device.allocateCommandBuffers(allocInfo);

}

vk::CommandBuffer vkContext::CommandManager::CreateOneCommandBuffer()
{
	return CreateCommandBuffer(1)[0];
}

vk::CommandBuffer vkContext::CommandManager::BeginSingleTimeCommands()
{
	auto cmdBuffer = CreateOneCommandBuffer();

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	cmdBuffer.begin(&beginInfo);
	return std::move(cmdBuffer);
}

void vkContext::CommandManager::EndSingleTimeCommands(vk::CommandBuffer cmdBuffer)
{
	auto device = Context::GetInstance().device;
	auto graphicsQueue = Context::GetInstance().graphicsQueue;
	
	cmdBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.sType = vk::StructureType::eSubmitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdBuffer;

	graphicsQueue.submit(submitInfo);
	graphicsQueue.waitIdle();

	device.freeCommandBuffers(cmdPool, cmdBuffer);
}

void vkContext::CommandManager::ExecuteCommand(vk::Queue& queue, RecordCmdFunc func)
{
	auto& context = Context::GetInstance();
	auto cmdBuffer = CreateOneCommandBuffer();

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
		;
	cmdBuffer.begin(beginInfo);
	if (func)
	{
		func(cmdBuffer);
	}
	cmdBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo
		.setCommandBuffers(cmdBuffer)
		;
	queue
		.submit(submitInfo)
		;
	queue.waitIdle();
	context.device.waitIdle();
	FreeCommandBuffer(cmdBuffer);
}

void vkContext::CommandManager::CreateCommandPool()
{
	auto&context = Context::GetInstance();
	vk::CommandPoolCreateInfo createInfo;

	createInfo
		.setQueueFamilyIndex(context.queueFamilyIndices.graphicsQueue.value())
		.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
		;
	cmdPool = context.device.createCommandPool(createInfo);
}
