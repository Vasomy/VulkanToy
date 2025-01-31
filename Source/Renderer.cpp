#include"Renderer.h"
#include"Context.h"
#include"QEM.h"
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
		

		CreateUniformBuffer();
		BufferUniformData();
	
		CreateDescriptorPool();
		AllocateSets();
		CreateSampler();

		UpdateSets();

		CreateViewportFramebuffer();

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
	
		auto dt = context.deltaTime;
		scene->Tick(dt);



		objectConstants.view = scene->d_view;
		objectConstants.proj = scene->d_proj;

		uniformBuffer->SetData(&objectConstants);
		

	
		//std::cout << "renderer\n";

		if (context.device.waitForFences(cmdFences[currentFrameIndex], true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
		{
			std::cout << "Wait for fence failed!\n";
		}

		context.device.resetFences(cmdFences[currentFrameIndex]);
		auto result = device.acquireNextImageKHR(
			context.swapchain->swapchain,
			std::numeric_limits<uint64_t>::max(),
			imageDrawAvaliable[currentFrameIndex]
		);

		if (result.result != vk::Result::eSuccess)
		{
			std::cout << "acquire next image fail!\n";
		}
	
		

		auto cmdBuffer = cmdBuffers[currentFrameIndex];

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
				//.setExtent(context.swapchain->info.imageExtent)
				.setExtent(viewportFramebuffers[currentFrameIndex]->GetExtent())
				;
			renderPassBeginInfo
				.setRenderPass(context.renderProcess->renderPass)
				.setRenderArea(rect)
				//.setFramebuffer(context.swapchain->framebuffers[currentFrameIndex])
				.setFramebuffer(viewportFramebuffers[currentFrameIndex]->GetFramebuffer())
				.setClearValues(clearValue)
				;
			//cmdBuffer.bindVertexBuffers(0, vertexBuffer->deviceBuffer->buffer,dSize);
			//cmdBuffer.bindIndexBuffer(indexBuffer->deviceBuffer->buffer, 0, vk::IndexType::eUint32);
			auto layout = context.renderProcess->layout;
			vk::DeviceSize dSize = 0;
			cmdBuffer.beginRenderPass(renderPassBeginInfo, {});
			for (int goIndex = 0; goIndex < scene->gameObjects.size(); ++goIndex) {
				auto& go = scene->gameObjects[goIndex];
				for (int i = 0; i < go->meshes.size(); ++i)
				{
					auto& mesh = go->meshes[i];

					cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, { sets[0].set,go->texture->set.set }, {});


					cmdBuffer.bindVertexBuffers(0, mesh->GetVertexBuffer(), dSize);
					cmdBuffer.bindIndexBuffer(mesh->GetIndexBuffer(), dSize, vk::IndexType::eUint32);

					cmdBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4), &mesh->GetModel());
					cmdBuffer.drawIndexed(mesh->GetIndexCount(), 1, 0, 0, 0);
				}
			}
			cmdBuffer.endRenderPass();
			/*cmdBuffer.bindVertexBuffers(0, scene->mesh->GetVertexBuffer(), dSize);
			cmdBuffer.bindIndexBuffer(scene->mesh->GetIndexBuffer(), dSize,vk::IndexType::eUint32);
			cmdBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4),&model);
			cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, context.renderProcess->layout, 0, sets[0], {});
			cmdBuffer.beginRenderPass(renderPassBeginInfo, {});
			{
				cmdBuffer.drawIndexed(6, 1, 0, 0, 0);
			}*/
			//cmdBuffer.endRenderPass();
		}
		cmdBuffer.end();

		vk::SubmitInfo submitInfo;
		std::array<vk::PipelineStageFlags, 1> waitStages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
		submitInfo
			.setCommandBuffers(cmdBuffer)
			.setWaitSemaphores( imageDrawAvaliable[currentFrameIndex] )
			.setSignalSemaphores( imageDrawFinish[currentFrameIndex] )
			.setWaitDstStageMask(waitStages)
			;
		context.graphicsQueue.submit(submitInfo,cmdFences[currentFrameIndex]);

		//device.waitIdle();

		vk::PresentInfoKHR presentInfoKHR;
		presentInfoKHR
			.setImageIndices(currentFrameIndex)
			.setSwapchains(context.swapchain->swapchain)
			.setWaitSemaphores(imageDrawFinish[currentFrameIndex])
			;
		if (context.presentQueue.presentKHR(presentInfoKHR) != vk::Result::eSuccess)
		{
			std::cout << "Image present failed!\n";
		}

		currentFrameIndex = currentFrameIndex + 1;
		currentFrameIndex %= 2;
	}

	void Renderer::PresentFrame()
	{
		
	}

	void Renderer::InitCommandPool()
	{
	}

	void Renderer::AllocCommandBuffer()
	{
		cmdBuffers.resize(2);
		for (int i = 0; i < 2; ++i) {
			cmdBuffers[i] = Context::GetInstance().commandManager->CreateOneCommandBuffer();
		}
	}

	void Renderer::CreateFence()
	{
		vk::FenceCreateInfo createInfo;
		createInfo
			.setFlags(vk::FenceCreateFlagBits::eSignaled)
			;
		cmdFences.resize(2);
		for (int i = 0; i < cmdFences.size(); ++i) 
		{
			cmdFences[i] = Context::GetInstance().device.createFence(createInfo);
		}
	}

	void Renderer::CreateSems()
	{
		imageDrawAvaliable.resize(2);
		imageDrawFinish.resize(2);
		auto& device = Context::GetInstance().device;
		vk::SemaphoreCreateInfo createInfo;
		for (int i = 0; i < 2; ++i) {
			imageDrawAvaliable[i] = device.createSemaphore(createInfo);
			imageDrawFinish[i] = device.createSemaphore(createInfo);
		}
	}




	void Renderer::CreateViewportFramebuffer()
	{
		viewportFramebuffers.resize(2);
		auto& context = Context::GetInstance();
		for (int i = 0; i < viewportFramebuffers.size(); ++i)
		{
			auto& framebuffer = viewportFramebuffers[i];
			framebuffer.reset(new Framebuffers(1920, 1080, context.renderProcess->renderPass));
		}
	}

	void Renderer::CreateUniformBuffer()
	{
		size_t _3matSize = sizeof(JRender::ObjectConstants);
		uniformBuffer.reset(new UploadBuffer(_3matSize, UBT_UniformBuffer));
		uniformBuffer2.reset(new UploadBuffer(sizeof(uniform2), UBT_UniformBuffer));
		return;

		
	}

	void Renderer::BufferUniformData()
	{
		uniformBuffer->SetData(objectConstants.data(), sizeof(objectConstants), 0);
		uniformBuffer2->SetData(uniform2.data(), sizeof(uniform2), 0);
		return;
	}


	void Renderer::CreateDescriptorPool() 
	{
		DescriptorSetManager::Init(maxFlightCount);
		return;
		
	}

	void Renderer::AllocateSets()
	{
		sets = DescriptorSetManager::GetInstance().AllocBufferSets(maxFlightCount);
		return;
	}

	void Renderer::CreateSampler()
	{
		sampler = Context::GetInstance().sampler;
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
				.setDstSet(set.set)
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
				.setDstSet(set.set)
				.setDstArrayElement(0)
				.setDescriptorCount(1)
				;
			// bind image
			/*vk::DescriptorImageInfo imageInfo;
			imageInfo
				.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
				.setImageView(texture->view)
				.setSampler(sampler)
				;

			writer[2].setImageInfo(imageInfo)
				.setDstBinding(2)
				.setDstArrayElement(0)
				.setDescriptorCount(1)
				.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
				.setDstSet(set);*/
			context.device.updateDescriptorSets(writer, {});
		}

	}

	Renderer::~Renderer()
	{
		delete scene;
		texture.reset();
		uniformBuffer.reset();
		vertexBuffer.reset();
		auto& context = Context::GetInstance();
		auto& device = Context::GetInstance().device;
		//device.freeDescriptorSets(descPool,sets); // descSets会在销毁descPool再销毁一次
		device.destroyDescriptorPool(descPool);
		for (int i = 0; i < 2; ++i) {
			device.destroySemaphore(imageDrawAvaliable[i]);
			device.destroySemaphore(imageDrawFinish[i]);
			context.commandManager->FreeCommandBuffer(cmdBuffers[i]);
			device.destroyFence(cmdFences[i]);
		}
		//device.freeCommandBuffers(cmdPool, cmdBuffer);
	}
}