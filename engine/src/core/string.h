#pragma once

#include "defines.h"

// Retorna o comprimento da string dada
KAPI u64 string_length(const char* str);

// Retorna uma copia da string dada. E reserva na memoria
KAPI char* string_duplicate(const char* str);

// Comparador de string (Case-sensitive). True se iguais false se nao.
KAPI b8 strings_equal(const char* str0, const char* str1);