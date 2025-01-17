#pragma once

#include"Context.h"
#include"UiContext.hpp"

namespace JayEngine
{
	using namespace vkContext;
	class Engine
	{
		Engine();
	public:
		static void Init();
		static void Quit();
		static Engine& GetInstance();
		
		~Engine();
		

		void Tick();
		void ShutDown();
	private:

		void RenderTick();
		void LogicTick();

		std::unique_ptr<Swapchain> mainSwapchain;// 管理软件从上下两帧的状态
		std::vector<vk::Fence> frameFences;
		std::vector<vk::Semaphore> frameFinish;
		std::vector<vk::Semaphore> frameAvailable;
		uint32_t currentFrameIndex;

		void CreateAppSwapchain();
		void CreateAppFrameSyncResource();

	private: 
		inline static std::unique_ptr<Engine> instance_ = nullptr;
	};

}
inline static uint32_t GetAppFrameIndex();
inline static uint32_t GetAppFrameIndexU32();
inline static uint64_t GetAppFrameIndexU64();
