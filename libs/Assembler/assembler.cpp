#include "assembler.h"

static int ScanCmnd( Fileinf* src, size_t line, char* cmnd );

static CALC_ERRS TranslateCmnd( void* p, size_t* fi, Fileinf* src, size_t line );

static int ScanCmnd( Fileinf* src, size_t line, char* cmnd )
{
    return sscanf( src->strData[line].ptr, CMND_SPECIFICATOR, cmnd );
}

static CALC_ERRS TranslateCmnd( void* p, size_t* fi, Fileinf* src, size_t line )
{
    assert( src != nullptr );

    char cmnd[MAX_CMND_LEN] = {};

    if( ScanCmnd( src, line, cmnd ) != 1 )
        return CALC_ERRS::SYNTAX_ERR;

    printf( "%s\n", cmnd );

    if( !strcmp( cmnd, CMNDS_NAME[0] ) )     //in
    {
        *( ( char* ) p + *fi ) = CMNDS::IN;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[1] ) )     //push
    {
        StackElem arg = 0;

        if( sscanf( src->strData[line].ptr + 4, "%lld", &arg ) != 1 )
        {
            char reg[4] = {};

            if( sscanf( src->strData[line].ptr + 4, "%3s", reg ) != 1 )
            {
                *( ( char* ) p + *fi ) = CMNDS::INVALID_SYNTAX;

                return CALC_ERRS::SYNTAX_ERR;
            }

            int reg_num = reg[1] - 'a' + 1;

            if( toLow( reg[0] ) == 'r' && toLow( reg[2] ) == 'x' && 1 <= reg_num && reg_num <= 4 )
            {
                *( ( char* ) p + *fi ) = 2 << 4 | CMNDS::PUSH;

                *( ( char* ) p + ( ++*fi ) ) = reg_num;

                return CALC_ERRS::OK;
            }
            else
            {
                *( ( char* ) p + *fi ) = INVALID_SYNTAX;

                return CALC_ERRS::SYNTAX_ERR;
            }
            
        }
        
        //fprintf( dest, "%d %lld\n", 1 << 4 | CMNDS::PUSH, num );

        *( ( char* ) p + *fi ) = 1 << 4 | CMNDS::PUSH;
        
        *( StackElem* ) ( ( char *) p + *fi  + 1 ) = arg;

        *fi += sizeof( StackElem );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[2] ) )     //addition
    {
        *( ( char* ) p + *fi ) = CMNDS::ADDITION;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[3] ) )     //subtraction
    {
        *( ( char* ) p + *fi ) = CMNDS::SUBTRACTION;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[4] ) )     //multiplication
    {
        *( ( char* ) p + *fi ) = CMNDS::MULTIPLICATION;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[5] ) )     //division
    {
        *( ( char* ) p + *fi ) = CMNDS::DIVISION;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[6] ) )     //out
    {
        *( ( char* ) p + *fi ) = CMNDS::OUT;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[7] ) )    //hlt
    {
        *( ( char* ) p + *fi ) = CMNDS::HALT;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS_NAME[8] ) )    //pop
    {
        char reg[4] = {};

        if( sscanf( src->strData[line].ptr + 4, "%3s", reg ) != 1 )
            {
                *( ( char* ) p + *fi ) = CMNDS::INVALID_SYNTAX;

                return CALC_ERRS::SYNTAX_ERR;
            }

            int reg_num = reg[1] - 'a' + 1;

            if( toLow( reg[0] ) == 'r' && toLow( reg[2] ) == 'x' && 1 <= reg_num && reg_num <= 4 )
            {
                *( ( char* ) p + *fi ) =  2 << 4 | CMNDS::POP_REG;

                *( ( char* ) p + ( ++*fi ) ) = reg_num;

                return CALC_ERRS::OK;
            }
            else
            {
                *( ( char* ) p + *fi ) = CMNDS::INVALID_SYNTAX;

                return CALC_ERRS::SYNTAX_ERR;
            }
    }

    *( ( char* ) p + *fi ) = CMNDS::INVALID_SYNTAX;

    return CALC_ERRS::SYNTAX_ERR;
}

void Assemble( const char* destFileName )
{
    Fileinf src = {};

    fillFileinf( &src );

    FILE* dest = fopen( destFileName, "wb" );

    void* p = calloc( 1, src.trueSize );

    u_int64_t fi = 0;

    if( p == nullptr )
    {
        perror( "calloc error" );

        return;
    }

    for( size_t line = 0; line < src.nlines; line++, fi++ )
    {
        TranslateCmnd( p, &fi, &src, line );
    }

    fwrite( p, 1, fi, dest );
}