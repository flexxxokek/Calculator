#include "data.h"

int64_t getFileSize( const char* fileName )
{   
    struct stat file = {};

    stat( fileName, &file ); //proverka

    return file.st_size;
}