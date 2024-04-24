#include "../../include/Vulkan/VkHelpers.h"
#include <vector>   //TODO: Remove all calls to new / delete, replace with std::vector. 
#include "../../include/Logger.h"


uint32_t VKR::VkHelpers::FindQueueFamilyIndex(VkPhysicalDevice device, VkQueueFlags flags)
{
    uint32_t familyCount;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    VkQueueFamilyProperties* pFamilies = new VkQueueFamilyProperties[familyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, pFamilies);

    for (uint32_t i = 0; i < familyCount; i++) {
        if (pFamilies[i].queueCount && (pFamilies[i].queueFlags & flags)) {
            return i;
        }
    }

    delete[] pFamilies;

    return 0;
}

uint32_t VKR::VkHelpers::FindMemoryTypeIndex(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    return UINT32_MAX;

}

VkFormat VKR::VkHelpers::FindSupportedFormat(VkPhysicalDevice device, const uint32_t numFormats, const VkFormat* pFormats, VkImageTiling tiling, VkFormatFeatureFlags flags)
{
    const bool isLinear = (tiling == VK_IMAGE_TILING_LINEAR);
    const bool isOptimal = (tiling == VK_IMAGE_TILING_OPTIMAL);

    for (uint32_t i = 0; i < numFormats; i++) {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(device, pFormats[i], &properties);

        if (isLinear && (properties.linearTilingFeatures & flags) == flags) {
            return pFormats[i];
        }
        else if (isOptimal && (properties.optimalTilingFeatures & flags) == flags) {
            return pFormats[i];
        }
    }

    return VK_FORMAT_UNDEFINED;
}

VkFormat VKR::VkHelpers::FindDepthFormat(VkPhysicalDevice device)
{
    const VkFormat formats[3] = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    return FindSupportedFormat(device, 3, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool VKR::VkHelpers::ValidateStencilComponent(VkFormat format)
{
    return (format == VK_FORMAT_D32_SFLOAT_S8_UINT) || (format == VK_FORMAT_D24_UNORM_S8_UINT);
}



bool VKR::VkHelpers::ValidateInstanceExtensionSupport(const char* const extension, const uint32_t extensionPropertyCount, VkExtensionProperties* pExtensionProperties)
{
#if DEBUG
    Log::Debug("[Vulkan]\tValidating extension %s.\n", extension);
#endif

    bool supported = false;

    VkExtensionProperties* instanceSupportedExtensions = nullptr;
    uint32_t numInstanceSupportedExtensions = 0;

    if (pExtensionProperties == nullptr || extensionPropertyCount <= 0)
    {
        //Allocate the instanceSupportedExtensions array. 
        vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceSupportedExtensions, nullptr);
        instanceSupportedExtensions = new VkExtensionProperties[numInstanceSupportedExtensions];
        vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceSupportedExtensions, instanceSupportedExtensions);
    }
    else {  //If extension properties have already been evaluated, Then we don't need to re-evaluate them.
        instanceSupportedExtensions = pExtensionProperties;
        numInstanceSupportedExtensions = extensionPropertyCount;
    }

    //Search the array for the extension. If found, then the extension is supported. 
    if (instanceSupportedExtensions == nullptr) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tInstance Supported Extensions array was nullptr!\n");
        return false;
    }

    for (uint32_t i = 0; i < numInstanceSupportedExtensions; i++)
    {
        if (strcmp(instanceSupportedExtensions[i].extensionName, extension) == 0) {
#if DEBUG
            Log::Debug("[Vulkan]\tExtension %s is supported by the current Instance!\n", extension);
#endif
            supported = true;
            goto ExtensionFound;
        }

    }

#if DEBUG
    //Report if we didn't find the extension. 
    Log::Debug("[Vulkan]\tExtension %s was not supported by the current Instance.\n", extension);
#endif

ExtensionFound:
    //If we allocated the instanceSupportedExtensions array internally, be sure to free it. 
    if (pExtensionProperties == nullptr || extensionPropertyCount <= 0)
    {
        delete[] instanceSupportedExtensions;
    }

    return supported;
}

