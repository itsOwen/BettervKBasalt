#include "effect_fxaa.hpp"

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
    FxaaEffect::FxaaEffect(LogicalDevice*       pLogicalDevice,
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
        fxaaQualitySubpix           = pConfig->getOption<float>("fxaaQualitySubpix", 0.75f);
        fxaaQualityEdgeThreshold    = pConfig->getOption<float>("fxaaQualityEdgeThreshold", 0.125f);
        fxaaQualityEdgeThresholdMin = pConfig->getOption<float>("fxaaQualityEdgeThresholdMin", 0.0312f);

        vertexCode   = full_screen_triangle_vert;
        fragmentCode = fxaa_frag;

        std::vector<VkSpecializationMapEntry> specMapEntrys(5);

        for (uint32_t i = 0; i < specMapEntrys.size(); i++)
        {
            specMapEntrys[i].constantID = i;
            specMapEntrys[i].offset     = sizeof(float) * i;
            specMapEntrys[i].size       = sizeof(float);
        }
        std::vector<float> specData = {
            fxaaQualitySubpix, fxaaQualityEdgeThreshold, fxaaQualityEdgeThresholdMin, (float) imageExtent.width, (float) imageExtent.height};

        VkSpecializationInfo fragmentSpecializationInfo;
        fragmentSpecializationInfo.mapEntryCount = specMapEntrys.size();
        fragmentSpecializationInfo.pMapEntries   = specMapEntrys.data();
        fragmentSpecializationInfo.dataSize      = sizeof(float) * specData.size();
        fragmentSpecializationInfo.pData         = specData.data();

        pVertexSpecInfo   = nullptr;
        pFragmentSpecInfo = &fragmentSpecializationInfo;

        init(pLogicalDevice, format, imageExtent, inputImages, outputImages, pConfig);
    }
    FxaaEffect::~FxaaEffect()
    {
    }
    
    bool FxaaEffect::updateConfig(Config* pConfig)
    {
        float newSubpix = pConfig->getOption<float>("fxaaQualitySubpix", 0.75f);
        float newEdgeThreshold = pConfig->getOption<float>("fxaaQualityEdgeThreshold", 0.125f);
        float newEdgeThresholdMin = pConfig->getOption<float>("fxaaQualityEdgeThresholdMin", 0.0312f);
        
        if (newSubpix != fxaaQualitySubpix || newEdgeThreshold != fxaaQualityEdgeThreshold || newEdgeThresholdMin != fxaaQualityEdgeThresholdMin)
        {
            fxaaQualitySubpix = newSubpix;
            fxaaQualityEdgeThreshold = newEdgeThreshold;
            fxaaQualityEdgeThresholdMin = newEdgeThresholdMin;
            
            // Recreate the effect with the new values
            std::vector<VkSpecializationMapEntry> specMapEntrys(5);
            for (uint32_t i = 0; i < specMapEntrys.size(); i++)
            {
                specMapEntrys[i].constantID = i;
                specMapEntrys[i].offset     = sizeof(float) * i;
                specMapEntrys[i].size       = sizeof(float);
            }
            std::vector<float> specData = {
                fxaaQualitySubpix, fxaaQualityEdgeThreshold, fxaaQualityEdgeThresholdMin, (float) imageExtent.width, (float) imageExtent.height};

            VkSpecializationInfo fragmentSpecializationInfo;
            fragmentSpecializationInfo.mapEntryCount = specMapEntrys.size();
            fragmentSpecializationInfo.pMapEntries   = specMapEntrys.data();
            fragmentSpecializationInfo.dataSize      = sizeof(float) * specData.size();
            fragmentSpecializationInfo.pData         = specData.data();

            pFragmentSpecInfo = &fragmentSpecializationInfo;
            
            // Reinitialize the effect
            destroy(pLogicalDevice);
            init(pLogicalDevice, format, imageExtent, inputImages, outputImages, pConfig);
            return true;
        }
        return false;
    }
    
    std::vector<std::pair<std::string, std::string>> FxaaEffect::getOptions() const
    {
        std::vector<std::pair<std::string, std::string>> options;
        options.push_back({"fxaaQualitySubpix", std::to_string(fxaaQualitySubpix)});
        options.push_back({"fxaaQualityEdgeThreshold", std::to_string(fxaaQualityEdgeThreshold)});
        options.push_back({"fxaaQualityEdgeThresholdMin", std::to_string(fxaaQualityEdgeThresholdMin)});
        return options;
    }
    
    bool FxaaEffect::setOption(const std::string& optionName, const std::string& value)
    {
        try
        {
            float newValue = std::stof(value);
            if (optionName == "fxaaQualitySubpix")
            {
                if (newValue != fxaaQualitySubpix)
                {
                    fxaaQualitySubpix = newValue;
                    return true;
                }
            }
            else if (optionName == "fxaaQualityEdgeThreshold")
            {
                if (newValue != fxaaQualityEdgeThreshold)
                {
                    fxaaQualityEdgeThreshold = newValue;
                    return true;
                }
            }
            else if (optionName == "fxaaQualityEdgeThresholdMin")
            {
                if (newValue != fxaaQualityEdgeThresholdMin)
                {
                    fxaaQualityEdgeThresholdMin = newValue;
                    return true;
                }
            }
        }
        catch (...)
        {
            return false;
        }
        return false;
    }
} // namespace vkBasalt
