#ifndef LOGICAL_SWAPCHAIN_HPP_INCLUDED
#define LOGICAL_SWAPCHAIN_HPP_INCLUDED
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <mutex>

#include "effect.hpp"

#include "vulkan_include.hpp"

#include "logical_device.hpp"

namespace vkBasalt
{
    // for each swapchain, we have the Images and the other stuff we need to execute the compute shader
    struct LogicalSwapchain
    {
        LogicalDevice*                       pLogicalDevice;
        VkSwapchainCreateInfoKHR             swapchainCreateInfo;
        VkExtent2D                           imageExtent;
        VkFormat                             format;
        uint32_t                             imageCount;
        std::vector<VkImage>                 images;
        std::vector<VkImage>                 fakeImages;
        std::vector<VkCommandBuffer>         commandBuffersEffect;
        std::vector<VkCommandBuffer>         commandBuffersNoEffect;
        std::vector<VkSemaphore>             semaphores;
        std::vector<std::shared_ptr<Effect>> effects;
        std::shared_ptr<Effect>              defaultTransfer;
        VkDeviceMemory                       fakeImageMemory;

        void destroy();
        
        // Runtime effect management
        bool addEffect(const std::string& effectName, size_t position = SIZE_MAX);
        bool removeEffect(size_t index);
        bool moveEffect(size_t fromIndex, size_t toIndex);
        bool replaceEffect(size_t index, const std::string& newEffectName);
        void recreateEffects();
        void updateEffectConfig(Config* pConfig);
        
        // Get effect information
        std::vector<std::string> getEffectNames() const;
        Effect* getEffect(size_t index) const;
        size_t getEffectCount() const { return effects.size(); }
        
    private:
        mutable std::mutex effectMutex;
        bool needsRecreation = false;
    };
} // namespace vkBasalt

#endif // LOGICAL_SWAPCHAIN_HPP_INCLUDED
