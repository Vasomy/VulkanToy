#pragma once

#include"Buffer.h"
namespace vkContext
{
	enum UploadBufferType
	{
		UBT_VertexBuffer,
		UBT_IndexBuffer,
		UBT_UniformBuffer,
	};
	class UploadBuffer
	{
	public:

		std::unique_ptr<vkContext::Buffer> hostBuffer;
		std::unique_ptr<vkContext::Buffer> deviceBuffer;

		UploadBuffer(size_t,UploadBufferType type);
		virtual ~UploadBuffer();

		void SetData(const void* data, size_t size = 0, int offset = 0);

		const void* GetData()const { return hostBuffer->map; }
		
	private:

	};

}