#include"Texture.hpp"
#include"Tools/ImageLoader.hpp"
#include"Context.h"
namespace vkContext
{
	Texture::Texture()
	{
		uint8_t* data = new uint8_t[4];
		for (int i = 0; i < 4; ++i)
		{
			data[i] = 1.0f;
		}
		size_t size = 4;
		Init(data, size,1,1);

	}
	Texture::Texture(const char* filename)
	{
		int w, h, channel = 0;
		uint8_t* data = Tools::ImageLoader::ImageLoad(filename,&w,&h,&channel);

		size_t size = w * h * 4;
		if (!data)
		{
			throw std::runtime_error("Image load failed!");
		}
		Init(data, size,w,h);
		
	}

	Texture::~Texture()
	{
		auto& device = Context::GetInstance().device;
		DescriptorSetManager::GetInstance().FreeImageSet(set);
		device.destroyImageView(view);
		device.freeMemory(memory);
		device.destroyImage(image);
	}

	void Texture::Init(void* data, size_t size,int w,int h)
	{
		std::unique_ptr<Buffer>buffer(
			new Buffer(
				size,
				vk::BufferUsageFlagBits::eTransferSrc,
				vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
			)
		);
		memcpy(buffer->map, data, size);

		auto& context = Context::GetInstance();
		CreateImage(w, h);
		AllocMemory();
		context.device.bindImageMemory(image, memory, 0);

		TransitionImageLayoutFromUndefinedToDst();
		TransformDataToImage(*buffer, w, h);
		TransitionImageLayoutFromDstToOptimal();

		CreateImageView();

		free(data);

		set = DescriptorSetManager::GetInstance().AllocImageSet();
		UpdateDescriptorSet();
	}

	void Texture::CreateImage(uint32_t w, uint32_t h)
	{
		vk::ImageCreateInfo createInfo;
		createInfo.setImageType(vk::ImageType::e2D)
			.setArrayLayers(1)
			.setMipLevels(1)
			.setExtent({ w, h, 1 })
			.setFormat(vk::Format::eR8G8B8A8Srgb)
			.setTiling(vk::ImageTiling::eOptimal)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
			.setSamples(vk::SampleCountFlagBits::e1);
		image = Context::GetInstance().device.createImage(createInfo);
	}
	void Texture::CreateImageView()
	{
		vk::ImageViewCreateInfo createInfo;
		vk::ComponentMapping mapping;
		vk::ImageSubresourceRange range;
		range.setAspectMask(vk::ImageAspectFlagBits::eColor)
			.setBaseArrayLayer(0)
			.setLayerCount(1)
			.setLevelCount(1)
			.setBaseMipLevel(0);
		createInfo.setImage(image)
			.setViewType(vk::ImageViewType::e2D)
			.setComponents(mapping)
			.setFormat(vk::Format::eR8G8B8A8Srgb)
			.setSubresourceRange(range);
		view = Context::GetInstance().device.createImageView(createInfo);
	}
	void Texture::AllocMemory()
	{
		auto& device = Context::GetInstance().device;
		vk::MemoryAllocateInfo allocInfo;

		auto requirements = device.getImageMemoryRequirements(image);
		allocInfo.setAllocationSize(requirements.size);

		auto index = QueryBufferMemTypeIndex(requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
		allocInfo.setMemoryTypeIndex(index);

		memory = device.allocateMemory(allocInfo);
	}
	uint32_t Texture::QueryImageMemoryIndex()
	{
		return 0;
	}
	void Texture::TransitionImageLayoutFromUndefinedToDst()
	{
		auto& context = Context::GetInstance();
		context.commandManager->ExecuteCommand(
			context.graphicsQueue,
			[&](vk::CommandBuffer cmdBuffer)
			{
				vk::ImageMemoryBarrier barrier;
				vk::ImageSubresourceRange range;
				range.setLayerCount(1)
					.setBaseArrayLayer(0)
					.setLevelCount(1)
					.setBaseMipLevel(0)
					.setAspectMask(vk::ImageAspectFlagBits::eColor);
				barrier.setImage(image)
					.setOldLayout(vk::ImageLayout::eUndefined)
					.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
					.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
					.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
					.setDstAccessMask((vk::AccessFlagBits::eTransferWrite))
					.setSubresourceRange(range);
				cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer,
					{}, {}, nullptr, barrier);
			}
		);
	}
	void Texture::TransformDataToImage(Buffer&buffer, uint32_t w, uint32_t h)
	{
		auto& context = Context::GetInstance();
		context.commandManager->ExecuteCommand(context.graphicsQueue,
			[&](vk::CommandBuffer cmdBuffer) {
				vk::BufferImageCopy region;
				vk::ImageSubresourceLayers subsource;
				subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
					.setBaseArrayLayer(0)
					.setMipLevel(0)
					.setLayerCount(1);
				region.setBufferImageHeight(0)
					.setBufferOffset(0)
					.setImageOffset(0)
					.setImageExtent({ w, h, 1 })
					.setBufferRowLength(0)
					.setImageSubresource(subsource);
				cmdBuffer.copyBufferToImage(buffer.buffer, image,
					vk::ImageLayout::eTransferDstOptimal,
					region);
			});
	}
	void Texture::TransitionImageLayoutFromDstToOptimal()
	{
		auto& context = Context::GetInstance();
		context.commandManager->ExecuteCommand(context.graphicsQueue,
			[&](vk::CommandBuffer cmdBuffer) {
				vk::ImageMemoryBarrier barrier;
				vk::ImageSubresourceRange range;
				range.setLayerCount(1)
					.setBaseArrayLayer(0)
					.setLevelCount(1)
					.setBaseMipLevel(0)
					.setAspectMask(vk::ImageAspectFlagBits::eColor);
				barrier.setImage(image)
					.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
					.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
					.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
					.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
					.setSrcAccessMask((vk::AccessFlagBits::eTransferWrite))
					.setDstAccessMask((vk::AccessFlagBits::eShaderRead))
					.setSubresourceRange(range);
				cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader,
					{}, {}, nullptr, barrier);
			});
	}
	void Texture::UpdateDescriptorSet()
	{
		vk::WriteDescriptorSet writer;
		vk::DescriptorImageInfo imageInfo;
		imageInfo.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setImageView(view)
			.setSampler(Context::GetInstance().sampler);
		writer.setImageInfo(imageInfo)
			.setDstBinding(0)
			.setDstArrayElement(0)
			.setDstSet(set.set)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler);
		Context::GetInstance().device.updateDescriptorSets(writer, {});
	}
}