#ifndef __STK__
#define __STK__

#include <stdio.h>

#define SET_RED_COLOR printf( "\x1b[31;1m" )
#define SET_GREEN_COLOR printf( "\x1b[32;1m" )
#define SET_GRAY_COLOR printf( "\x1b[37;1m")
#define SET_DEFAULT_COLOR printf( "\x1b[0m" )

#define STACK_DUMP( stack )                         \
_StackDump( stack, __FILE__, __func__, __LINE__ );    

#define STACK_CTOR( stack )                 \
StackCtor( stack, #stack, __FILE__,         \
           __func__, __LINE__ );            

typedef long long int StackElem;

typedef unsigned long long Canary;

const unsigned long long LEFT_CHIKEN_DEFAULT_NUM = ( 1ull << 63 ) - 1;

const unsigned long long RIGHT_CHICKEN_DEFAULT_NUM = ( 0xffffffffffffffff );

const size_t STACK_DEFAULT_CAPACITY = 1;

enum STACK_ERRORS
{
    STACK_PTR_IS_NULL = - 98,

    DATA_PTR_IS_NULL = -99,

    SIZE_IS_LOWER_THAN_NULL = -100,

    CAPACITY_IS_LOWER_THAN_NULL = -101,

    SIZE_IS_GREATER_THAN_CAPACITY = -102,

    LEFT_CHIKEN_HAS_FALLEN = -103,

    RIGHT_CHIKEN_HAS_FALLEN = -104,

    DATA_LEFT_CHIKEN_HAS_FALLEN = -105,

    DATA_RIGHT_CHIKEN_HAS_FALLEN = -106,

    HASH_HAS_FALLEN = -107,

    DATA_HASH_HAS_FALLEN = -108,

    NOT_ENOUGH_MEMORY = -109,

    STACK_ALLRIGHT = 0
};

struct Stack
{
    unsigned long long int leftChicken;

    StackElem* data;

    size_t size;

    size_t capacity;

    long long hash;

    long long dataHash;

    const char* varName;

    const char* fileName;

    const char* funcName;

    size_t line; 

    unsigned long long int rightChicken;
};

void StackCtor( struct Stack* stack, const char* varName, const char* fileName,
                const char* funcName, size_t line );

STACK_ERRORS StackPop( struct Stack* stack, StackElem* elem );

STACK_ERRORS StackPush( struct Stack* stack, StackElem elem );

STACK_ERRORS StackVerify( const struct Stack* stack );

void _StackDump ( const struct Stack* stack, const char* fileName,
                  const char* funcName, const size_t line );

STACK_ERRORS StackDtor( struct Stack* stack );

#endif