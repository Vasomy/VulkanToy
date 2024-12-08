#pragma once
#include"../UploadBuffer.hpp"
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/quaternion.hpp"
#include"../Vertex.h"
namespace JRender
{
	
	class RawMesh
	{
		
	public:
		RawMesh() = delete;
		RawMesh(int vertexCount,int indexCount,const Vertex* verticesData, const Index* indicesData);
		~RawMesh();
		glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f);


		vk::Buffer& GetVertexBuffer() { return vertexBuffer->deviceBuffer->buffer; }
		vk::Buffer& GetIndexBuffer() { return indexBuffer->deviceBuffer->buffer; }
		int GetVertexCount() { return vertexCount; }
		int GetIndexCount() { return indexCount; }
		glm::mat4 GetModel();
	private:
		std::shared_ptr<vkContext::UploadBuffer>vertexBuffer;
		std::shared_ptr<vkContext::UploadBuffer>indexBuffer;
		int vertexCount;
		int indexCount;
	};
	class cube
	{
	public:
		std::vector<RawMesh*>meshes;
	};
}