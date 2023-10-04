#include "compiller.h"

const char* CMNDS_NAME[NUM_OF_CMNDS] = { "in",
                                        "push",
                                        "add",
                                        "sub",
                                        "mul",
                                        "div",
                                        "out"   };

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line );

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line )
{
    assert( dest != nullptr );
    assert( src != nullptr );

    char cmnd[MAX_CMND_LEN] = {};

    if( sscanf( src->strData[line].ptr, "%s", cmnd ) != 1 )
        return CALC_ERRS::SYNTAX_ERR;

    printf( "%s\n", cmnd );

    if( !strcmp( cmnd, CMNDS_NAME[0] ) )     //in
    {
        fprintf( dest, "%d\n", CMNDS::IN );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[1] ) )     //push
    {
        StackElem num = 0;

        if( sscanf( src->strData[line].ptr + 4, "%lld", &num ) != 1 )
        {
            fprintf( dest, "%d\n", CMNDS::INVALID_SYNTAX );

            return CALC_ERRS::SYNTAX_ERR;
        }
            
        fprintf( dest, "%d %lld\n", CMNDS::PUSH, num );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[2] ) )     //addition
    {
        fprintf( dest, "%d\n", CMNDS::ADDITION );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[3] ) )     //subtraction
    {
        fprintf( dest, "%d\n", CMNDS::SUBTRACTION );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[4] ) )     //multiplication
    {
        fprintf( dest, "%d\n", CMNDS::MULTIPLICATION );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[5] ) )     //division
    {
        fprintf( dest, "%d\n", CMNDS::DIVISION );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[6] ) )     //out
    {
        fprintf( dest, "%d\n", CMNDS::OUT );

        return CALC_ERRS::OK;
    }

    fprintf( dest, "%d\n", CMNDS::INVALID_SYNTAX );

    return CALC_ERRS::SYNTAX_ERR;
}

void Compile( const char* destFileName )
{
    Fileinf src = {};

    fillFileinf( &src );

    FILE* dest = fopen( destFileName, "w+" );

    for( size_t line = 0; line < src.nlines; line++ )
    {
        TranslateCmnd( dest, &src, line );
    }
}