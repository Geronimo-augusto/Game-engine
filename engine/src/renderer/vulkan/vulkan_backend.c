#include "vulkan_backend.h"

#include "vulkan_types.inl"

#include "core/logger.h"

// Contexto estatico do Vulkan
static vulkan_context context;

b8 vulkan_initialize(renderer_backend* backend, const char* application_name, struct platform_state* plat_state){

    //TODO: Allocator proprio nosso, nao entendi muito sendo que ja temos um. talvez nao um dos melhores mas temos k
    context.allocator = 0;

    //Setup da instancia do Vulkan
    //Importante
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
    app_info.apiVersion = VK_API_VERSION_1_4;
    app_info.pApplicationName = application_name;
    //nem tanto
    app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
    app_info.pEngineName = "THE GAME ENGINE";
    app_info.engineVersion = VK_MAKE_VERSION(1,0,0);

    // No inicio de toda "estrutura" do vulkan o 1° argumento é o tipo
    VkInstanceCreateInfo create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames=0;
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = 0;

    VkResult result = vkCreateInstance(&create_info, context.allocator, &context.instance);
    if(result != VK_SUCCESS){
        KERROR("Falha nao criaçao de instacia do vulkan: %u", result);
        return FALSE;
    }

    KINFO("Sucesso na inicialização do Vulkan.");
    return TRUE;

}

void vulkan_shutdown(renderer_backend* backend){

}

void vulkan_resized(renderer_backend* backend, u16 width, u16 height){

}

b8 vulkan_begin_frame(renderer_backend* backend, f32 delta_time){
    return TRUE;
}
b8 vulkan_end_frame(renderer_backend* backend, f32 delta_time){
    return TRUE;
}