#include "disassembler.h"

int main()
{
    char name[MAX_FILE_NAME_LEN] = {};

    getFileName( name );

    char destName[MAX_FILE_NAME_LEN] = {};

    getFileName( destName );

    Disassemble( name, destName );

    return 0;
}