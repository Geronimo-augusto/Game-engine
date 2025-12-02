#include <core/logger.h>
#include <core/asserts.h>

//pra formataçao funcionar
// #include <windows.h>


int main(void){
    //formatação de console para utf-8 (se ativa pelas configs do windows nao precisa)
    // SetConsoleOutputCP(CP_UTF8);
    KFATAL ("Esse é um log fatal: %f", 3.14f);
    KERROR ("Esse é um log fatal: %f", 3.14f);
    KWARN ("Esse é um log fatal: %f", 3.14f);
    KINFO ("Esse é um log fatal: %f", 3.14f);
    KDEBUG ("Esse é um log fatal: %f", 3.14f);
    KTRACE ("Esse é um log fatal: %f", 3.14f);

    KASSERT(1==0);
    return 0;
}