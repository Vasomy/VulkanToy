#pragma once
#include"Buffer.h"
namespace vkContext
{
	class Texture 
	{
	public:
		Texture(const char* filename);
		~Texture();

		vk::Image image;
		vk::DeviceMemory memory;
		vk::ImageView view;
	private:
		void CreateImage(uint32_t w, uint32_t h);
		void CreateImageView();
		void AllocMemory();
		uint32_t QueryImageMemoryIndex();
		void TransitionImageLayoutFromUndefinedToDst();
		void TransformDataToImage(Buffer&buffer,uint32_t w,uint32_t h);
		void TransitionImageLayoutFromDstToOptimal();
	};
}