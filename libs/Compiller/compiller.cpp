#include "compiller.h"

static int ScanCmnd( Fileinf* src, size_t line, char* cmnd );

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line );

static int ScanCmnd( Fileinf* src, size_t line, char* cmnd )
{
    return sscanf( src->strData[line].ptr, CMND_SPECIFICATOR, cmnd );
}

static CALC_ERRS TranslateCmnd( FILE* dest, Fileinf* src, size_t line )
{
    assert( dest != nullptr );
    assert( src != nullptr );

    char cmnd[MAX_CMND_LEN] = {};

    if( ScanCmnd( src, line, cmnd ) != 1 )
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
            char reg[4] = {};

            if( sscanf( src->strData[line].ptr + 4, "%3s", reg ) != 1 )
            {
                fprintf( dest, "%d\n", CMNDS::INVALID_SYNTAX );

                return CALC_ERRS::SYNTAX_ERR;
            }

            int reg_num = reg[1] - 'a' + 1;

            if( toLow( reg[0] ) == 'r' && toLow( reg[2] ) == 'x' && 1 <= reg_num && reg_num <= 4 )
            {
                fprintf( dest, "%d %d\n", 2 << 4 | CMNDS::PUSH, reg_num );

                return CALC_ERRS::OK;
            }
            else
            {
                fprintf( dest, "%d\n", CMNDS::INVALID_SYNTAX );

                return CALC_ERRS::SYNTAX_ERR;
            }
            
        }
        
        fprintf( dest, "%d %lld\n", 1 << 4 | CMNDS::PUSH, num );

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

    if( !strcmp( cmnd, CMNDS_NAME[7] ) )    //hlt
    {
        fprintf( dest, "%d\n", CMNDS::HALT );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[8] ) )    //pop
    {
        char reg[4] = {};

        if( sscanf( src->strData[line].ptr + 4, "%3s", reg ) != 1 )
            {
                fprintf( dest, "%d\n", CMNDS::INVALID_SYNTAX );

                return CALC_ERRS::SYNTAX_ERR;
            }

            int reg_num = reg[1] - 'a' + 1;

            if( toLow( reg[0] ) == 'r' && toLow( reg[2] ) == 'x' && 1 <= reg_num && reg_num <= 4 )
            {
                fprintf( dest, "%d %d\n", 2 << 4 | CMNDS::POP_REG, reg_num );

                return CALC_ERRS::OK;
            }
            else
            {
                fprintf( dest, "%d\n", CMNDS::INVALID_SYNTAX );

                return CALC_ERRS::SYNTAX_ERR;
            }
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