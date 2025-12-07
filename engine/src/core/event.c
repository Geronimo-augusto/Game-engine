#include "core/event.h"
#include "core/memory.h"
#include "containers/darray.h"
#include "core/logger.h"


//TODO: Torna-lo multi-thread
//TODO: Adicionar prioridade aos eventos

typedef struct registered_event {
    void* listener;
    PFN_on_event callback;
} registered_event;

typedef struct event_code_entry {
    registered_event* events;
} event_code_entry;

// Provavelmente mais do que o suficiente
#define MAX_EVENT_CODES 16384

// Estrutura dos estados internos do sistema de eventos
typedef struct event_system_state {
    // Array de entradas de códigos de eventos
    event_code_entry registered[MAX_EVENT_CODES];
} event_system_state;

// Estado interno do sistema de eventos
static b8 is_initialized = FALSE;
static event_system_state state;

b8 evente_initialize(){
    if(is_initialized == TRUE ){
        return FALSE;
    }

    is_initialized = FALSE;
    kzero_memory(&state, sizeof(state));

    is_initialized = TRUE;
    return TRUE;
}

void event_shutdown(){
    // Limpa todos os listeners registrados
    for(u16 i =0; i< MAX_EVENT_CODES; i++){
        if(state.registered[i].events != 0){
            darray_destroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

b8 event_register(u16 code, void* listener, PFN_on_event on_event){
    if(is_initialized == FALSE){
        KWARN("Tentativa de registrar listener em sistema de eventos não inicializado");
        return FALSE;
    }

    if(state.registered[code].events == 0){
        state.registered[code].events = darray_create(registered_event);
    }

    u64 registered_cout = darray_length(state.registered[code].events);
    for(u64 i=0; i< registered_cout; ++i){
        if(state.registered[code].events[i].listener == listener){
            KWARN("Tentativa de registrar listener duplicado para o código de evento %u", code);
            return FALSE;
        }
    }

    // Se nenhuma duplicado for encontrado, registra o listener
    registered_event new_event;
    new_event.listener = listener;
    new_event.callback = on_event;
    darray_push(state.registered[code].events, new_event);

    return TRUE;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event on_event){
    if(is_initialized == FALSE){
        KWARN("Tentativa de desregistrar listener em sistema de eventos não inicializado");
        return FALSE;
    }

    if(state.registered[code].events == 0){
        KWARN("Nenhum listener registrado para o código de evento %u", code);
        return FALSE;
    }

    u64 registered_count = darray_length(state.registered[code].events);
    for(u64 i=0; i< registered_count; ++i){
        registered_event e = state.registered[code].events[i];
        if(e.listener == listener && e.callback == on_event){
            // Remove o listener encontrado
            registered_event popped_event;
            darray_pop_at(state.registered[code].events, i,&popped_event);
            return TRUE;
        }
    }

    KWARN("Listener não encontrado para o código de evento %u", code);
    return FALSE;
}

b8 event_fire(u16 code, void* sender, event_context context){
    if(is_initialized == FALSE){
        KWARN("Tentativa de disparar evento em sistema de eventos não inicializado");
        return FALSE;
    }

    if(state.registered[code].events == 0){
        // Nenhum listener registrado para esse código de evento
        return FALSE;
    }

    u64 registered_count = darray_length(state.registered[code].events);
    for(u64 i=0; i< registered_count; ++i){
        registered_event e = state.registered[code].events[i];
        if(e.callback(code, sender, e.listener, context)){
            // Evento consumido, para de propagar
            return TRUE;
        }
    }
    // nada encontrado
    return FALSE;
}