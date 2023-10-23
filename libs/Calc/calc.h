#ifndef CALC_
#define CALC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../Stack/stack.h"
#include "../Data/data.h"
#include "../Io/io.h"

const size_t NUM_OF_CMNDS = 32;

const size_t MAX_CMND_LEN = 6;

#define CMND_SPECIFICATOR \
"%4s"

const long long MULTIPLYER = 100;

const int DEAD_INT = 0xdeaddead;

const long long DEAD_LONG_LONG = 0xdeaddeaddeaddead;

const int MIN_REG_NUM = 1;

const int MAX_REG_NUM = 4;

const int REG_LEN = 4;

const size_t NUM_OF_RAM_ELEMS = 200;

enum CALC_ERRS
{
    OK = 0,

    SYNTAX_ERR
};

enum CMNDS
{
    IN                  = 0,
    PUSH                = 1,
    ADDITION            = 2,
    SUBTRACTION         = 3,
    MULTIPLICATION      = 4,
    DIVISION            = 5,
    OUT                 = 6,
    INVALID_SYNTAX      = 7,
    HALT                = 8,
    POP                 = 11,
    JUMP_ABOVE          = 9,
    JUMP_ABOVE_OR_EQUAL = 10,
    JUMP_BELOW          = 12,
    JUMP_BELOW_OR_EQUAL = 13,
    JUMP_EQUAL          = 14,
    JUMP_NOT_EQUAL      = 15,
    JUMP                = 16,
    CALL                = 17,
    RETURN              = 18,
    SQUARE_ROOT         = 19
};

extern const char* CMNDS_NAME[NUM_OF_CMNDS];

struct regNames
{
    StackElem rax;

    StackElem rbx;

    StackElem rcx;

    StackElem rdx;
};

union AllRegs
{
    StackElem buff[sizeof( regNames ) / sizeof( StackElem )];

    regNames names;
};


struct SPU
{
    u_int64_t ip;

    Stack stk;

    void* commands;

    AllRegs regs;

    StackElem RAM[NUM_OF_RAM_ELEMS];
};

enum SPU_ERRS
{
    SPU_IS_NULL = 1 << 1,

    COMMANDS_IS_NULL = 1 << 2,

    STACK_IS_CURSED = 1 << 3,

    FI_IS_LOWER_THAN_NULL = 1 << 4

};



int Calculate( const char* name );

#endif
