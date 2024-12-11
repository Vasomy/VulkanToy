#include"GameObject.hpp"
namespace JRender
{
	JGameObject::JGameObject(const std::string& name)
	{
		this->name = name;
	}
	void JGameObject::LoadTexture(const char* filename)
	{
		texture.reset();
		texture.reset(
			new vkContext::Texture(filename)
		);
	}
}