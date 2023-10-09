#ifndef CALC_
#define CALC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Stack/stack.h"
#include "../String/dataFuncs.h"

const size_t NUM_OF_CMNDS = 10;

const size_t MAX_CMND_LEN = 6;

#define CMND_SPECIFICATOR \
"%4s"

const long long MULTIPLYER = 100;

enum CALC_ERRS
{
    OK = 0,

    SYNTAX_ERR
};

enum CMNDS
{
    IN = 0,
    PUSH = 1,
    ADDITION = 2,
    SUBTRACTION = 3,
    MULTIPLICATION = 4,
    DIVISION = 5,
    OUT = 6,
    INVALID_SYNTAX = 7,
    HALT = 8,
    POP_REG = 1 << 3 | 1 << 1 | 1
};

extern const char* CMNDS_NAME[NUM_OF_CMNDS];

struct SPU
{
    size_t fi;

    Stack stk;

    void* commands;

    StackElem rax;
    
    StackElem rbx;

    StackElem rcx;

    StackElem rdx;
};

enum SPU_ERRS
{
    SPU_IS_NULL = 1 << 1,

    COMMANDS_IS_NULL = 1 << 2,

    STACK_IS_NULL = 1 << 3,

    FI_IS_LOWER_THAN_NULL = 1 << 4

};

void SPU_Ctor( SPU* spu, u_int64_t comandsBuffSize );

CALC_ERRS Calculate( Fileinf* cmnds, SPU* spu );

#endif
