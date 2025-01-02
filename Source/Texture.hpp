#pragma once
#include"Buffer.h"
#include"DescriptorManager.hpp"
namespace vkContext
{
	class Texture 
	{
	public:
		Texture();
		Texture(const char* filename);
		~Texture();

		vk::Image image;
		vk::DeviceMemory memory;
		vk::ImageView view;
		DescriptorSetManager::SetInfo set;
	private:
		void Init(void*data,size_t size,int w,int h);
		void CreateImage(uint32_t w, uint32_t h);
		void CreateImageView();
		void AllocMemory();
		uint32_t QueryImageMemoryIndex();
		void TransitionImageLayoutFromUndefinedToDst();
		void TransformDataToImage(Buffer&buffer,uint32_t w,uint32_t h);
		void TransitionImageLayoutFromDstToOptimal();
		void UpdateDescriptorSet();
	};
}