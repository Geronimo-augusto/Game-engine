#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"
//pra formataçao funcionar
// #include <windows.h>

// Definição externa da funçao criar jogo
extern b8 create_game(game* out_game);

// The main Entrey point da aplicaçao
int main(void){
    //formatação de console para utf-8 (se ativa pelas configs do windows nao precisa)
    // SetConsoleOutputCP(CP_UTF8);

    game game_inst;
    if(!create_game(&game_inst)){
        KFATAL("Falha ao criar o jogo");
        return -1;
    }

    // Ter certeza que as funções essenciais estão definidas
    if(!game_inst.initialize || !game_inst.update || !game_inst.render || !game_inst.on_resize){
        KFATAL("Funções essenciais do jogo não estão definidas");
        return -2;
    }

    // Inicializa a aplicação
    if(!application_create(&game_inst)){
        KFATAL("Falha ao criar a aplicação");
        return 1;
    }

    // Inicializa o loop principal
    if(!application_run()){
        KFATAL("Falha ao executar a aplicação");
        return 2;
    }
    
    return 0;
}