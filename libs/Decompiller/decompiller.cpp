#include "decompiller.h"

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line );

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line )
{
    assert( dest != nullptr );
    assert( src != nullptr );

    int cmnd = CMNDS::INVALID_SYNTAX;

    sscanf( src->strData[line].ptr, "%d", &cmnd );

    int cmnd_code = cmnd & 0xf;

    int cmnd_arg = cmnd >> 4;

    switch ( cmnd_code )
    {
        case CMNDS::IN:
            fprintf( dest, "%s\n", CMNDS_NAME[0] );

            return CALC_ERRS::OK;

        case CMNDS::PUSH:
        {
            switch( cmnd_arg )
            {
                case 1:
                {
                    StackElem num = 0;

                    if( sscanf( src->strData[line].ptr + 2, "%lld", &num ) != 1 )
                    {
                        fprintf( dest, "INVALID_SYNTAX\n" );

                        return CALC_ERRS::SYNTAX_ERR;
                    }

                    fprintf( dest, "%s %lld\n", CMNDS_NAME[1], num );

                    return CALC_ERRS::OK;
                }

                case 2:
                {
                    int reg_num = 0;

                    if( sscanf( src->strData[line].ptr + 2, "%d", &reg_num ) != 1 )
                    {
                        fprintf( dest, "INVALID_SYNTAX\n" );

                        return CALC_ERRS::SYNTAX_ERR;
                    }

                    if( 4 < reg_num || reg_num < 1 )
                    {
                        fprintf( dest, "INVALID_SYNTAX\n" );

                        return CALC_ERRS::SYNTAX_ERR;
                    }

                    fprintf( dest, "%s r%cx\n", CMNDS_NAME[1], 'a' );

                    return CALC_ERRS::OK;
                }

                default:
                    fprintf( dest, "INVALID_SYNTAX\n" );

                    return CALC_ERRS::SYNTAX_ERR;
            }
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