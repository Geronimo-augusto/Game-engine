#pragma once
#include "defines.h"

typedef struct clock{
    f64 start_time;
    f64 elapsed;
} clock;

//Atualiza o relogio provido. Deve ser chamada apenas antes de checar o tempo decorrido
// Nao tem nenhum efeito em relogios nao inicados
void clock_update(clock* clock);

// Inicia o relogio provido. Reseta o tempo decorrido
void clock_start(clock* clock);

// Encera o relogio provido. Não reseta o tempo decorrido
void clock_stop(clock* clock);

