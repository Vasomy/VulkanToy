#include"Shader.h"
#include"Context.h"
namespace vkContext
{
	Shader::Shader(const std::string&vertexSource,const std::string&fragSource)
	{
		vk::ShaderModuleCreateInfo createInfo;
		createInfo.codeSize = vertexSource.size();
		createInfo.pCode = (uint32_t*)vertexSource.data();
		
		vertexModule = Context::GetInstance().device.createShaderModule(createInfo);

		createInfo.codeSize = fragSource.size();
		createInfo.pCode = (uint32_t*)fragSource.data();
		fragmentModule = Context::GetInstance().device.createShaderModule(createInfo);

		InitStage();
		InitDescriptorSetLayouts();
	}

	std::vector<vk::PipelineShaderStageCreateInfo> Shader::GetStage()
	{
		return stage_;
	}

	void Shader::InitStage()
	{
		stage_.resize(2);
	
		stage_[0]
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(this->vertexModule)
			.setPName("main")
			;
		stage_[1]
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(this->fragmentModule)
			.setPName("main")
			;
	}

	void Shader::InitDescriptorSetLayouts()
	{
		vk::DescriptorSetLayoutCreateInfo createInfo;
		std::vector<vk::DescriptorSetLayoutBinding> bindings(2);
		bindings[0]
			.setBinding(0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setStageFlags(vk::ShaderStageFlagBits::eVertex|vk::ShaderStageFlagBits::eFragment)
			;
		bindings[1]
			.setBinding(1)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setStageFlags(vk::ShaderStageFlagBits::eFragment)
			;
		createInfo
			.setBindings(bindings)
			;
		layouts.push_back(Context::GetInstance().device.createDescriptorSetLayout(createInfo));
	}

	vk::PushConstantRange Shader::GetPushConstantsRange()
	{
		vk::PushConstantRange range;
		range
			.setOffset(0)
			.setSize(sizeof(glm::mat4))
			.setStageFlags(vk::ShaderStageFlagBits::eVertex)
			;
		return range;

	}

	Shader::~Shader()
	{
		auto& device = Context::GetInstance().device;
		device.destroyShaderModule(vertexModule);
		device.destroyShaderModule(fragmentModule);
	}
}