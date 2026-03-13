#pragma once

#include "defines.h"
#include "core/asserts.h"

#include <vulkan/vulkan.h>

//Checa a expressão dada com VK_SUCCESS e retorna seu resultado
#define VK_CHECK(expr)               \
    {                                \
        KASSERT(expr == VK_SUCCESS); \
    }

//local que fica todas as statics dadas 
typedef struct vulkan_context{
    VkInstance instance;
    VkAllocationCallbacks* allocator;

#if defined(_DEBUG)
    VkDebugUtilsMessengerEXT debug_messenger;
#endif    
} vulkan_context;
