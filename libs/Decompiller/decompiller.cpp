#include "decompiller.h"

const char* CMNDS_NAME[NUM_OF_CMNDS] = { "in",
                                         "push",
                                         "add",
                                         "sub",
                                         "mul",
                                         "div",
                                         "out"  };

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line );

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line )
{
    assert( dest != nullptr );
    assert( src != nullptr );

    CMNDS cmnd = CMNDS::INVALID_SYNTAX;

    sscanf( src->strData[line].ptr, "%d", &cmnd );

    switch ( cmnd )
    {
        case CMNDS::IN:
            fprintf( dest, "%s\n", CMNDS_NAME[0] );

            return CALC_ERRS::OK;

        case CMNDS::PUSH:
        {
            StackElem num = 0;

            if( sscanf( src->strData[line].ptr + 1, "%lld", &num ) != 1 )
            {
                fprintf( dest, "INVALID_SYNTAX\n" );

                return CALC_ERRS::SYNTAX_ERR;
            }

            fprintf( dest, "%s %lld\n", CMNDS_NAME[1], num );

            return CALC_ERRS::OK;
        }

        case CMNDS::ADDITION:
            fprintf( dest, "%s\n", CMNDS_NAME[2] );

            return CALC_ERRS::OK;

        case CMNDS::SUBTRACTION:
            fprintf( dest, "%s\n", CMNDS_NAME[3] );

            return CALC_ERRS::OK;

        case CMNDS::MULTIPLICATION:
            fprintf( dest, "%s\n", CMNDS_NAME[4] );

            return CALC_ERRS::OK;

        case CMNDS::DIVISION:
            fprintf( dest, "%s\n", CMNDS_NAME[5] );

            return CALC_ERRS::OK;

        case CMNDS::OUT:
            fprintf( dest, "%s\n", CMNDS_NAME[6] );

            return CALC_ERRS::OK;

        default:
            fprintf( dest, "INVALID_SYNTAX\n" );

            return CALC_ERRS::SYNTAX_ERR;
    }
}

void Decompile( const char* destFileName )
{
    Fileinf src = {};

    fillFileinf( &src );

    FILE* dest = fopen( destFileName, "w+" );

    for( size_t line = 0; line < src.nlines; line++ )
    {
        TranslateCmnd( dest, &src, line );
    }
}