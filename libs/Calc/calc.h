#ifndef CALC_
#define CALC_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../String/stroki.h"
#include "../Stack/stack.h"

const size_t NUM_OF_CMNDS = 6;

const size_t MAX_CMND_LEN = 4;

const long long MULTIPLYER = 100;

enum CALC_ERRS
{
    OK = 0,

    SYNTAX_ERR
};

void Calculate( FILE* fp );

#endif
