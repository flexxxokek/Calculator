#include "main.h"

int main()
{
    char fileName[MAX_FILE_NAME_LEN];

    getFileName( fileName );

    Calculate( fileName );

    return 0;
}