#include"RenderProcess.h"
#include"Context.h"
#include"Vertex.h"
namespace vkContext
{
	RenderProcess::RenderProcess()
	{
		auto& context = Context::GetInstance();
		InitRenderPass();
		//setLayout = CreateSetLayout();
		layout = InitLayout();
		InitPipeline(context.width,context.height);
	}

	void RenderProcess::InitPipeline(int width,int height)
	{
		vk::GraphicsPipelineCreateInfo createInfo;

		vk::PipelineVertexInputStateCreateInfo inputState;
		auto binding = Vertex::GetBinding();
		auto attribute = Vertex::GetAttribute();
		inputState
			.setVertexAttributeDescriptions(attribute)
			.setVertexBindingDescriptions(binding)
			;

		createInfo.setPVertexInputState(&inputState);

		vk::PipelineInputAssemblyStateCreateInfo inputAsm;
		inputAsm
			// line list 1->2 3->4 
			// linestrip 1->2->3->4
			// triList   1->2->3 4->5->6
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			;
		createInfo.setPInputAssemblyState(&inputAsm);

		//3 shader
		auto stages = Context::GetInstance().shader->GetStage();
		createInfo.setStages(stages);

		//4 viewPort
		vk::PipelineViewportStateCreateInfo viewportInfo;
		vk::Viewport viewport;
		viewport
			.setX(0)
			.setY(0)
			.setWidth(width)
			.setHeight(height)
			.setMinDepth(0)
			.setMaxDepth(1)
			;
		vk::Rect2D rect(
			{ 0,0 },
			{ static_cast<uint32_t>(width),static_cast<uint32_t>(height) }
		);

		viewportInfo
			.setViewports(viewport)
			.setScissors(rect)
			;
		createInfo.setPViewportState(&viewportInfo);
		//5 rasterization
		vk::PipelineRasterizationStateCreateInfo rastInfo;
		rastInfo
			.setRasterizerDiscardEnable(false)
			.setCullMode(vk::CullModeFlagBits::eNone)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setPolygonMode(vk::PolygonMode::eLine)
			.setLineWidth(1)
			;
		createInfo.setPRasterizationState(&rastInfo);

		// 6 muiltiple sample
		vk::PipelineMultisampleStateCreateInfo multisampleInfo;
		multisampleInfo
			.setSampleShadingEnable(false)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1)
			;
		createInfo.setPMultisampleState(&multisampleInfo);

		//7 test depth stencil
		vk::PipelineColorBlendAttachmentState blendAttachState;
		blendAttachState
			.setBlendEnable(true)
			.setColorWriteMask(
				vk::ColorComponentFlagBits::eA |
				vk::ColorComponentFlagBits::eR |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eB 
			)
			.setSrcColorBlendFactor(vk::BlendFactor::eOne)
			.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
			.setColorBlendOp(vk::BlendOp::eAdd)
			.setSrcAlphaBlendFactor(vk::BlendFactor::eOne)
			.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
			.setAlphaBlendOp(vk::BlendOp::eAdd)
			;
		vk::PipelineColorBlendStateCreateInfo blendInfo;
		blendInfo
			.setAttachments(blendAttachState)
			.setLogicOpEnable(false)
			;
		//createInfo.setPColorBlendState(&blendInfo);
		//

		//8 ColorBlending
		//vk::PipelineColorBlendStateCreateInfo blendInfo;
		vk::PipelineColorBlendAttachmentState attachs;
		attachs
			.setBlendEnable(false)
			.setColorWriteMask(vk::ColorComponentFlagBits::eA |
				vk::ColorComponentFlagBits::eB |
				vk::ColorComponentFlagBits::eG |
				vk::ColorComponentFlagBits::eR)
			;
		blendInfo
			.setLogicOpEnable(false)
			.setAttachments(attachs)
			;
		createInfo.setPColorBlendState(&blendInfo);

		//9 render pass and layout
		createInfo
			.setRenderPass(renderPass)
			.setLayout(layout)
			;


		auto result =Context::GetInstance().device.createGraphicsPipeline(nullptr, createInfo);
		if (result.result != vk::Result::eSuccess)
		{
			std::cout << "Failed to create graphics pipeline!\n";
		}
		pipeline = result.value;

	}

	void RenderProcess::InitRenderPass()
	{
		vk::RenderPassCreateInfo createInfo;
		vk::AttachmentDescription attachDesc;

		attachDesc
			.setFormat(Context::GetInstance().swapchain->info.format.format)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setSamples(vk::SampleCountFlagBits::e1)
			;
		createInfo.setAttachments(attachDesc);
		vk::AttachmentReference reference;
		reference
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal)
			.setAttachment(0) // 引用的 attachDesc中第一个附加件
			;

		vk::SubpassDescription subpass;
		subpass
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachments(reference)
			;
		createInfo.setSubpasses(subpass);
		
		vk::SubpassDependency subpassDependency;
		subpassDependency
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			// subpass 数组的下标 通reference的setAttachment
			.setDstSubpass(0)
			 // 对于现subpass的访问权限
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
			//设置 原subpass和现的作用场景
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			;
		createInfo.setDependencies(subpassDependency);

		renderPass = Context::GetInstance().device.createRenderPass(createInfo); 
	}

	vk::PipelineLayout RenderProcess::InitLayout()
	{
		vk::PipelineLayoutCreateInfo createInfo;
		auto range = Context::GetInstance().shader->GetPushConstantsRange();
		createInfo
			.setSetLayouts(Context::GetInstance().shader->GetLayouts())
			.setPushConstantRanges(range)
			;

		layout = Context::GetInstance().device.createPipelineLayout(createInfo);
		return layout;
	}

	vk::DescriptorSetLayout RenderProcess::CreateSetLayout()
	{
		vk::DescriptorSetLayoutCreateInfo createInfo;
		auto binding = Uniform::GetBinding();
		createInfo
			.setBindings(binding)
			;
		return Context::GetInstance().device.createDescriptorSetLayout(createInfo);
	}


	RenderProcess::~RenderProcess()
	{
		auto& device = Context::GetInstance().device;
		device.destroyRenderPass(renderPass);
		device.destroyPipelineLayout(layout);
		device.destroyDescriptorSetLayout(setLayout);
		device.destroyPipeline(pipeline);
	}

}