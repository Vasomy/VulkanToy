#include"Renderer.h"
#include"Context.h"
namespace vkContext
{
	void DebugMat4(glm::mat4&mat)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				std::cout << mat[i][j] << ' ';
			}
			std::cout << '\n';
		}
	}
	void DebugVec4(glm::vec4&vec)
	{
		for (int i = 0; i < 4; ++i)
		{
			std::cout << vec[i] << ' ';
		}
		std::cout << '\n';
	}

	const std::array<Vertex, 3>vertices = {
		Vertex{0.0f,-0.5f,0},
		Vertex{0.5f,0.5f,0},
		Vertex{-0.5,0.5f,0}
	};

	JRender::ObjectConstants objectConstants
		=
	{
		glm::mat4(1.0f),
		glm::mat4(1.0f)
	};

	const std::array<Uniform, 1> uniform = {
		Uniform{{1,0,1}}
	};
	const std::array<Uniform, 1> uniform2 = {
		Uniform{{1,1,1}}
	};

	const std::array<uint32_t, 3> indices =
	{
		0,1,2
	};


	Renderer::Renderer()
	{
		InitCommandPool();
		AllocCommandBuffer();
		CreateFence();
		CreateSems();
		CreateVertexBuffer();
		BufferVertexData();
		CreateIndexBuffer();
		BufferIndexData();

		CreateUniformBuffer();
		BufferUniformData();
	
		CreateDescriptorPool();
		AllocateSets();

		UpdateSets();

		scene = new JRender::Scene;
		scene->Init();
		scene->CreateRectMesh();
	}

	void Renderer::Render()
	{
		static bool ishoold = false;
		auto& device = Context::GetInstance().device;
		auto& context = Context::GetInstance();
		auto* window = Context::GetInstance().m_window;

		objectConstants.view = scene->d_view;
		objectConstants.proj = scene->d_proj;
		auto model = scene->mesh->GetModel();
		uniformBuffer->SetData(&objectConstants);

		auto* mesh = scene->mesh;
		
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			mesh->position.x += 0.01f;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			mesh->position.x -= 0.01f;
		}
		
		//std::cout << "renderer\n";

		if (context.device.waitForFences(cmdFence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
		{
			std::cout << "Wait for fence failed!\n";
		}

		context.device.resetFences(cmdFence);

		auto result = device.acquireNextImageKHR(
			context.swapchain->swapchain,
			std::numeric_limits<uint64_t>::max(),
			imageDrawAvaliable
		);
		
		if (result.result != vk::Result::eSuccess)
		{
			std::cout << "acquire next image fail!\n";
		}

		auto imageIndex = result.value;
		cmdBuffer.reset();
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit)
			;
		cmdBuffer.begin(beginInfo);
		{
			cmdBuffer.bindPipeline(
				vk::PipelineBindPoint::eGraphics,
				context.renderProcess->pipeline
			);
			vk::RenderPassBeginInfo renderPassBeginInfo;
			vk::Rect2D rect;
			vk::ClearValue clearValue;
			vk::ClearColorValue clearColorValue(0.1f,0.1f,0.1f,1.0f);
			clearValue
				.setColor(clearColorValue)
				;
			rect
				.setOffset({ 0,0 })
				.setExtent(context.swapchain->info.imageExtent)
				;
			renderPassBeginInfo
				.setRenderPass(context.renderProcess->renderPass)
				.setRenderArea(rect)
				.setFramebuffer(context.swapchain->framebuffers[imageIndex])
				.setClearValues(clearValue)
				;
			vk::DeviceSize dSize = 0;
			//cmdBuffer.bindVertexBuffers(0, vertexBuffer->deviceBuffer->buffer,dSize);
			//cmdBuffer.bindIndexBuffer(indexBuffer->deviceBuffer->buffer, 0, vk::IndexType::eUint32);
			auto layout = context.renderProcess->layout;
			cmdBuffer.bindVertexBuffers(0, scene->mesh->GetVertexBuffer(), dSize);
			cmdBuffer.bindIndexBuffer(scene->mesh->GetIndexBuffer(), dSize,vk::IndexType::eUint32);
			cmdBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4),&model);
			cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, context.renderProcess->layout, 0, sets[0], {});
			cmdBuffer.beginRenderPass(renderPassBeginInfo, {});
			{
				cmdBuffer.drawIndexed(6, 1, 0, 0, 0);
			}
			cmdBuffer.endRenderPass();
		}
		cmdBuffer.end();

		vk::SubmitInfo submitInfo;
		std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo
			.setCommandBuffers(cmdBuffer)
			.setWaitSemaphores(imageDrawAvaliable)
			.setSignalSemaphores(imageDrawFinish)
			.setWaitDstStageMask(waitStages)
			;
		context.graphicsQueue.submit(submitInfo,cmdFence);

		vk::PresentInfoKHR presentInfoKHR;
		presentInfoKHR
			.setImageIndices(imageIndex)
			.setSwapchains(context.swapchain->swapchain)
			.setWaitSemaphores(imageDrawFinish)
			;
		if (context.presentQueue.presentKHR(presentInfoKHR) !=vk::Result::eSuccess)
		{
			std::cout << "Image present failed!\n";
		}


	}

	void Renderer::InitCommandPool()
	{
		/*vk::CommandPoolCreateInfo createInfo;
		createInfo
			.setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
			;
		cmdPool = Context::GetInstance().device.createCommandPool(createInfo);
		*/
		cmdPool = Context::GetInstance().cmdPool;
	}

	void Renderer::AllocCommandBuffer()
	{
		vk::CommandBufferAllocateInfo allocInfo;
		allocInfo
			.setCommandPool(cmdPool)
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary)// 可以直接提交给GPU执行
			;
		cmdBuffer = Context::GetInstance().device.allocateCommandBuffers(allocInfo)[0];
	}

	void Renderer::CreateFence()
	{
		vk::FenceCreateInfo createInfo;
		createInfo
			.setFlags(vk::FenceCreateFlagBits::eSignaled)
			;
		cmdFence = Context::GetInstance().device.createFence(createInfo);
	}

	void Renderer::CreateSems()
	{
		auto& device = Context::GetInstance().device;
		vk::SemaphoreCreateInfo createInfo;
		imageDrawAvaliable = device.createSemaphore(createInfo);
		imageDrawFinish = device.createSemaphore(createInfo);
	}


	void Renderer::CreateVertexBuffer()
	{
		vertexBuffer.reset(new UploadBuffer(sizeof(vertices), UBT_VertexBuffer));
		return;
		//hostVertexBuffer.reset(new Buffer(
		//	sizeof(vertices),
		//	vk::BufferUsageFlagBits::eTransferSrc,
		//	vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
		//));
		//deviceVertexBuffer.reset(new Buffer(
		//	sizeof(vertices),
		//	vk::BufferUsageFlagBits::eTransferDst|vk::BufferUsageFlagBits::eVertexBuffer,
		//	vk::MemoryPropertyFlagBits::eDeviceLocal
		//));
	}
	void Renderer::BufferVertexData()
	{
		vertexBuffer->SetData(vertices.data(), sizeof(vertices), 0);
		return;
		//auto& context = Context::GetInstance();
		//void* ptr = context.device.mapMemory(hostVertexBuffer->memory
		//	, 0, hostVertexBuffer->size);
		//memcpy(ptr, vertices.data(), sizeof(vertices));
		//context.device.unmapMemory(hostVertexBuffer->memory);
		//// create one commandBuffer
		//vk::CommandBufferAllocateInfo allocInfo;
		//allocInfo
		//	.setCommandPool(cmdPool)
		//	.setCommandBufferCount(1)
		//	.setLevel(vk::CommandBufferLevel::ePrimary)
		//	;
		//auto cmdBuf = context.device.allocateCommandBuffers(allocInfo)[0];

		//vk::CommandBufferBeginInfo beginInfo;
		//cmdBuf.begin(beginInfo);
		//vk::BufferCopy region;
		//region
		//	.setSize(hostVertexBuffer->size)
		//	.setSrcOffset(0)
		//	.setDstOffset(0)
		//	;
		//cmdBuf.copyBuffer(hostVertexBuffer->buffer, deviceVertexBuffer->buffer, region);
		//cmdBuf.end();

		//vk::SubmitInfo submit;
		//submit
		//	.setCommandBuffers(cmdBuf)
		//	;
		//context.graphicsQueue.submit(submit);

		//context.device.waitIdle();

		//context.device.freeCommandBuffers(cmdPool,cmdBuf);
	}

	void Renderer::CreateIndexBuffer()
	{
		indexBuffer.reset(new UploadBuffer(sizeof(indices), UBT_IndexBuffer));
	}

	void Renderer::BufferIndexData()
	{
		indexBuffer->SetData(indices.data(), sizeof(indices), 0);
	}


	void Renderer::CreateUniformBuffer()
	{
		size_t _3matSize = sizeof(JRender::ObjectConstants);
		uniformBuffer.reset(new UploadBuffer(_3matSize, UBT_UniformBuffer));
		uniformBuffer2.reset(new UploadBuffer(sizeof(uniform2), UBT_UniformBuffer));
		return;
		/*auto& context = Context::GetInstance();
		auto& device = Context::GetInstance().device;

		hostUniformBuffer.resize(1);
		deviceUniformBuffer.resize(1);
		for (auto& buffer : hostUniformBuffer)
		{
			buffer.reset(new Buffer(
				sizeof(uniform),
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostCoherent|vk::MemoryPropertyFlagBits::eHostVisible
			));
		}
		for (auto& buffer : deviceUniformBuffer)
		{
			buffer.reset(new Buffer(
				sizeof(uniform),
				vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eUniformBuffer,
				vk::MemoryPropertyFlagBits::eDeviceLocal
			));
		}*/
		
	}

	void Renderer::BufferUniformData()
	{
		uniformBuffer->SetData(objectConstants.data(), sizeof(objectConstants), 0);
		uniformBuffer2->SetData(uniform2.data(), sizeof(uniform2), 0);
		return;
		/*auto& context = Context::GetInstance();
		auto& device = Context::GetInstance().device;
		for (int i =0;i<hostUniformBuffer.size();++i)
		{
			auto& buffer = hostUniformBuffer[i];
			void* ptr = device.mapMemory(buffer->memory, 0, buffer->size);
			memcpy(ptr, uniform.data(), sizeof(uniform));
			device.unmapMemory(buffer->memory);
		}
		
		vk::CommandBufferAllocateInfo allocInfo;
		allocInfo
			.setCommandPool(cmdPool)
			.setCommandBufferCount(1)
			.setLevel(vk::CommandBufferLevel::ePrimary)
			;
		auto cmdBuf = device.allocateCommandBuffers(allocInfo)[0];
		vk::CommandBufferBeginInfo beginInfo;
		cmdBuf.begin(beginInfo);

		assert(hostUniformBuffer.size() == deviceUniformBuffer.size());
		vk::BufferCopy region;

		region
			.setSize(hostUniformBuffer[0]->size)
			.setDstOffset(0)
			.setSrcOffset(0)
			;
		for (int i = 0; i < hostUniformBuffer.size(); ++i)
		{
			std::cout << region.size << '\n';
			cmdBuf.copyBuffer(hostUniformBuffer[i]->buffer, deviceUniformBuffer[i]->buffer, region);
		}
		cmdBuf.end();

		vk::SubmitInfo submit;
		submit
			.setCommandBuffers(cmdBuf)
			;
		context.graphicsQueue.submit(submit);

		context.device.waitIdle();

		context.device.freeCommandBuffers(cmdPool, cmdBuf);*/

	}

	void Renderer::CreateDescriptorPool() 
	{
		auto& device = Context::GetInstance().device;
		vk::DescriptorPoolCreateInfo createInfo;
		vk::DescriptorPoolSize poolSize;
		poolSize
			.setType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(maxFlightCount)
			;
		createInfo
			.setMaxSets(maxFlightCount)
			.setPoolSizes(poolSize)
			;
		descPool = device.createDescriptorPool(createInfo);
	}

	void Renderer::AllocateSets()
	{
		auto& context = Context::GetInstance();
		std::vector<vk::DescriptorSetLayout>layouts(maxFlightCount, context.shader->GetLayouts()[0]);
		vk::DescriptorSetAllocateInfo allocInfo;
		allocInfo
			.setDescriptorPool(descPool)
			.setDescriptorSetCount(maxFlightCount)
			.setSetLayouts(layouts)
			;
		sets = context.device.allocateDescriptorSets(allocInfo);
	}

	void Renderer::UpdateSets()
	{
		auto& context = Context::GetInstance();
		for (int i =0;i<sets.size();++i)
		{
			auto& set = sets[i];
			std::vector<vk::WriteDescriptorSet> writer(2);
			vk::DescriptorBufferInfo bufferInfo1;
			bufferInfo1
				.setBuffer(uniformBuffer->deviceBuffer->buffer)
				.setOffset(0)
				.setRange(uniformBuffer->deviceBuffer->size)
				;
			writer[0]
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setBufferInfo(bufferInfo1)
				.setDstBinding(0)
				.setDstSet(set)
				.setDstArrayElement(0)
				.setDescriptorCount(1)
				;
			vk::DescriptorBufferInfo bufferInfo2;
			bufferInfo2
				.setBuffer(uniformBuffer2->deviceBuffer->buffer)
				.setOffset(0)
				.setRange(uniformBuffer2->deviceBuffer->size)
				;
			writer[1]
				.setDescriptorType(vk::DescriptorType::eUniformBuffer)
				.setBufferInfo(bufferInfo2)
				.setDstBinding(1)
				.setDstSet(set)
				.setDstArrayElement(0)
				.setDescriptorCount(1)
				;
			context.device.updateDescriptorSets(writer, {});
		}
	}

	Renderer::~Renderer()
	{
		uniformBuffer.reset();
		vertexBuffer.reset();
		/*hostVertexBuffer.reset();
		deviceVertexBuffer.reset();
		for (auto& buffer : hostUniformBuffer)
		{
			buffer.reset();
		}
		for (auto& buffer : deviceUniformBuffer)
		{
			buffer.reset();
		}*/
		auto& device = Context::GetInstance().device;
		//device.freeDescriptorSets(descPool,sets); // descSets会在销毁descPool再销毁一次
		device.destroyDescriptorPool(descPool);
		device.destroySemaphore(imageDrawAvaliable);
		device.destroySemaphore(imageDrawFinish);
		device.freeCommandBuffers(cmdPool, cmdBuffer);
		device.destroyCommandPool(cmdPool);
		device.destroyFence(cmdFence);
	}
}