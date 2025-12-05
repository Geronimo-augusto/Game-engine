#include "platform.h"

#if KPLATFORM_WINDOWS

#include "core/logger.h"

#include <windows.h>
#include <windowsx.h> // parametro de extraçao de input
#include <stdlib.h>

typedef struct internal_state{
    HINSTANCE h_instance;
    HWND hwnd;
} internal_state;

//clock
static f64 clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_startup(
    platform_state *plat_state,
    const char *application_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height) {
    plat_state->internal_state = malloc(sizeof(internal_state));// o uso do malloc, (uso dinamico de memoria), nao é normalmente recomendado. Entretando por estar em uma layer de "inializaçao", esse uso dinamico não é tao significativo.
    internal_state *state = (internal_state *)plat_state->internal_state;

    state->h_instance = GetModuleHandleA(0);

    // Registro da classe window.
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc,0,sizeof(wc));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;// é chamadao pelo dispach message em platform_pump_messages
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Com ele sendo nulo, o controle se torna manual
    wc.hbrBackground = NULL; //Transparente
    wc.lpszClassName  = "THE_GAME_EGINE";

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

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

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
        window_ex_style, "THE_GAME_EGINE", application_name, 
        window_style, window_x, window_y, window_width, window_height,
        0, 0 , state->h_instance, 0);

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
void *platform_allocate(u64 size, b8 aligned){
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
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, Error, Warn, Info, Debug, Trace
    static u8 levels[6]= {64,4,6,2,1,8};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written=0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void platform_console_write_error(const char* message, u8 color){
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, Error, Warn, Info, Debug, Trace
    static u8 levels[6]= {64,4,6,2,1,8};
    SetConsoleTextAttribute(console_handle, levels[color]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD number_written=0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

f64 platform_get_abs_time(){
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    return (f64)current_time.QuadPart * clock_frequency;
}

void platform_sleep(u64 ms){
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param){
    switch(msg){
        case WM_ERASEBKGND:
            // Notifica ao sistema que o fundo sera a pintado manualmente pela aplicaçao
            return 1;
        case WM_CLOSE:
            // TODO: Enviar evento de fechamento da janela
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:{
            // Pega o Tamanho autalizadao
            // RECT r;
            // GetClientRect(hwnd, &r);
            // u32 width = r.right - r.left;
            // u32 height = r.bottom - r.top;

            // TODO: Enviar evento de redimensionamento
        }break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:{
            // Key apertqada/solta
            // b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            // TODO: input processing
        }break;
        case WM_MOUSEMOVE:{
            // //Mouse move
            // i32 x_pos = GET_X_LPARAM(l_param);
            // i32 y_pos = GET_Y_LPARAM(l_param); 
            // TODO: input processing
        }break;
        case WM_MOUSEWHEEL:{
            // i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if(z_delta != 0){
            //     // Deixando o input em um range de (-1 a 1)
            //     z_delta = (z_delta < 0) ? -1 : 1;
            //     // TODO: input processing
            // }
        }break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:{
            // b8 pressed = (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN);
            // TODO: input processing
        }break;
    }
    return DefWindowProcA(hwnd, msg, w_param, l_param);
};

#endif //Platform Windows
