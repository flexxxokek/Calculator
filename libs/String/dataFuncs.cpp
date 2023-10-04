#include "dataFuncs.h"

static FILEINF_ERRORS FileinfVerify( Fileinf* text )
{
    if( text->filename == nullptr ) return FILE_NAME_IS_NULL;
    if( text->buff == nullptr ) return FILE_BUFF_IS_NULL;
    if( text->size == 0 ) return FILE_SIZE_IS_NULL;
    if( text->strData == nullptr ) return FILE_STRDATA_IS_NULL;

    return FILEINF_ALLRIGHT;
}

static void* pushVoidPtr( void* ptr, size_t bytes )
{
    return ( void* )( ( ( char* ) ptr ) + bytes );
}

void printData( const struct Fileinf* text )
{
    assert( text->strData != NULL );
    
    for( size_t i = 0; i < text->nlines; i++ )
    {
        printf( "%zu ", text->strData[i].len );
    
        MyPuts( text->strData[i].ptr );
    }
}

void getFilename( struct Fileinf* text )
{
    printf( "Enter file name: " );

    fgets( stdin, text->filename );
}               

void fscanData( FILE* fp, char* strData[], const size_t rows )
{
    assert( fp != NULL );
    assert( strData != NULL );

    char buffer[MAX_STR_LENGTH] = {};

    for( size_t i = 0; i < rows; i++ )
    {
        size_t len = fgets( fp, buffer );

        strData[i] = ( char* ) calloc( MyStrlen( buffer ) + 1, sizeof( char ) );

        printf( "%zu", len );

        MyStrncpy( strData[i], buffer, len + 1 );
    }
}

void fscanData( FILE* fp, char* strData[] )
{
    assert( fp != NULL );
    assert( strData != NULL );

    size_t rows = 0;

    fscanf( fp, "%zu", &rows );

    fscanData( fp, strData, rows );
}

void freeData( const char ** strData )
{
    assert( strData != NULL );

    for( int i = 0; i < MAX_STR_NUM; i++ )
    {
        free( ( void* ) *strData );

        *strData = nullptr;

        strData++;
    }
}

static int checkFilePointer( FILE* fp, int* err = 0 )
{
    if( fp == NULL )
    {
        *err = FILE_POINTER_IS_NULL;

        printf( "File pointer is NULL" );

        return FILE_POINTER_IS_NULL;
    }

    return 0;
}

void getFileSize( struct Fileinf* text )
{   
    struct stat fileStat = {};

    stat( text->filename, &fileStat ); //proverka

    text->size = fileStat.st_size;
}

void fileToBuff( struct Fileinf* text )
{
    if( !text->size )
    {
        printf( "File \"%s\" doesn't exist\n", text->filename );
        
        return;
    }

    FILE* fp = fopen( text->filename, "r" );
    checkFilePointer( fp );

    text->buff = ( char* ) calloc( text->size + 1, sizeof( char ) );

    text->trueSize = fread( text->buff, sizeof( char ), text->size / sizeof( char ), fp );

    if( text->trueSize == 0 )
    {
        printf( "File wasn't read successfully!\n" );

        return;
    }
}

size_t parseBuff( struct Fileinf* text )
{
    text->strData[0].ptr = text->buff;

    int nlines = 1;

    for( size_t i = 0; i < text->trueSize; i++ )
    {
        if( text->buff[i] == '\n' )
        {
            text->strData[nlines - 1].len = ( size_t )( text->buff + i - text->strData[nlines - 1].ptr );

            text->strData[nlines].ptr = text->buff + i + 1;

            nlines++;

            text->buff[i] = '\0';
        }
    }

    text->strData[nlines - 1].len = ( size_t )( text->buff + text->trueSize - text->strData[nlines - 1].ptr );

    return text->trueSize;
}

void freeFileinf( struct Fileinf* text )
{
    free( text->buff );
    free( text->strData );
}

void fillFileinf( struct Fileinf* text )
{
    getFilename( text );

    getFileSize( text );    

    fileToBuff( text );

    getNlines( text );

    text->strData = ( struct String* ) calloc( text->nlines, sizeof( String ) );

    parseBuff( text );
}

void printFileinfBuff( struct Fileinf* text )
{
    for( size_t i = 0, currPos = 0; i < text->nlines; i++ )
    {
        currPos += MyPuts( text->buff + currPos ) + 1;
    }
}

size_t getNlines( struct Fileinf* text )
{
    size_t nlines = 1;

    for( size_t i = 0; i < text->trueSize; i++ )
        if( text->buff[i] == '\n' )
            nlines++;

    text->nlines = nlines;
    
    return nlines;
}
