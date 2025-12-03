#pragma once

#include "core/application.h"

// Vai representar o basico de um jogo
// Chamada para a criação da aplicação

typedef struct game{
    // Configuração da aplicação
    application_config app_config;

    // Função que aponta a inicialização do jogo
    b8 (*initialize)(struct game* game_inst);

    // Função que aponta ao udpate do jogo
    b8 (*update)(struct game* game_inst, f32 delta_time);

    // Função que aponta o render do jogo
    b8 (*render)(struct game* game_inst, f32 delta_time);

    // Função que aponta para o redicionamente, se aplicado
    void (*on_resize)(struct game* game_inst, u32 width, u32 height);

    // Estado do especifico do jogo, criado e gerenciado pelo proprio jogo
    void* state;
} game;