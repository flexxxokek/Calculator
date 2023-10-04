#ifndef CALC_
#define CALC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Stack/stack.h"
#include "../String/dataFuncs.h"

const size_t NUM_OF_CMNDS = 7;

const size_t MAX_CMND_LEN = 5;

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
    INVALID_SYNTAX = 7
};

void Calculate( Fileinf* cmnds );

#endif
