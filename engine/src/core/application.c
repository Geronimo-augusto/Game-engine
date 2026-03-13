#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"
#include "memory.h"
#include "event.h"
#include "input.h"
#include "clock.h"

#include "renderer/renderer_frontend.h"


#include <stdio.h>

#include "containers/darray.h"

typedef struct application_state{
    platform_state platform;
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    i16 width;
    i16 height;
    i8 fps;
    clock clock;
    f64 last_time;
} application_state;

static b8 initialize = FALSE;
static application_state app_state;

// Event handlers
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context);
b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context);

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
    // for(int i=0; i<sizeof(l); i++){
    //     KINFO("%c,%zu", i, i);
    // }

    // Registra os eventos da aplicação
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key);
    

    // Inicializa a plataforma
    if(!platform_startup(
        &app_state.platform,
       game_inst-> app_config.name,
       game_inst-> app_config.start_pos_x,
       game_inst-> app_config.start_pos_y,
       game_inst-> app_config.start_width,
       game_inst-> app_config.start_height)){
        return FALSE;
    }

    // Renderer startup
    if(!renderer_initialize(game_inst->app_config.name, &app_state.platform)){
        KFATAL("Failed to initialize renderer. Aborting application.");
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
    clock_start(&app_state.clock);
    clock_update(&app_state.clock);
    app_state.last_time = app_state.clock.elapsed;
    app_state.fps = 120;
    f64 running_time = 0;
    u8 frame_count = 0;
    f64 target_frame_seconds = 1.0f/ app_state.fps;

    KINFO(get_memory_usage_string());
    
    while (app_state.is_running){
        if(!platform_pump_messages(&app_state.platform)){
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended){
            // Atualiza o delta e o relogio
            clock_update(&app_state.clock);
            f64 current_time = app_state.clock.elapsed;
            f64 delta = (current_time - app_state.last_time);
            f64 frame_start_time = platform_get_abs_time();

            if(!app_state.game_inst->update(app_state.game_inst,(f32)delta)){
                KFATAL("Falha no update do jogo");
                app_state.is_running = FALSE;
                break;
            }

            // Chama o render
            if(!app_state.game_inst->render(app_state.game_inst, (f32)delta )){
                KFATAL("Falha no render do jogo");
                app_state.is_running = FALSE;
                break;
            }

            //HACK: apenas para funcionar agora isso precisa ser mudado pra algo mais robusto
            render_packet packet;
            packet.delta_time = delta;
            renderer_draw_frame(&packet);

            // Analise para ver quanto tempo demorou para o frame executar e se "estourou" o tempo
            f64 frame_end_time = platform_get_abs_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_seconds = target_frame_seconds - frame_elapsed_time;

            // metodo de "troco" para o sistema. Evitar de consumir mais que o necessario
            if(remaining_seconds>0){
                u64 remaining_ms =( remaining_seconds*1000);

                // Se tiver "troco" devolve pra o OS
                b8 limit_frames = FALSE;
                if(remaining_ms>0&& limit_frames){
                    platform_sleep(remaining_ms-1);
                }
                frame_count++;
            }


            // NOTE: A atualização de entrada/copiar o estado deve sempre ser tratada após qualquer entrada ser registrada; ou seja, antes desta linha. 
            //Por segurança, a entrada é a última coisa a ser atualizada antes do término Frame.
            input_update(delta);

            //Ultimo update
            app_state.last_time = current_time;
        }
    }

    app_state.is_running = FALSE;

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    event_shutdown();
    input_shutdown();

    renderer_shutdown();

    platform_shutdown(&app_state.platform);

    return TRUE;
} 

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context){
    switch(code){
        case EVENT_CODE_APPLICATION_QUIT: {
            KINFO("Evento de saída da aplicação recebido. Desligando...");
            app_state.is_running = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}

b8 application_on_key(u16 code, void* sender, void* listener_inst, event_context context){
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key = context.data.u16[0];
        switch (key){
            case KEY_ESCAPE:{
                //NOTE: tecnnicamente esta disparando um evento dentro de outro evento, mas pode ter outros listeners
                // event_context data ={};
                // event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
                // bloqueia o evento para outros listeners
                return TRUE;
            }break;
            case KEY_A:{
                // teste test
                // KDEBUG("A tecla A foi pressionada");
            }break;
            default:{
                KDEBUG("Tecla '%c' pressionada", key);
            }break;
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key = context.data.u16[0];
        switch (key){
            case KEY_B:{
                // KDEBUG("A tecla B foi solta");
            }
            default:{
                // KDEBUG("Tecla '%c' solta", key);
            }
        }
   }
    return FALSE;
}