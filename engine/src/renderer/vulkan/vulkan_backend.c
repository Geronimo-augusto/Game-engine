#include "vulkan_backend.h"
#include "vulkan_platform.h"
#include "vulkan_types.inl"

#include "core/logger.h"
#include "core/string.h"

#include "containers/darray.h"
#include "platform/platform.h"

// Contexto estatico do Vulkan
static vulkan_context context;

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data);


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

    // Obtém a lista de extensões requeridas
    const char** required_extensions = darray_create(const char*);
    darray_push(required_extensions, &VK_KHR_SURFACE_EXTENSION_NAME); // pega o nome da extensão genérica pra a renderização de superficies
    platform_get_required_extension_names(&required_extensions);  // vai pegar as extensões especificas para cada plataforma (OS)
    
#if defined(_DEBUG)
    darray_push (required_extensions, &VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // Ferramentas para dbug

    KDEBUG("Extensões Necessárias:");
    u32 length = darray_length(required_extensions);
    for (u32 i = 0; i<length; ++i){
        KDEBUG(required_extensions[i])
    }
#endif

    create_info.enabledExtensionCount = 0;
    create_info.ppEnabledExtensionNames=0;

    // Validação das layers
    const char** required_validation_layer_names = 0;
    u32 required_validation_layer_count = 0;

    // Se a validação for necessária, pegamos a lista required_validation_layer_names
    // e fazemos ter certeza de sua existência. Deve ser ativado apenas em builds non-release.
#if defined(_DEBUG)
    KINFO ("Validação de Layers ativada. Enumerando...");

    //A Lista de camadas de validação necessárias
    required_validation_layer_names = darray_create(const char*);
    darray_push(required_validation_layer_names, &"VK_LAYER_KHRONOS_validation");
    required_validation_layer_count = darray_length(required_validation_layer_names);

    // Obtém uma lista de camadas de validação disponíveis
    u32 available_layer_count = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, 0));
    VkLayerProperties* available_layers = darray_reserve(VkLayerProperties, available_layer_count);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers));

    // Verificação se todas as layers estão disponíveis.
    for (u32 i = 0; i< required_validation_layer_count; ++i){
        KINFO("Procurando pelas Layers: %s...", required_validation_layer_names[i]);
        b8 found = FALSE;
        for(u32 j = 0; j< available_layer_count; ++j){
            if (strings_equal(required_validation_layer_names[i], available_layers[j].layerName)){
                found = TRUE;
                KINFO("Encontrado.");
                break;
            }
        }

        if(!found){
            KFATAL("Layer de validação esta faltando: %s", required_validation_layer_names[i]);
            return FALSE;
        }
    }
    KINFO("Todas as layers estão presentes")
#endif



    create_info.enabledLayerCount = required_validation_layer_count; // vai nos permitir fazer debug, algo que o vulkan nao faz por padrão
    create_info.ppEnabledLayerNames = required_validation_layer_names;

    VK_CHECK(vkCreateInstance(&create_info, context.allocator, &context.instance)); // funçãozinha que apenas serve como macro para um if simples de verificação
    KINFO("Instancia do Vulkan criada ")

    KINFO("Sucesso na inicialização do Vulkan.");
    return TRUE;

// Debugger
#if defined(_DEBUG)
    KDEBUG("Criando Vulkan debugger...");
    // Esses comentados sao outros tipos de mensagens de debug, mas que nao serão tao uteis assim. o info talvez, ja o verbose quando for algo muito avançado
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT; // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
     

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT};
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = vk_debug_callback;
    debug_create_info.pUserData = 0;

    // O carregamento do function pointe da extensão do Vulkan
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkCreateDebugUtilsMessengerEXT");
    KASSERT_MSG(func, "Falha na criação das mensagens de debug");
    VK_CHECK(func(context.instance, &debug_create_info, context.allocator, &context.debug_messenger));
    KDEBUG("Vulkan Debugger criado.")
#endif
}

void vulkan_shutdown(renderer_backend* backend){

    // teoricamente so precisaria destruir a instancia do vulkan, mas como ele age diretamente nos driver da gpu. é melhor colocarmos explicitamente os destroyers
    KDEBUG("Destruindo o Vulkan debugger...");
    if(context.debug_messenger){
        PFN_vkDestroyDebugUtilsMessengerEXT func=(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(context.instance, "vkDestroyDebugUtilsMessengerEXT");
        func(context.instance, context.debug_messenger, context.allocator);
    }

    KDEBUG("Destruindo a Instancia do Vulkan")
}

void vulkan_resized(renderer_backend* backend, u16 width, u16 height){

}

b8 vulkan_begin_frame(renderer_backend* backend, f32 delta_time){
    return TRUE;
}
b8 vulkan_end_frame(renderer_backend* backend, f32 delta_time){
    return TRUE;
}


//Basicamente uma função que vai pegar o nível de severidade e fazer um log para nos
VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_types,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data){
        switch (message_severity){
            default:
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                KERROR(callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                KWARN(callback_data->pMessage);
                break;
            case  VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                KINFO(callback_data->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                KTRACE(callback_data->pMessage);
                break;
        }
        return VK_FALSE; // Precisa retornar um bool pq o Vulkan que ksk
    }
