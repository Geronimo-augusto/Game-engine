#pragma once

#include "defines.h"

/*
    Dynamic Array (DArray) - Um array que pode crescer e encolher dinamicamente conforme necessário.
    Fornece operações para adicionar, remover e acessar elementos de forma eficiente.

    Layout da Memória:
    u64 capacity;    // Capacidade total do array (quantidade máxima de elementos que pode conter sem realocar)
    u64 length;      // Número atual de elementos no array
    u64 stride;      // Tamanho em bytes de cada elemento
    void* elements;    // Ponteiro para o bloco de memória que armazena os elementos do array
*/

enum{
    DARRAY_CAPACITY,
    DARRAY_LENGTH,
    DARRAY_STRIDE,
    DARRAY_FIELD_LENGTH,
};

// O _ é pra definir a funçao como interna/privada, mesmo que va ser usadas como se fossem publicas
//elas nao serao chamadas diretamente, utilizaremos alguns macros para isso para pegar a informaçao e processar(pegar o tipo e etc...
KAPI void* _darray_create(u64 length,u64 stride);
KAPI void  _darray_destroy(void* array);

KAPI u64 _darray_field_get(void* array, u64 field);
KAPI void  _darray_field_set(void* array, u64 field, u64 value);

KAPI void* _darray_resize(void* array);

KAPI void* _darray_push(void* array, const void* value_ptr);
KAPI void  _darray_pop(void* array, void* dest);

KAPI void* _darray_pop_at(void* array, u64 index, void* dest);
KAPI void* _darray_insert_at(void* array, u64 index, void* value_ptr);

#define DARRAY_DEFAUT_CAPACITY 1
#define DARRAY_RESIZE_FACTOR 2

// Macros para facilitar o uso do DArray
#define darray_create(type)\
    _darray_create(DARRAY_DEFAUT_CAPACITY, sizeof(type))

#define darray_reserve(type, capacity)    \
    _darray_create(capacity, sizeof(type))

#define darray_destroy(array) \
    _darray_destroy(array)

#define darray_push(array,values)                  \
    {                                             \
        typeof(values) temp_value = values;       \
        array = _darray_push(array, &temp_value); \
    }
// NOTE: Poderia ter usado o __auto_type, mas o intellisense do VS Code nao reconhece. Como ambos sao GNU extensions, optei por usar o typeof mesmo

#define darray_pop(array, value_ptr)          \
    _darray_pop(array, value_ptr)

#define darray_insert_at(array, index, values)        \
    {                                             \
        typeof(values) temp_value = values;       \
        array = _darray_insert_at(array, index, &temp_value); \
    }    

#define darray_pop_at(array, index, value_ptr)    \
    _darray_pop_at(array, index, value_ptr)

#define darray_clear(array) \
    _darray_field_set(array, DARRAY_LENGTH, 0)

#define darray_capacity(array) \
    _darray_field_get(array, DARRAY_CAPACITY)

#define darray_length(array) \
    _darray_field_get(array, DARRAY_LENGTH)

#define darray_stride(array) \
    _darray_field_get(array, DARRAY_STRIDE)

#define darray_length_set(array, value) \
    _darray_field_set(array, DARRAY_LENGTH, value)

