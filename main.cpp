#include "main.h"

int main()
{
    Fileinf cmnds = {};
    
    fillFileinf( &cmnds );

    printData( &cmnds );

    Calculate( &cmnds );

    return 0;
}