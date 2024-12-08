#pragma once
#include"vulkan/vulkan.hpp"
#include"Buffer.h";
#include"Vertex.h"
#include"UploadBuffer.hpp"
#include"Uniform.h"
#include"3D/Scene.hpp"
namespace vkContext {
	class Renderer
	{
		JRender::Scene* scene = nullptr;
		static const int maxFlightCount = 1;
	public:
		Renderer();
		~Renderer();
		void Render();
		vk::CommandPool cmdPool;
	private:
		void InitCommandPool();
		void AllocCommandBuffer();
		void CreateFence(); 
		void CreateSems();
		vk::CommandBuffer cmdBuffer;

		vk::Fence cmdFence;

		vk::Semaphore imageDrawFinish;
		vk::Semaphore imageDrawAvaliable;

		std::unique_ptr<UploadBuffer> vertexBuffer;
		std::unique_ptr<UploadBuffer> indexBuffer;

		std::unique_ptr<UploadBuffer>uniformBuffer;
		std::unique_ptr<UploadBuffer>uniformBuffer2;

		vk::DescriptorPool descPool;
		std::vector<vk::DescriptorSet>sets;

		void CreateVertexBuffer();
		void BufferVertexData();
		void CreateIndexBuffer();
		void BufferIndexData();

		void CreateUniformBuffer();
		void BufferUniformData();

		void CreateDescriptorPool();
		void AllocateSets();

		void UpdateSets();
		
	};
}