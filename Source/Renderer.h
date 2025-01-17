#pragma once
#include"vulkan/vulkan.hpp"
#include"Buffer.h";
#include"Vertex.h"
#include"UploadBuffer.hpp"
#include"Uniform.h"
#include"3D/Scene.hpp"
#include"Texture.hpp"
#include"DescriptorManager.hpp"

#include"RHI/Framebuffers.hpp"
namespace vkContext {
	class Renderer
	{
		static const int maxFlightCount = 1;
	public:
		JRender::Scene* scene = nullptr;
		Renderer();
		~Renderer();
		void Render();
		void PresentFrame();
		vk::CommandPool cmdPool;
		vk::Sampler sampler;
		uint32_t currentFrameIndex;

		std::vector<std::unique_ptr<Framebuffers>> viewportFramebuffers;
	private:
		void InitCommandPool();
		void AllocCommandBuffer();
		void CreateFence(); 
		void CreateSems();
		std::vector<vk::CommandBuffer> cmdBuffers;
		std::vector<vk::Fence> cmdFences;
		std::vector<vk::Semaphore> imageDrawFinish;
		std::vector<vk::Semaphore> imageDrawAvaliable;


		std::unique_ptr<UploadBuffer> vertexBuffer;
		std::unique_ptr<UploadBuffer> indexBuffer;

		std::unique_ptr<UploadBuffer>uniformBuffer;
		std::unique_ptr<UploadBuffer>uniformBuffer2;

		std::unique_ptr<Texture> texture;

		vk::DescriptorPool descPool;
		std::vector<DescriptorSetManager::SetInfo>sets;
		std::vector<vk::DescriptorSet>imgSets;

		//std::vector<vk::Framebuffer> viewportFramebuffer;


		void CreateViewportFramebuffer();

		void CreateUniformBuffer();
		void BufferUniformData();

		void CreateDescriptorPool();
		void AllocateSets();
		void CreateSampler();


		void UpdateSets();

		
	};
}