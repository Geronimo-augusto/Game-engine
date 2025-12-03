#include "logger.h"
#include "asserts.h"
#include "platform/platform.h"

// TODO: temporario
#include <stdio.h>
#include <string.h>
#include <stdarg.h>



b8 initialize_logging() {
    // TODO: criaçao de arquivos de log
    return TRUE;
}

void shutdown_logging(){
    // TODO: limpreza de loggs/escrever entradas pendentes
}

void log_output(log_level level, const char* menssgem, ...){
    const char* level_strings[6] = {
        "[FATAL]: ",
        "[ERROR]: ",
        "[WARN]: ",
        "[INFO]: ",
        "[DEBUG]: ",
        "[TRACE]: "
    };

    b8 is_error = level<LOG_LEVEL_WARN;

    //Sera imposto um limite de 32k de caracteres para os logs,não é recomendado. Entretanto isso evita o deslocamento de memoria dinamica tornando o sistema mais rápido.
    const i32 msg_length = 32000;
    char out_message[msg_length];
    memset(out_message, 0, sizeof(out_message));

    // Formatação da mensagem original.
    // NOTE: O MicroSoftwares's headers sobrescreve o GCC/Clang va_list com um "typedef char* va_list", causando alguns erros estranhos. Por enquanto usaremso o __builtin_va_list,
    // Que é um tipo esperado pelo GCC/Clang.
    __builtin_va_list args_ptr;
    va_start(args_ptr, menssgem);
    vsnprintf(out_message,32000, menssgem, args_ptr);
    va_end(args_ptr);

    char out_message_output[32000];
    sprintf(out_message_output, "%s%s\n", level_strings[level], out_message);

    // Saida especifica para windows
    if(is_error){
        platform_console_write_error(out_message_output, level);
    }else{
        platform_console_write(out_message_output, level);
    }
}

void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line){
    log_output(LOG_LEVEL_NONE, "Assertion failed: (%s), message: %s, in file: %s, line: %d", expression, message, file, line);
}