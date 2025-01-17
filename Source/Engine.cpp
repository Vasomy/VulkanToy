#include"Engine.hpp"

using namespace JayEngine;

Engine::Engine()
{
}

Engine::~Engine()
{
	ShutDown();
}

void Engine::Init()
{
	instance_.reset(new Engine());
}

void Engine::Quit()
{
	instance_.reset();
}

Engine& Engine::GetInstance()
{
	return *instance_;
}

void Engine::RenderTick()
{

}

void Engine::LogicTick()
{

}

void Engine::Tick()
{

}

void Engine::ShutDown()
{

}

void Engine::CreateAppFrameSyncResource()
{
	frameFences.resize(2);
	frameAvailable.resize(2);
	frameFinish.resize(2);

	vk::FenceCreateInfo fenceInfo;
	fenceInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
	vk::SemaphoreCreateInfo semInfo;
	assert( ( frameFences.size() == frameFinish.size() == frameAvailable.size() ) );
	auto& device = Context::GetInstance().device;
	for (int i = 0; i < frameFences.size(); ++i)
	{
		frameAvailable[i] = device.createSemaphore(semInfo);
	}
}

void Engine::CreateAppSwapchain()
{
	int w = Context::GetInstance().width;
	int h = Context::GetInstance().height;
	mainSwapchain.reset(new Swapchain(w, h));
}