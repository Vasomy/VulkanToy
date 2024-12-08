#include"Scene.hpp"
#include"../Context.h"
namespace JRender
{
	const glm::vec3 cameraPos = { 0,0,1 };
	const glm::vec3 cameraFront = { 0,0,-1 };
	const glm::vec3 worldUp = { 0,1,0 };
	const std::array<Vertex, 4> rectVertices =
	{
		Vertex{ -0.5,-0.5,0},
		Vertex{0.5,-0.5 ,0},
		Vertex{-0.5, 0.5,0},
		Vertex{0.5,0.5,0}
	};
	const std::array<uint32_t, 6>rectIndices =
	{
		0,1,2,
		1,2,3
	};
	Scene::Scene()
	{

	}

	Scene::~Scene()
	{
		delete mesh;
	}

	void Scene::Init()
	{
		auto& context = vkContext::Context::GetInstance();
		float aspect = static_cast<float>(context.width) / static_cast<float>(context.height);

		d_view = glm::lookAt(cameraPos, cameraFront, worldUp);
		d_proj = glm::perspective(45.0f, aspect, 0.1f, 100.0f);

		
	}

	void Scene::UpdateSets()
	{
		auto& context = vkContext::Context::GetInstance();

	}


	void Scene::CreateRectMesh()
	{

		mesh = new RawMesh(4, 6, rectVertices.data(), rectIndices.data());
	}
}