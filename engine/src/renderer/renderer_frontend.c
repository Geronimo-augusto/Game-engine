#include "renderer_frontend.h"

#include "renderer_backend.h"

#include "core/logger.h"
#include "core/memory.h"

// Backend render contxt
static renderer_backend* backend = 0;

b8 renderer_initialize(const char* application_name, struct platform_state* plat_state){
    backend = kallocate(sizeof(renderer_backend), MEMORY_TAG_RENDERER);

    // NOTE: nessa parte, se quiser, poderar fazer ter traca de render. Fazendo parar de ser hardcode
    renderer_backend_create(RENDERER_BACKEND_TYPE_VULKAN, plat_state, backend);
    backend->frame_number=0;

    if(!backend->initialize(backend, application_name, plat_state)){
        KFATAL("RENDER backend failed to initialize. Shutting down.");
        return FALSE;
    }

    return TRUE;
}

void renderer_shutdown(){
    backend->shutdown(backend);
    kfree(backend, sizeof(renderer_backend), MEMORY_TAG_RENDERER);
}

b8 renderer_beging_frame(f32 delta_time){
    return backend->begin_frame(backend, delta_time);
}

b8 renderer_end_frame(f32 delta_time){
    b8 result = backend->end_frame(backend,delta_time);
    backend->frame_number++;
    return result;
}

b8 renderer_draw_frame(render_packet* packet){
    // Se o frame retoner um True, as operaçoes do frame continuam.
    // Não necessariamente a "falha" no inicio do frame realmente é um erro
    if(renderer_beging_frame(packet->delta_time)){

        //Fim do freme, se qui tiver erro provavelmente sera algo muito ruim
        b8 result = renderer_end_frame(packet->delta_time);

        if(!result){
            KERROR("Falha no finalizamento do frame. Aplicação finalizando...");
            return FALSE;
        }
    }

    return TRUE;
}

