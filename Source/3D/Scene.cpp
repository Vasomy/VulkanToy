#include"Scene.hpp"
#include"../Context.h"
namespace JRender
{
	const glm::vec3 cameraPos = { 0,0,1 };
	const glm::vec3 cameraFront = { 0,0,-1 };
	const glm::vec3 worldUp = { 0,1,0 };
	const std::array<Vertex, 4> rectVertices =
	{
		Vertex{ -0.5,-0.5,0,0,0},
		Vertex{0.5,-0.5 ,0,0,1},
		Vertex{-0.5, 0.5,0,1,0},
		Vertex{0.5,0.5,0,1,1}
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


		camera = RenderCamera();
		d_view = camera.GetView();
		d_proj = camera.GetProj();

		go->LoadTexture("asset/jay.jpg");
	}

	void Scene::Tick(float dt)
	{
		auto* window = vkContext::Context::GetInstance().m_window;
		d_view = camera.GetView();
		d_proj = camera.GetProj();
		dt = 0.01f;
		if (glfwGetKey(window, GLFW_KEY_W))
			camera.MoveForward(dt);
		if (glfwGetKey(window, GLFW_KEY_S))
			camera.MoveBackward(dt);
		if (glfwGetKey(window, GLFW_KEY_A))
			camera.MoveLeft(dt);
		if (glfwGetKey(window, GLFW_KEY_D))
			camera.MoveRight(dt);
		if (glfwGetKey(window, GLFW_KEY_Q))
			camera.RotateCamera(-1.f, 0);
		if (glfwGetKey(window, GLFW_KEY_E))
			camera.RotateCamera(1.f, 0);
	}

	void Scene::UpdateSets()
	{
		auto& context = vkContext::Context::GetInstance();

	}


	void Scene::CreateRectMesh()
	{
		go = new JGameObject("AAAA");
		mesh = new RawMesh(4, 6, rectVertices.data(), rectIndices.data());
		
		for (int i = 0; i < 5; ++i)
		{
			std::shared_ptr<RawMesh> mesh;
			mesh.reset(new RawMesh(4, 6, rectVertices.data(), rectIndices.data()));
			mesh->position = { i,0,0 };
			go->meshes.push_back(mesh);
		}
	}
}