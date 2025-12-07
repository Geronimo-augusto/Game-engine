#pragma once

#include "defines.h"

typedef struct event_context {
    // 128 bytes para contexto do evento
    union{
        i64 i64[2];
        u64 u64[2];
        f64 f64[2];

        i32 i32[4];
        u32 u32[4];
        f32 f32[4];

        i16 i16[8];
        u16 u16[8];
        
        i8 i8[16];
        u8 u8[16];

        char c[16];        
    }data;
    
} event_context;

//Deve retonar TRUE se ativado
typedef b8 (*PFN_on_event)(u16 code, void* sender, void* listener_inst, event_context data);

b8 evente_initialize();
void event_shutdown();

/**
 * Registra um listener para quando os eventos forem disparados com o código especificado.
 * Eventos com duplicados serao ignorados e nao registrados retornando FALSE.
 * @param code O código do evento a ser escutado.
 * @param listener Um pointer para a instância do listener (pode ser 0/NULL se não for necessário).
 * @param on_event A função de callback a ser chamada quando o evento for disparado.
 * @return TRUE se o listener foi registrado com sucesso, FALSE caso contrário.
 */
KAPI b8 event_register(u16 code, void* listener, PFN_on_event on_event);

/**
 * Desregistra um listener para o código de evento especificado.
 * Se nenhum correspondente for encontrado, retorna FALSE.
 * @param code O código do evento a para de ser escutado.
 * @param listener  Um pointer para a instância do listener (pode ser 0/NULL se não for necessário).
 * @param on_event  A função de callback que sera desregistrada.
 * @return TRUE se o listener foi desregistrado com sucesso, FALSE caso contrário.
 */
KAPI b8 event_unregister(u16 code, void* listener, PFN_on_event on_event);

/**
 * Dispara um evento para todos os listeners registrados com o código especificado.
 * Se o handlers de um evento retornar TRUE, o evento é considerado "consumido" e não é passado para os próximos listeners.
 * @param code O código do evento a ser disparado.
 * @param sender Um pointer para o sender (pode ser 0/NULL se não for necessário).
 * @param context O contexto do evento a ser passado para os listeners.
 */
KAPI b8 event_fire(u16 code, void* sender, event_context context);

// Codigos de eventos reservados pelo sistema. A aplicaçao deve usar códigos acima de 255.

typedef enum system_event_code{
    // Encerra a aplicaçao no proximo quadro.
    EVENT_CODE_APPLICATION_QUIT = 0x01,

    // Tecla do tecldo pressionado.
    /* Contexto usado:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_PRESSED = 0x02,

    // Tecla do tecldo soltado.
    /* Contexto usado:
     * u16 key_code = data.data.u16[0];
     */
    EVENT_CODE_KEY_RELEASED = 0x03,

    // Butao do mouse apertado.
    /* Contexto usado:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_PRESSED = 0x04,

    // Butao do mouse solto.
    /* Contexto usado:
     * u16 button = data.data.u16[0];
     */
    EVENT_CODE_BUTTON_RELEASED = 0x05,

    // Movimento do mouse.
    /* Contexto usado:
     * u16 x = data.data.u16[0];
     * u16 y = data.data.u16[1];
     */
    EVENT_CODE_MOUSE_MOVED = 0x06,

    // Movimento do mouse.
    /* Contexto usado:
     * u8 z_delta = data.data.u8[0];
     */
    EVENT_CODE_MOUSE_WHEEL = 0x07,

    // Tamnho/resoluçao alterado pelo OS.
    /* Contexto usado:
     * u16 width = data.data.u16[0];
     * u16 height = data.data.u16[1];
     */
    EVENT_CODE_RESIZED = 0x08,

    MAX_EVENT_CODE = 0xFF
} system_event_code;