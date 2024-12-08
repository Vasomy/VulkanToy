#pragma once
#include"vulkan/vulkan.hpp"
using Vertex = vkDetail::Vertex;
using Index = uint32_t;

namespace vkDetail
{
	class Vertex final
	{
	public:
		float x, y, z;

		static vk::VertexInputAttributeDescription GetAttribute()
		{
			vk::VertexInputAttributeDescription attr;
			attr
				.setBinding(0)
				.setFormat(vk::Format::eR32G32Sfloat)
				.setLocation(0)
				.setOffset(0)
				;
			return attr;
		}
		static vk::VertexInputBindingDescription GetBinding()

		{
			vk::VertexInputBindingDescription binding;
			binding
				.setBinding(0)
				.setInputRate(vk::VertexInputRate::eVertex)
				.setStride(sizeof(Vertex))
				;
			return binding;
		}
	private:
	};

}