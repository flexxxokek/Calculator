#include "disassembler.h"

int main()
{
    char name[MAX_FILE_NAME_LEN] = {};

    getFileName( name );

    Disassemble( name, "dest1" );

    return 0;
}