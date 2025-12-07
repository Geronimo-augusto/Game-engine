#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"
#include "memory.h"
#include "event.h"
#include "input.h"


typedef struct application_state{
    platform_state platform;
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    i16 width;
    i16 height;
    f64 last_time;
} application_state;

static b8 initialize = FALSE;
static application_state app_state;

b8 application_create(game* game_inst){
    if(initialize){
        KERROR("Aplicação ja inicializada");
        return FALSE;
    }

    app_state.game_inst = game_inst;

    // Inicializaçao de substistemas
    initialize_logging();
    input_initialize();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if(!evente_initialize()){
        KFATAL("Falha na inicialização do sistema de eventos");
        return FALSE;
    }
    
    if(!platform_startup(
        &app_state.platform,
       game_inst-> app_config.name,
       game_inst-> app_config.start_pos_x,
       game_inst-> app_config.start_pos_y,
       game_inst-> app_config.start_width,
       game_inst-> app_config.start_height)){
        return FALSE;
    }

    //Inicializa o jogo
    if(!game_inst->initialize(app_state.game_inst)){
        KFATAL("Falha na inicialização do jogo");
        return FALSE;
    }
    
    app_state.game_inst->on_resize(app_state.game_inst,app_state.width, app_state.height);

    initialize = TRUE;
    return TRUE;

}

b8 application_run(){
    KINFO(get_memory_usage_string());

    while (app_state.is_running){
        if(!platform_pump_messages(&app_state.platform)){
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended){
            f64 current_time = platform_get_abs_time();
            f32 delta_time = (f32)(current_time - app_state.last_time);
            app_state.last_time = current_time;

            if(!app_state.game_inst->update(app_state.game_inst, delta_time)){
                KFATAL("Falha no update do jogo");
                app_state.is_running = FALSE;
                break;
            }

            if(!app_state.game_inst->render(app_state.game_inst, delta_time)){
                KFATAL("Falha no render do jogo");
                app_state.is_running = FALSE;
                break;
            }

            // NOTE: A atualização de entrada/copiar o estado deve sempre ser tratada após qualquer entrada ser registrada; ou seja, antes desta linha. 
            //Por segurança, a entrada é a última coisa a ser atualizada antes do término Frame.
            input_update(0);
        }
    }

    app_state.is_running = FALSE;

    event_shutdown();
    input_shutdown();

    platform_shutdown(&app_state.platform);

    return TRUE;
} 