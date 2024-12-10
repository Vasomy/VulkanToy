#pragma once

inline static unsigned char* ImageLoad(const char* file_name,
	int* w,
	int* h,
	int* channel);
inline static float* ImageLoadf();

inline void FreeImageData(void*data);

namespace Tools {
	class ImageLoader
	{
	public:
		static unsigned char* ImageLoad(const char* file_name,int*w = nullptr,int*h = nullptr,int *channel = nullptr);
		static float* ImageLoadf(const char*file_name, int* w = nullptr, int* h = nullptr, int* channel = nullptr);
		
	};
}