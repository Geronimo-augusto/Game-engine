#pragma once

#include "defines.h"

#include <vulkan/vulkan.h>

//local que fica todas as statics dadas 
typedef struct vulkan_context{
    VkInstance instance;
    VkAllocationCallbacks* allocator;
} vulkan_context;
