#include"ImageLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"
inline static unsigned char* ImageLoad(const char*file_name,
	int*w,
	int*h,
	int*channel
)
{
	return Tools::ImageLoader::ImageLoad(file_name, w, h, channel);
}


inline static float* ImageLoadf()
{

}

inline static void FreeImageData(void* data)
{
	stbi_image_free(data);
}

namespace Tools{
	unsigned char* ImageLoader::ImageLoad(const char*file_name,
		int*w,
		int*h,
		int*channel
	)
	{
		stbi_uc* data = stbi_load(file_name, w, h, channel, STBI_rgb_alpha);
		return data;
	}

	float* ImageLoader::ImageLoadf(const char*file_name,
		int*w,
		int*h,
		int*channel
	)
	{
		float* data = stbi_loadf(file_name, w, h, channel, STBI_rgb_alpha);
		return data;
	}
}