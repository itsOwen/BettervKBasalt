#include "effect_cas.hpp"

#include <cstring>

#include "image_view.hpp"
#include "descriptor_set.hpp"
#include "buffer.hpp"
#include "renderpass.hpp"
#include "graphics_pipeline.hpp"
#include "framebuffer.hpp"
#include "shader.hpp"
#include "sampler.hpp"

#include "shader_sources.hpp"

namespace vkBasalt
{
    CasEffect::CasEffect(LogicalDevice*       pLogicalDevice,
                         VkFormat             format,
                         VkExtent2D           imageExtent,
                         std::vector<VkImage> inputImages,
                         std::vector<VkImage> outputImages,
                         Config*              pConfig)
        : pLogicalDevice(pLogicalDevice)
        , format(format)
        , imageExtent(imageExtent)
        , inputImages(inputImages)
        , outputImages(outputImages)
    {
        sharpness = pConfig->getOption<float>("casSharpness", 0.4f);

        vertexCode   = full_screen_triangle_vert;
        fragmentCode = cas_frag;

        VkSpecializationMapEntry sharpnessMapEntry;
        sharpnessMapEntry.constantID = 0;
        sharpnessMapEntry.offset     = 0;
        sharpnessMapEntry.size       = sizeof(float);

        VkSpecializationInfo fragmentSpecializationInfo;
        fragmentSpecializationInfo.mapEntryCount = 1;
        fragmentSpecializationInfo.pMapEntries   = &sharpnessMapEntry;
        fragmentSpecializationInfo.dataSize      = sizeof(float);
        fragmentSpecializationInfo.pData         = &sharpness;

        pVertexSpecInfo   = nullptr;
        pFragmentSpecInfo = &fragmentSpecializationInfo;

        init(pLogicalDevice, format, imageExtent, inputImages, outputImages, pConfig);
    }
    CasEffect::~CasEffect()
    {
    }
    
    bool CasEffect::updateConfig(Config* pConfig)
    {
        float newSharpness = pConfig->getOption<float>("casSharpness", 0.4f);
        if (newSharpness != sharpness)
        {
            sharpness = newSharpness;
            
            // Recreate the effect with the new sharpness value
            VkSpecializationMapEntry sharpnessMapEntry;
            sharpnessMapEntry.constantID = 0;
            sharpnessMapEntry.offset     = 0;
            sharpnessMapEntry.size       = sizeof(float);

            VkSpecializationInfo fragmentSpecializationInfo;
            fragmentSpecializationInfo.mapEntryCount = 1;
            fragmentSpecializationInfo.pMapEntries   = &sharpnessMapEntry;
            fragmentSpecializationInfo.dataSize      = sizeof(float);
            fragmentSpecializationInfo.pData         = &sharpness;

            pFragmentSpecInfo = &fragmentSpecializationInfo;
            
            // Reinitialize the effect
            destroy(pLogicalDevice);
            init(pLogicalDevice, format, imageExtent, inputImages, outputImages, pConfig);
            return true;
        }
        return false;
    }
    
    std::vector<std::pair<std::string, std::string>> CasEffect::getOptions() const
    {
        std::vector<std::pair<std::string, std::string>> options;
        options.push_back({"casSharpness", std::to_string(sharpness)});
        return options;
    }
    
    bool CasEffect::setOption(const std::string& optionName, const std::string& value)
    {
        if (optionName == "casSharpness")
        {
            try
            {
                float newSharpness = std::stof(value);
                if (newSharpness != sharpness)
                {
                    sharpness = newSharpness;
                    // We need to recreate the pipeline with the new sharpness value
                    // This would be similar to updateConfig
                    return true;
                }
            }
            catch (...)
            {
                return false;
            }
        }
        return false;
    }
} // namespace vkBasalt
