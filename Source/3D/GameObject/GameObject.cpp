#include"GameObject.hpp"
namespace JRender
{
	JGameObject::JGameObject(const std::string& name)
	{
		this->name = name;
	}
	JGameObject::~JGameObject()
	{
		texture.reset();
		for (auto& mesh : meshes)
		{
			mesh.reset();
		}
	}
	void JGameObject::LoadTexture(const char* filename)
	{
		if (filename == nullptr)
		{

		}
		else
		{
			texture.reset(
				new vkContext::Texture(filename)
			);
		}
	}
	void JGameObject::FreeAllMesh()
	{
		for(auto& i : meshes)
		{
			i.reset();
		}
		meshes.clear();
		
	}
}