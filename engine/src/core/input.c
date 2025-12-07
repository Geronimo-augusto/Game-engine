#include "input.h"
#include "event.h"
#include "logger.h"
#include "memory.h"

typedef struct keyboard_state{
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state{
    i32 x;
    i32 y;
    b8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

// Por padrao todas as teclas e botoes estao soltas (FALSE)
typedef struct input_state{
    keyboard_state current_keyboard;
    keyboard_state previous_keyboard;

    mouse_state current_mouse;
    mouse_state previous_mouse;
} input_state;

// Estado interno do sistema de input
static b8 is_initialized = FALSE;
static input_state state = {};

void input_initialize(){
    if(is_initialized){
        KWARN("Tentativa de inicializar sistema de input ja inicializado");
        return;
    }
    kzero_memory(&state, sizeof(state));
    is_initialized = TRUE;
    KINFO("Sistema de input inicializado");
}

void input_shutdown(){
    //TODO: Adicionar rotinas de desligamento que forem necessárias
    is_initialized = FALSE;
    KINFO("Sistema de input desligado");
}

void input_update(f64 delta_time){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return;
    }

    // Atualiza o estado anterior do teclado
    kcopy_memory(&state.previous_keyboard, &state.current_keyboard, sizeof(keyboard_state));

    // Atualiza o estado anterior do mouse
    kcopy_memory(&state.previous_mouse, &state.current_mouse, sizeof(mouse_state));
}

void input_process_key(keys key, b8 pressed){
    // Apenas atualiza se o estado mudou
    if(state.current_keyboard.keys[key] != pressed){
        // Atualiza o estado da tecla
        state.current_keyboard.keys[key] = pressed;

        // Dispara o evento de tecla pressionada/solta
        event_context context;
        context.data.u16[0] =key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED,0,context);
    }
}

void input_process_button(buttons button, b8 pressed){
    // Apenas atualiza se o estado mudou
    if(state.current_mouse.buttons[button] != pressed){
        // Atualiza o estado do botão
        state.current_mouse.buttons[button] = pressed;

        // Dispara o evento de botão pressionado/solto
        event_context context;
        context.data.u16[0] =button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED,0,context);
    }
}   

void input_process_mouse_move(i16x, i16y){
    // Atualiza a posição do mouse apenas se mudar
    if(state.current_mouse.x != x || state.current_mouse.y != y){
        // NOTE: remova o comentario pra fazer debug, se quiser poder copiar no de keyboard tambem
        // KDEBUG("Mouse movido para (%d, %d)", x, y);
        // Atualiza o estado interno
        state.current_mouse.x = x;
        state.current_mouse.y = y;

        // Dispara o evento de movimento do mouse
        event_context context;
        context.data.u16[0] = x;
        context.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, context);
    }
}

void input_process_mouse_wheel(i8 z_delta){
    // NOTE: Não tem um internal state pro wheel
    // Dispara o evento de roda do mouse
    event_context context;
    context.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, context);
}

//input keys
b8 input_is_key_down(keys key){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return FALSE;
    }
    return state.current_keyboard.keys[key]== TRUE;
}

b8 input_is_key_up(keys key){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return TRUE;
    }
    return state.current_keyboard.keys[key]== FALSE;
}

b8 input_was_key_down(keys key){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return FALSE;
    }
    return state.previous_keyboard.keys[key]== TRUE;
}

b8 input_was_key_up(keys key){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return TRUE;
    }
    return state.previous_keyboard.keys[key]== FALSE;
}

//input mouse
b8 input_is_button_down(buttons button){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return FALSE;
    }
    return state.current_mouse.buttons[button]== TRUE;
}

b8 input_is_button_up(buttons button){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return TRUE;
    }
    return state.current_mouse.buttons[button]== FALSE;
}

b8 input_was_button_down(buttons button){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return FALSE;
    }
    return state.previous_mouse.buttons[button]== TRUE;
}

b8 input_was_button_up(buttons button){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        return TRUE;
    }
    return state.previous_mouse.buttons[button]== FALSE;
}

void input_get_mouse_position(i32* x, i32* y){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.current_mouse.x;
    *y = state.current_mouse.y;
}

void input_get_previous_mouse_position(i32* x, i32* y){
    if(!is_initialized){
        KWARN("sistema de input não inicializado");
        *x = 0;
        *y = 0;
        return;
    }
    *x = state.previous_mouse.x;
    *y = state.previous_mouse.y;
}