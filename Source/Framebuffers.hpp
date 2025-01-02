#pragma once

namespace vkContext
{
	class Framebuffers
	{
	public:
		Framebuffers();
		~Framebuffers();
		Framebuffers(Framebuffers& rhs) = delete;

		void GetImages();
		void CreateFramebuffers();
		void CreateImageViews();
	};
}