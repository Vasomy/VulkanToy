#include"RawMesh.hpp"
#include"../Context.h"
namespace JRender
{


	RawMesh::RawMesh(int inVertexCount,int inIndexCount,
		const Vertex* verticesData, const Index* indicesData)
		:vertexCount(inVertexCount),indexCount(inIndexCount)
	{
		auto& cmdPool = vkContext::Context::GetInstance().renderer->cmdPool;
		vertexBuffer.reset(new vkContext::UploadBuffer (
			sizeof(Vertex)*inVertexCount,
			vkContext::UBT_VertexBuffer
		));
		indexBuffer.reset(new vkContext::UploadBuffer(
			sizeof(Index)*inIndexCount,
			vkContext::UBT_IndexBuffer
		));
		
		vertexBuffer->SetData(verticesData);
		indexBuffer->SetData(indicesData);
	}

	RawMesh::~RawMesh()
	{
		vertexBuffer.reset();
		indexBuffer.reset();
	}

	glm::mat4 RawMesh::GetModel()
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		return model;
	}

}