#ifndef EFFECT_HPP_INCLUDED
#define EFFECT_HPP_INCLUDED
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "vulkan_include.hpp"
#include "config.hpp"

namespace vkBasalt
{
    class Effect
    {
    public:
        void virtual applyEffect(uint32_t imageIndex, VkCommandBuffer commandBuffer) = 0;
        void virtual updateEffect(){};
        void virtual useDepthImage(VkImageView depthImageView){};
        
        // New methods for runtime configuration
        virtual std::string getName() const = 0;
        virtual bool updateConfig(Config* pConfig) { return false; }
        virtual std::vector<std::pair<std::string, std::string>> getOptions() const { return {}; }
        virtual bool setOption(const std::string& optionName, const std::string& value) { return false; }
        
        virtual ~Effect(){};

    private:
    };
} // namespace vkBasalt

#endif // EFFECT_HPP_INCLUDED
