#include "renderer_backend.h"

#include "vulkan/vulkan_backend.h"

b8 renderer_backend_create(renderer_backend_type type, struct platform_state* plat_state, renderer_backend* out_renderer_backend){
    out_renderer_backend->plat_state = plat_state;

    if(type == RENDERER_BACKEND_TYPE_VULKAN){
        out_renderer_backend->initialize = vulkan_initialize;
        out_renderer_backend->shutdown = vulkan_shutdown;
        out_renderer_backend->begin_frame = vulkan_begin_frame;
        out_renderer_backend->end_frame = vulkan_end_frame;
        out_renderer_backend->resized = vulkan_resized;

        return TRUE;
    }
    // se a criaçao nao der certou ou passar um tipo de renderer invalido falha
    return FALSE;
}

// basicamente apenas "anula"/ volta ao padrao, os valores do renderer
void renderer_backend_destroy(renderer_backend* renderer_backend){
    renderer_backend->initialize = 0;
    renderer_backend->shutdown = 0;
    renderer_backend->begin_frame = 0;
    renderer_backend->end_frame = 0;
    renderer_backend->resized = 0;
}