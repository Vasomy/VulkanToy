#pragma once


#include "vulkan/vulkan.hpp"
#include <vector>
#include <memory>

namespace vkContext {

    class DescriptorSetManager final {
    public:
        struct SetInfo {
            vk::DescriptorSet set;
            vk::DescriptorPool pool;
        };

        static void Init(uint32_t maxFlight) {
            instance.reset(new DescriptorSetManager(maxFlight));
        }

        static void Quit() {
            instance.reset();
        }

        static DescriptorSetManager& GetInstance() {
            return *instance;
        }

        DescriptorSetManager(uint32_t maxFlight);
        ~DescriptorSetManager();

        std::vector<SetInfo> AllocBufferSets(uint32_t num);
        SetInfo AllocImageSet();

        void FreeImageSet(const SetInfo&);

    private:
        struct PoolInfo {
            vk::DescriptorPool pool;
            uint32_t remainNum;
        };

        PoolInfo bufferSetPool;

        std::vector<PoolInfo> fulledImageSetPool;
        std::vector<PoolInfo> avalibleImageSetPool;

        void AddImageSetPool();
        PoolInfo& GetAvaliableImagePoolInfo();

        uint32_t maxFlight;

        inline static std::unique_ptr<DescriptorSetManager> instance = nullptr;
    };

}