bool VKR::VkHelpers::ValidateInstanceExtensionSupportArray(const uint32_t numExtensions, const char* const* pExtensions)
{
    bool result = true;

    if (pExtensions == nullptr || numExtensions == 0) {
#if DEBUG
        Log::Debug("[Vulkan]\tValidateArrayInstanceExtensionSupport() was called, but pExtensions was [0x%08x], and numExtensions was %d!\nDefaulting to False\n", pExtensions, numExtensions);
#endif
        return false;
    }

    //Check each extension against the instance's supported extensions. 
    {
        //Evaluate the instance supported extensions once here, and pass through to the Validation function. 
        uint32_t numInstanceExtensions = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, nullptr);
        VkExtensionProperties* instanceSupportedExtensions = new VkExtensionProperties[numInstanceExtensions];
        vkEnumerateInstanceExtensionProperties(nullptr, &numInstanceExtensions, instanceSupportedExtensions);

        for (uint32_t i = 0; i < numExtensions; i++) {
            result &= ValidateInstanceExtensionSupport(pExtensions[i], numInstanceExtensions, instanceSupportedExtensions);
        }

        //Ensure the array is properly deallocated. 
        delete[] instanceSupportedExtensions;
    }

    return result;
}

bool VKR::VkHelpers::ValidateInstanceLayerSupport(const char* const layer, const uint32_t layerPropertyCount, VkLayerProperties* pLayerProperties)
{
#if DEBUG
    Log::Debug("[Vulkan]\tValidating layer %s.\n", layer);
#endif

    bool supported = false;

    VkLayerProperties* instanceSupportedLayers = nullptr;
    uint32_t numInstanceSupportedLayers = 0;

    if (pLayerProperties == nullptr || layerPropertyCount <= 0)
    {
        //Allocate the instanceSupportedLayers array. 
        vkEnumerateInstanceLayerProperties(&numInstanceSupportedLayers, nullptr);
        instanceSupportedLayers = new VkLayerProperties[numInstanceSupportedLayers];
        vkEnumerateInstanceLayerProperties(&numInstanceSupportedLayers, instanceSupportedLayers);
    }
    else {  //If layer properties have already been evaluated, Then we don't need to re-evaluate them.
        instanceSupportedLayers = pLayerProperties;
        numInstanceSupportedLayers = layerPropertyCount;
    }

    //Search the array for the layer. If found, then the layer is supported. 
    if (instanceSupportedLayers == nullptr) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tInstance Supported Layers array was nullptr!\n");
    }

    for (uint32_t i = 0; i < numInstanceSupportedLayers; i++)
    {
        if (strcmp(instanceSupportedLayers[i].layerName, layer) == 0) {
#if DEBUG
            Log::Debug("[Vulkan]\tLayer %s is supported by the current Instance!\n", layer);
#endif
            supported = true;
            goto LayerFound;
        }

    }

#if DEBUG
    //Report if we didn't find the layer. 
    Log::Debug("[Vulkan]\tLayer %s was not supported by the current Instance.\n", layer);
#endif


LayerFound:
    //If we allocated the instanceSupportedLayers array internally, be sure to free it. 
    if (pLayerProperties == nullptr || layerPropertyCount <= 0)
    {
        delete[] instanceSupportedLayers;
    }

    return supported;
}

bool VKR::VkHelpers::ValidateInstanceLayerSupportArray(const uint32_t numLayers, const char* const* pLayers)
{
    bool result = true;

    if (pLayers == nullptr || numLayers == 0) {
#if DEBUG
        Log::Debug("[Vulkan]\tValidateArrayInstanceLayerSupport() was called, but pLayers was [0x%08x], and numLayers was %d!\nDefaulting to True\n", pLayers, numLayers);
#endif
        return true;
    }

    //Check each layer against the instance's supported layers. 
    {
        //Evaluate the instance supported layers once here, and pass through to the Validation function. 
        uint32_t numInstanceLayers = 0;
        vkEnumerateInstanceLayerProperties(&numInstanceLayers, nullptr);
        VkLayerProperties* instanceSupportedLayers = new VkLayerProperties[numInstanceLayers];
        vkEnumerateInstanceLayerProperties(&numInstanceLayers, instanceSupportedLayers);

        for (uint32_t i = 0; i < numLayers; i++) {
            result &= ValidateInstanceLayerSupport(pLayers[i], numInstanceLayers, instanceSupportedLayers);
        }

        //Ensure the array is properly deallocated. 
        delete[] instanceSupportedLayers;
    }

    return result;
}

