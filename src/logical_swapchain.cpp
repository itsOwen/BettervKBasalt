#include "logical_swapchain.hpp"
#include "logger.hpp"
#include "effect_factory.hpp"
#include "format.hpp"
#include "config.hpp"
#include "command_buffer.hpp"

namespace vkBasalt
{
    void LogicalSwapchain::destroy()
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        if (imageCount > 0)
        {
            effects.clear();
            defaultTransfer.reset();

            pLogicalDevice->vkd.FreeCommandBuffers(
                pLogicalDevice->device, pLogicalDevice->commandPool, commandBuffersEffect.size(), commandBuffersEffect.data());
            pLogicalDevice->vkd.FreeCommandBuffers(
                pLogicalDevice->device, pLogicalDevice->commandPool, commandBuffersNoEffect.size(), commandBuffersNoEffect.data());
            Logger::debug("after free commandbuffer");

            pLogicalDevice->vkd.FreeMemory(pLogicalDevice->device, fakeImageMemory, nullptr);

            for (uint32_t i = 0; i < fakeImages.size(); i++)
            {
                pLogicalDevice->vkd.DestroyImage(pLogicalDevice->device, fakeImages[i], nullptr);
            }

            for (unsigned int i = 0; i < imageCount; i++)
            {
                pLogicalDevice->vkd.DestroySemaphore(pLogicalDevice->device, semaphores[i], nullptr);
            }
            Logger::debug("after DestroySemaphore");
        }
    }

    bool LogicalSwapchain::addEffect(const std::string& effectName, size_t position)
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        // Determine image indices for the new effect
        size_t insertPos = (position == SIZE_MAX || position > effects.size()) ? effects.size() : position;
        
        std::vector<VkImage> inputImages;
        std::vector<VkImage> outputImages;
        
        // Calculate input/output images based on position
        if (insertPos == 0)
        {
            // First effect uses fake images as input
            inputImages = std::vector<VkImage>(fakeImages.begin(), fakeImages.begin() + imageCount);
        }
        else
        {
            // Use output of previous effect as input
            inputImages = std::vector<VkImage>(fakeImages.begin() + imageCount * insertPos,
                                             fakeImages.begin() + imageCount * (insertPos + 1));
        }
        
        if (insertPos == effects.size())
        {
            // Last effect outputs to swapchain images (if supported) or last fake images
            outputImages = pLogicalDevice->supportsMutableFormat ? images : 
                          std::vector<VkImage>(fakeImages.end() - imageCount, fakeImages.end());
        }
        else
        {
            // Output to next set of fake images
            outputImages = std::vector<VkImage>(fakeImages.begin() + imageCount * (insertPos + 1),
                                              fakeImages.begin() + imageCount * (insertPos + 2));
        }
        
        // Create the new effect
        auto newEffect = EffectFactory::createEffect(effectName, pLogicalDevice, format, 
                                                   imageExtent, inputImages, outputImages, 
                                                   pConfig.get());
        
        if (!newEffect)
        {
            Logger::err("Failed to create effect: " + effectName);
            return false;
        }
        
        // Insert the effect
        effects.insert(effects.begin() + insertPos, newEffect);
        needsRecreation = true;
        
        return true;
    }
    
    bool LogicalSwapchain::removeEffect(size_t index)
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        if (index >= effects.size())
            return false;
        
        effects.erase(effects.begin() + index);
        needsRecreation = true;
        
        return true;
    }
    
    bool LogicalSwapchain::moveEffect(size_t fromIndex, size_t toIndex)
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        if (fromIndex >= effects.size() || toIndex >= effects.size())
            return false;
        
        if (fromIndex == toIndex)
            return true;
        
        auto effect = effects[fromIndex];
        effects.erase(effects.begin() + fromIndex);
        effects.insert(effects.begin() + toIndex, effect);
        needsRecreation = true;
        
        return true;
    }
    
    bool LogicalSwapchain::replaceEffect(size_t index, const std::string& newEffectName)
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        if (index >= effects.size())
            return false;
        
        // Get input/output images of the current effect
        // Note: This is a simplified version, you might need to track these properly
        std::vector<VkImage> inputImages;
        std::vector<VkImage> outputImages;
        
        // Create the new effect
        auto newEffect = EffectFactory::createEffect(newEffectName, pLogicalDevice, format, 
                                                   imageExtent, inputImages, outputImages, 
                                                   pConfig.get());
        
        if (!newEffect)
        {
            Logger::err("Failed to create replacement effect: " + newEffectName);
            return false;
        }
        
        effects[index] = newEffect;
        needsRecreation = true;
        
        return true;
    }
    
    void LogicalSwapchain::recreateEffects()
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        if (!needsRecreation)
            return;
        
        // Free existing command buffers
        if (commandBuffersEffect.size() > 0)
        {
            pLogicalDevice->vkd.FreeCommandBuffers(pLogicalDevice->device, pLogicalDevice->commandPool, 
                                                 commandBuffersEffect.size(), commandBuffersEffect.data());
            commandBuffersEffect.clear();
        }
        
        // Reallocate command buffers
        commandBuffersEffect = allocateCommandBuffer(pLogicalDevice, imageCount);
        
        // Rewrite command buffers with current effects
        VkImageView depthImageView = pLogicalDevice->depthImageViews.size() ? pLogicalDevice->depthImageViews[0] : VK_NULL_HANDLE;
        VkImage depthImage = pLogicalDevice->depthImageViews.size() ? pLogicalDevice->depthImages[0] : VK_NULL_HANDLE;
        VkFormat depthFormat = pLogicalDevice->depthImageViews.size() ? pLogicalDevice->depthFormats[0] : VK_FORMAT_UNDEFINED;
        
        writeCommandBuffers(pLogicalDevice, effects, depthImage, depthImageView, depthFormat, commandBuffersEffect);
        
        needsRecreation = false;
    }
    
    void LogicalSwapchain::updateEffectConfig(Config* pConfig)
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        for (auto& effect : effects)
        {
            if (effect->updateConfig(pConfig))
            {
                needsRecreation = true;
            }
        }
    }
    
    std::vector<std::string> LogicalSwapchain::getEffectNames() const
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        std::vector<std::string> names;
        names.reserve(effects.size());
        
        for (const auto& effect : effects)
        {
            names.push_back(effect->getName());
        }
        
        return names;
    }
    
    Effect* LogicalSwapchain::getEffect(size_t index) const
    {
        std::lock_guard<std::mutex> lock(effectMutex);
        
        if (index < effects.size())
            return effects[index].get();
        
        return nullptr;
    }
} // namespace vkBasalt
