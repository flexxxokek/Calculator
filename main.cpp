#include "main.h"

int main()
{
    FILE* fp = fopen( "test", "r" );

    Calculate( fp );

    return 0;
}