#ifndef DATA_FUNCS_
#define DATA_FUNCS_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/file.h>

#include "stroki.h"
#include "constants.h"

void printData( const struct Fileinf* text );

void getFilename( struct Fileinf* text );

void fscanData( FILE* fp, char* strData[], const size_t rows );

void fscanData( FILE* fp, char* strData[] );

void freeData( const char** strData );

void getFileSize( struct Fileinf* text );

void fileToBuff( struct Fileinf* text );

size_t parseBuff( struct Fileinf* text );

void freeFileinf( struct Fileinf* text );

void fillFileinf( struct Fileinf* text );

void printFileinfBuff( struct Fileinf* text );

size_t getNlines( struct Fileinf* text );

#endif
