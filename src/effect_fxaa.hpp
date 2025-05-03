#ifndef EFFECT_FXAA_HPP_INCLUDED
#define EFFECT_FXAA_HPP_INCLUDED
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include "vulkan_include.hpp"

#include "effect_simple.hpp"
#include "config.hpp"

namespace vkBasalt
{
    class FxaaEffect : public SimpleEffect
    {
    public:
        FxaaEffect(LogicalDevice*       pLogicalDevice,
                   VkFormat             format,
                   VkExtent2D           imageExtent,
                   std::vector<VkImage> inputImages,
                   std::vector<VkImage> outputImages,
                   Config*              pConfig);
        ~FxaaEffect();
        
        // Implement Effect interface
        std::string getName() const override { return "fxaa"; }
        bool updateConfig(Config* pConfig) override;
        std::vector<std::pair<std::string, std::string>> getOptions() const override;
        bool setOption(const std::string& optionName, const std::string& value) override;
        
    private:
        float fxaaQualitySubpix = 0.75f;
        float fxaaQualityEdgeThreshold = 0.125f;
        float fxaaQualityEdgeThresholdMin = 0.0312f;
        LogicalDevice* pLogicalDevice;
        VkFormat format;
        VkExtent2D imageExtent;
        std::vector<VkImage> inputImages;
        std::vector<VkImage> outputImages;
    };
} // namespace vkBasalt

#endif // EFFECT_FXAA_HPP_INCLUDED
