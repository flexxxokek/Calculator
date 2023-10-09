#include "main.h"

int main()
{
    Fileinf cmnds = {};

    SPU spu = {};

    SPU_Ctor( &spu, cmnds.trueSize );

    fillFileinf( &cmnds );

    Calculate( &cmnds, &spu );

    return 0;
}