#include "platformn.h"

#if KPLATFORM_WINDOWS

#include "core/logger.h"

#include <windows.h>
#include <windowsx.h> // parametro de extraçao de input
#include <stdlib.h>

typedf struct internal_state{
    HINSTANCE hInstance;
    HWND hwnd;
} internal_state;

//clock
static f64 clock_frequency;
static LARGE_INTERGER start_time;


LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_startup(
    platform_state* plat_state,
    const char* applicatoin_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height
){
    plat_state ->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state*)plat_state ->internal_state;

    state ->h_instance = GetModuleHandleA(0);

    // Registro da classe window.
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc,0,sizeof(wc));
    wc.styke = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;// é chamadao pelo dispach message em platform_pump_messages
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.HInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Com ele sendo nulo, o controle se torna manual
    wc.hbrBackground = NULL; //Transparente
    wc.lpszMenuName = "THE_GAME_EGINE";

    if(!RegisterClassA(&wc)){
        MessageBoxA(0, "Falha no registro da janela","Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    // Criação da Janela
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_syle |= WS_MAXIMIZEBOX;
    window_syle |= WS_MINMIZEBOX;
    window_syle |= WS_THICKFRAME;

    // Obter o tamanho da janela
    RECT border_rect = {0,0,0,0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    // Caso a borda for negativa
    window_x += border_rect.left;
    window_y += border_rect.top;

    // Grow by the size of the borders
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(
        window_ex_style, "THE_GAME_ENGINE_WINDOW_CLASS", applicatoin_name,, 
        window_style, window_x, window_y, window_width, window_height,
        0, 0 , state->h_instance, 0
    )

    if(handle == 0){
        MessageBoxA(NULL, "Falha na criaçao da janela","Error", MB_ICONEXCLAMATION | MB_OK);
        KFATAL("Falha na criaçao da janela");

        return FALSE;
    } else{
        state->hwnd = handle;
    }

    // Mostrar a janela
    b32 should_activate = 1; // TODO: Se a Janale nao deve aceitar input, deve ser 0
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;     // o Show faz com que aceite input, o Shownoactivate nao aceita input
    // Se precisar inicializar minimizado ou maximizado, adicionar os flags: SW_MINIMIZE : SW_SHOWMINNOACTIVE  ou SW_SHOWMAXIMIZED : SW_MAXIMIZE 

    ShowWindow(state->hwnd, show_window_command_flags);

    // Inicializaçao do clock
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);    
    clock_frequency = 1.0 /(f64)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);
    return TRUE;

}


void platform_shutdown(platform_state* plat_state){
    // Cold-cast simples capra tipo indefinido
    internal_state* state = (internal_state*)plat_state->internal_state;

    if(state->hwnd){
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platform_pump_messages(platform_state* plat_state){
    MSG msg;
    while(PeekMessageA(&msg, NULL, 0,0, PM_REMOVE)){
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
    return TRUE;
}

// TODO: Implementaçao porca, sera mudada depois
void *platform_allocate(u64 size, b8akigned){
    return malloc(size);
}

void platform_free(void* block, b8 aligned){
    free(block);
}

void* platform_zero_memory(void* block, u64 size){
    return memset(block, 0, size);
}

void* platform_copy_memory(void* dest, const void* source, u64 size){
    return memcpy(dest, source, size);
}

void* platform_set_memory(void* dest, i32 value, u64 size){
    return memset(dest, value, size);
}

void platform_console_write(const char* message, u8 color){
    Handle console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, Error, Warn, Info, Debug, Trace
    static u8 levels[6]= {64,4,6,2,1,8};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD nuumber_written=0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, nuumber_written, 0);
}

void platform_console_write_error(const char* message, u8 color){
    Handle console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, Error, Warn, Info, Debug, Trace
    static u8 levels[6]= {64,4,6,2,1,8};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD nuumber_written=0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, nuumber_written, 0);
}

f64 platform_get_abs_time(){
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    return (f64)current_time.QuadPart * clock_frequency;
}

void platform_sleep(u64 ms){
    Sleep(ms);
}

// #endif
