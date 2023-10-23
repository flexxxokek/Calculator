#include <stdio.h>

#include "../Calc/calc.h"
#include "../Stack/stack.h"
#include "../String/stroki.h"
#include "../String/dataFuncs.h"

void Assemble( const char* destFileName );

const size_t MAX_LABEL_LEN = 8;

#define LABEL_SPEC "%8s"

struct Label
{
    char name[MAX_LABEL_LEN];

    u_int64_t ip;
};
