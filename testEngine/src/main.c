#include <core/logger.h>
#include <core/asserts.h>

//pra formataçao funcionar
// #include <windows.h>
// TODO: teste
#include <core/platform/platform.h>

int main(void){
    //formatação de console para utf-8 (se ativa pelas configs do windows nao precisa)
    // SetConsoleOutputCP(CP_UTF8);
    KFATAL ("Esse é um log fatal: %f", 3.14f);
    KERROR ("Esse é um log fatal: %f", 3.14f);
    KWARN ("Esse é um log fatal: %f", 3.14f);
    KINFO ("Esse é um log fatal: %f", 3.14f);
    KDEBUG ("Esse é um log fatal: %f", 3.14f);
    KTRACE ("Esse é um log fatal: %f", 3.14f);

    platform_state state;
    if(platform_startup(&state, "Test Engine Window", 100,100,1280,720)){
        while (TRUE)
        {
            platform_pump_messages(&state);
        }
    }
    platform_shutdown(&state);
    return 0;
}