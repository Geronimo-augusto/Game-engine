#include "application.h"
#include "game_types.h"
#include "logger.h"
#include "platform/platform.h"
#include "memory.h"
#include "event.h"
#include "input.h"

#include "containers/darray.h"

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

char l[] = {
    0x08,
    0x10,
    0x09,
    0x10,
    0x11,

    0x13,
    0x14,

    0x1B,

    0x1C,
    0x1D,
    0x1E,
    0x1F,

    0x32,
    0x21,
    0x22,
    0x23,
    0x24,
    0x25,
    0x26,
    0x27,
    0x28,
    0x29,
    0x2A,
    0x2B,
    0x2C,
    0x2D,
    0x2E,
    0x2F,

    0x0a,
    0x42,
    0x43,
    0x44,
    0x45,
    0x46,
    0x47,
    0x48,
    0x49,
    0x4A,
    0x4B,
    0x4C,
    0x4D,
    0x4E,
    0x4F,
    0x50,
    0x51,
    0x52,
    0x53,
    0x54,
    0x55,
    0x56,
    0x57,
    0x58,
    0x59,
    0x5A,

    0x5B,
    0x5C,
    0x5D,

    0x5F,

    0x60,
    0x61,
    0x62,
    0x63,
    0x64,
    0x65,
    0x66,
    0x67,
    0x68,
    0x69,
    0x6A,
    0x6B,
    0x6C,
    0x6D,
    0x6E,
    0x6F,
    0x70,
    0x71,
    0x72,
    0x73,
    0x74,
    0x75,
    0x76,
    0x77,
    0x78,
    0x79,
    0x7A,
    0x7B,
    0x7C,
    0x7D,
    0x7E,
    0x7F,
    0x80,
    0x81,
    0x82,
    0x83,
    0x84,
    0x85,
    0x86,
    0x87,

    0x90,
    0x91,

    0x92,

    0xA0,
    0xA1,
    0xA2,
    0xA3,
    0xA4,
    0xA5,

    0x59,
    0xBB,
    0x2C,
    0xBD,
    0xBE,
    0xBF,
    0xC0,
};

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

    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key);

    event_shutdown();
    input_shutdown();

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
    KDEBUG("codigo passado '%c'", 1)
   if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key = context.data.u16[0];
        switch (key){
            case KEY_ESCAPE:{
                //NOTE: tecnnicamente esta disparando um evento dentro de outro evento, mas pode ter outros listeners
                event_context data ={};
                event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);
                // bloqueia o evento para outros listeners
                return TRUE;
            }break;
            case KEY_A:{
                // teste test
                KDEBUG("A tecla A foi pressionada");
            }break;
            default:{
                KDEBUG("Tecla '%c' pressionada", key);
            }break;
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key = context.data.u16[0];
        switch (key){
            case KEY_B:{
                KDEBUG("A tecla B foi solta");
            }
            default:{
                KDEBUG("Tecla '%c' solta", key);
            }
        }
   }
    return FALSE;
}