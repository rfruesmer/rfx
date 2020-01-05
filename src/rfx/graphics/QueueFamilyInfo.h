#pragma once


namespace rfx
{

struct QueueFamilyInfo
{
    uint32_t familyIndex = -1;
    VkQueueFamilyProperties properties;
    std::vector<float> priorities;
    bool supportsPresentation = false;
    std::vector<std::string> flagNames;
};

}
