#include"DescriptorManager.hpp"
#include"Context.h"
using namespace vkContext;
vkContext::DescriptorSetManager::DescriptorSetManager(uint32_t maxFlight)
    :maxFlight(maxFlight)
{
    vk::DescriptorPoolSize size;
    size.setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(2 * maxFlight);
    vk::DescriptorPoolCreateInfo createInfo;
    createInfo.setMaxSets(maxFlight)
        .setPoolSizes(size);
    auto pool = Context::GetInstance().device.createDescriptorPool(createInfo);
    bufferSetPool.pool = pool;
    bufferSetPool.remainNum = maxFlight;
}

vkContext::DescriptorSetManager::~DescriptorSetManager()
{
    auto& device = Context::GetInstance().device;

    device.destroyDescriptorPool(bufferSetPool.pool);
    for (auto pool : fulledImageSetPool) {
        device.destroyDescriptorPool(pool.pool);
    }
    for (auto pool : avalibleImageSetPool) {
        device.destroyDescriptorPool(pool.pool);
    }
}

std::vector<DescriptorSetManager::SetInfo> vkContext::DescriptorSetManager::AllocBufferSets(uint32_t num)
{
    std::vector<vk::DescriptorSetLayout> layouts(maxFlight, Context::GetInstance().shader->GetLayouts()[0]);
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.setDescriptorPool(bufferSetPool.pool)
        .setDescriptorSetCount(num)
        .setSetLayouts(layouts);
    auto sets = Context::GetInstance().device.allocateDescriptorSets(allocInfo);

    std::vector<SetInfo> result(num);

    for (int i = 0; i < num; i++) {
        result[i].set = sets[i];
        result[i].pool = bufferSetPool.pool;
    }

    return result;
}

DescriptorSetManager::SetInfo vkContext::DescriptorSetManager::AllocImageSet()
{
    std::vector<vk::DescriptorSetLayout> layouts{ Context::GetInstance().shader->GetLayouts()[1] };
    vk::DescriptorSetAllocateInfo allocInfo;
    auto& poolInfo = GetAvaliableImagePoolInfo();
    allocInfo.setDescriptorPool(poolInfo.pool)
        .setDescriptorSetCount(1)
        .setSetLayouts(layouts);
    auto sets = Context::GetInstance().device.allocateDescriptorSets(allocInfo);

    SetInfo result;
    result.pool = poolInfo.pool;
    result.set = sets[0];

    poolInfo.remainNum = std::max<int>(static_cast<int>(poolInfo.remainNum) - sets.size(), 0);
    if (poolInfo.remainNum == 0) {
        fulledImageSetPool.push_back(poolInfo);
        avalibleImageSetPool.pop_back();
    }

    return result;
}

void vkContext::DescriptorSetManager::FreeImageSet(const SetInfo&info)
{
    auto it = std::find_if(fulledImageSetPool.begin(), fulledImageSetPool.end(),
        [&](const PoolInfo& poolInfo) {
            return poolInfo.pool == info.pool;
        });
    if (it != fulledImageSetPool.end()) {
        it->remainNum++;
        avalibleImageSetPool.push_back(*it);
        fulledImageSetPool.erase(it);
        return;
    }

    it = std::find_if(avalibleImageSetPool.begin(), avalibleImageSetPool.end(),
        [&](const PoolInfo& poolInfo) {
            return poolInfo.pool == info.pool;
        });
    if (it != avalibleImageSetPool.end()) {
        it->remainNum++;
        return;
    }
    Context::GetInstance().device.freeDescriptorSets(info.pool, info.set);
}

void vkContext::DescriptorSetManager::AddImageSetPool()
{
    constexpr uint32_t MaxSetNum = 10;

    vk::DescriptorPoolSize size;
    size.setType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(MaxSetNum);
    vk::DescriptorPoolCreateInfo createInfo;
    createInfo.setMaxSets(MaxSetNum)
        .setPoolSizes(size)
        .setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    auto pool = Context::GetInstance().device.createDescriptorPool(createInfo);
    avalibleImageSetPool.push_back({ pool, MaxSetNum });

}

DescriptorSetManager::PoolInfo& vkContext::DescriptorSetManager::GetAvaliableImagePoolInfo()
{
    if (avalibleImageSetPool.empty()) {
        AddImageSetPool();
        return avalibleImageSetPool.back();
    }
    return avalibleImageSetPool.back();
}
