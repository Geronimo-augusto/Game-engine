#pragma once

#include "defines.h"

struct game;

// Configuração da aplicação
typedef struct application_config{
    // Nome da aplicação usado na janela. Se necesséario
    const char* name;
    //  Posição e tamanho inicial da janela 
    i32 start_pos_x;
    i32 start_pos_y;
    i32 start_width;
    i32 start_height;
} application_config;


KAPI b8 application_create(struct game* game_inst);

KAPI b8 application_run();