bool VKR::VkHelpers::ValidatePhysicalDeviceExtensionSupport(const VkPhysicalDevice& device, const char* const extension, const uint32_t extensionPropertyCount, VkExtensionProperties* pExtensionProperties)
{
    bool isSupported = false;
    VkExtensionProperties* instanceSupportedExtensions = nullptr;
    uint32_t numDeviceSupportedExtensions = 0;

    if (pExtensionProperties == nullptr || extensionPropertyCount <= 0)
    {
        //Allocate the instanceSupportedExtensions array. 
        vkEnumerateDeviceExtensionProperties(device, nullptr, &numDeviceSupportedExtensions, nullptr);
        instanceSupportedExtensions = new VkExtensionProperties[numDeviceSupportedExtensions];
        vkEnumerateDeviceExtensionProperties(device, nullptr, &numDeviceSupportedExtensions, instanceSupportedExtensions);
    }
    else {  //If extension properties have already been evaluated, Then we don't need to re-evaluate them.
        instanceSupportedExtensions = pExtensionProperties;
        numDeviceSupportedExtensions = extensionPropertyCount;
    }

    //Search the array for the extension. If found, then the extension is supported. 
    if (instanceSupportedExtensions == nullptr) {
        Log::Error(__FILE__, __LINE__, __PRETTY_FUNCTION__, "[Vulkan]\tDevice Supported Extensions array was nullptr!\n");
    }

    for (uint32_t i = 0; i < numDeviceSupportedExtensions; i++)
    {
        if (strcmp(instanceSupportedExtensions[i].extensionName, extension) == 0) {
#if DEBUG
            Log::Debug("[Vulkan]\tExtension %s is supported by the current Physical Device!\n", extension);
#endif
            isSupported = true;
            goto ExtensionFound;
        }

    }

#if DEBUG
    //Report if we didn't find the extension. 
    Log::Debug("[Vulkan]\tExtension %s was not supported by the current Physical Device.\n", extension);
#endif


ExtensionFound:
    //If we allocated the instanceSupportedExtensions array internally, be sure to free it. 
    if (pExtensionProperties == nullptr || extensionPropertyCount <= 0)
    {
        delete[] instanceSupportedExtensions;
    }

    return isSupported;
}

bool VKR::VkHelpers::ValidatePhysicalDeviceExtensionSupportArray(const VkPhysicalDevice& device, const uint32_t numExtensions, const char* const* pExtensions)
{
    bool result = true;

    if (pExtensions == nullptr || numExtensions == 0) {
#if DEBUG
        Log::Debug("[Vulkan]\tValidatePhysicalDeviceExtensionSupportArray() was called, but pExtensions was [0x%08x], and numExtensions was %d!\nDefaulting to False\n", pExtensions, numExtensions);
#endif
        return false;
    }

    //Check each extension against the device's supported extensions. 
    {
        //Evaluate the device supported extensions once here, and pass through to the Validation function. 
        uint32_t numDeviceExtensions = 0;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &numDeviceExtensions, nullptr);
        VkExtensionProperties* deviceSupportedExtensions = new VkExtensionProperties[numDeviceExtensions];
        vkEnumerateDeviceExtensionProperties(device, nullptr, &numDeviceExtensions, deviceSupportedExtensions);

        for (uint32_t i = 0; i < numExtensions; i++) {
            result &= ValidatePhysicalDeviceExtensionSupport(device, pExtensions[i], numDeviceExtensions, deviceSupportedExtensions);
        }

        //Ensure the array is properly deallocated. 
        delete[] deviceSupportedExtensions;
    }

    return result;
}

bool VKR::VkHelpers::ValidatePhysicalDevicePresentationSupport(const VkPhysicalDevice& device, const uint32_t presentQueueFamilyIndex, VkSurfaceKHR surface)
{
    VkBool32 presentSupported = VK_FALSE;

    vkGetPhysicalDeviceSurfaceSupportKHR(device, presentQueueFamilyIndex, surface, &presentSupported);

    return presentSupported == VK_TRUE;
}



VkResult VKR::VkHelpers::QueryPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice device, VkSurfaceKHR surface, VkSurfaceCapabilitiesKHR& capabilities)
{
    return vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);
}

std::vector<VkSurfaceFormatKHR> VKR::VkHelpers::QueryPhysicalDeviceSurfaceFormats(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t numFormats = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(numFormats);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &numFormats, formats.data());
    return formats;
}

std::vector<VkPresentModeKHR> VKR::VkHelpers::QueryPhysicalDeviceSurfacePresentModes(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t numModes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numModes, nullptr);
    std::vector<VkPresentModeKHR> presentModes(numModes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &numModes, presentModes.data());

    return presentModes;
}

