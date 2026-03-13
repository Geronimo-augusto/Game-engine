#pragma once

#include "defines.h"

// NOTE: no tutorial ele meio que alem de querer ser multiplataforma, tambem quer ser ter multirenderers. Algo que nao faremos mas vou deixar a estrutura "preparada" caso sim
// e como não irei escrever os codigos para usar os outros render meio que nao vai fazer diferença significativa no final
typedef enum renderer_backend_type{
    RENDERER_BACKEND_TYPE_VULKAN,
    // RENDERER_BACKEND_OPENGL, // render buxa
    // RENDERER_BACKEND_DIRECTX, // render pra console
}renderer_backend_type;

typedef struct renderer_backend{
    struct platform_state* plat_state;
    u64 frame_number;

    b8 (*initialize)(struct renderer_backend* backend, const char* application_name, struct platform_state* plat_state);

    void (*shutdown)(struct renderer_backend* backend);

    void (*resized)(struct renderer_backend* backend, u16 width, u16 height);

    b8 (*begin_frame)(struct renderer_backend* backend, f32 delta_time);
    b8 (*end_frame)(struct renderer_backend* backend, f32 delta_time);
} renderer_backend;

typedef struct render_packet{
    f32 delta_time;
} render_packet;