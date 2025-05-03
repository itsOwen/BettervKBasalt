#ifndef EFFECT_CAS_HPP_INCLUDED
#define EFFECT_CAS_HPP_INCLUDED
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
    class CasEffect : public SimpleEffect
    {
    public:
        CasEffect(LogicalDevice*       pLogicalDevice,
                  VkFormat             format,
                  VkExtent2D           imageExtent,
                  std::vector<VkImage> inputImages,
                  std::vector<VkImage> outputImages,
                  Config*              pConfig);
        ~CasEffect();
        
        // Implement Effect interface
        std::string getName() const override { return "cas"; }
        bool updateConfig(Config* pConfig) override;
        std::vector<std::pair<std::string, std::string>> getOptions() const override;
        bool setOption(const std::string& optionName, const std::string& value) override;
        
    private:
        float sharpness = 0.4f;
        LogicalDevice* pLogicalDevice;
        VkFormat format;
        VkExtent2D imageExtent;
        std::vector<VkImage> inputImages;
        std::vector<VkImage> outputImages;
    };
} // namespace vkBasalt

#endif // EFFECT_CAS_HPP_INCLUDED
