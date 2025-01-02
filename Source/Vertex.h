#pragma once
#include"vulkan/vulkan.hpp"
#include"glm/glm.hpp"
using Index = uint32_t;
struct vvv
{
	glm::vec3 v;
	glm::vec3 v2;
};

class Vertex final
{
public:
	union {
		struct { float x, y, z; };
		struct { glm::vec3 position; };
	};
	union {
		struct { float texU, texV; };
		struct { glm::vec2 texcoord; };
	};
	static std::vector<vk::VertexInputAttributeDescription> GetAttribute()
	{
		std::vector<vk::VertexInputAttributeDescription> attrs;
		attrs.push_back({});
		attrs.back()
			.setBinding(0)
			.setFormat(vk::Format::eR32G32B32Sfloat)
			.setLocation(0)
			.setOffset(0)
			;
		attrs.push_back({});
		attrs.back()
			.setBinding(0)
			.setFormat(vk::Format::eR32G32Sfloat)
			.setLocation(1)
			.setOffset(3*sizeof(float))
			;
		return attrs;
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

