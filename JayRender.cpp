// JayRender.cpp: 定义应用程序的入口点。
//

#include "JayRender.h"
#include"Source/Context.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
using namespace std;
const int screen_width = 1440, screen_height = 960;
int main(int argc,char**argv)
{

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Vulkan", nullptr, nullptr);
	

	uint32_t count = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);

	const char** extension = glfwGetRequiredInstanceExtensions(&count);
	std::vector<const char*>extensions(count);
	for (int i = 0; i < count; ++i)
	{
		extensions[i] = extension[i];
		std::cout << extensions[i] << '\n';
	}

	vkContext::Context::Init(extensions,window, screen_width,screen_height);
	auto& context = vkContext::Context::GetInstance();
	auto& renderer = vkContext::Context::GetRenderer();
	float lastTime = glfwGetTime();
	float deltaTime = 0;
	context.deltaTime = deltaTime;
	while (!glfwWindowShouldClose(window))
	{
		deltaTime = glfwGetTime()-lastTime;
		context.deltaTime = deltaTime;
		renderer.Render();
		vkContext::UiContext::GetInstance().UiRender();
		glfwPollEvents();
		lastTime = glfwGetTime();

	}
	vkContext::Context::Quit();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}