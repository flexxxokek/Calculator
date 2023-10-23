#include "disassembler.h"

static CALC_ERRS TranslateCmnd( FILE* dest, void* src, u_int64_t* fi);

static CALC_ERRS TranslateCmnd( FILE* dest, void* src, u_int64_t* fi )
{
    assert( dest != nullptr );
    assert( src != nullptr );

    int cmnd = CMNDS::INVALID_SYNTAX;

    *( ( char* ) &cmnd ) = *( ( char* ) src + *fi );

    int cmnd_code = cmnd & 0x1f;

    int cmnd_arg = cmnd >> 5;

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
                    StackElem arg = *( StackElem* ) ( ( char* ) src + *fi + 1 );

                    *fi += sizeof( StackElem );

                    fprintf( dest, "%s %lld\n", CMNDS_NAME[1], arg );

                    return CALC_ERRS::OK;
                }

                case 2:
                {
                    int reg_num = 0;

                    reg_num = *( char* ) ( ( char* ) src + *fi + 1 );

                    *fi += sizeof( char );

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

        case CMNDS::HALT:
            fprintf( dest, "%s\n", CMNDS_NAME[7] );

            return CALC_ERRS::OK;

        case CMNDS::POP:
        {
            char arg = 0;

            arg = *( ( char* ) src + ( ++*fi ) );

            fprintf( dest, "%s\n", CMNDS_NAME[8] );

            return CALC_ERRS::OK;
            
        }

        default:
            fprintf( dest, "INVALID_SYNTAX\n" );

            return CALC_ERRS::SYNTAX_ERR;
    }
}

void Disassemble( const char* name, const char* destFileName )
{
    u_int64_t size = getFileSize( name );

    FILE* src = fopen( name, "rb" );

    FILE* dest = fopen( destFileName, "w" );

    u_int64_t ip = 0;

    void* cmnds = calloc( 1, size );

    if( !cmnds )
    {
        perror( "calloc error:" );

        abort();
    }

    fread( cmnds, size, 1, src );

    int cmnd = CMNDS::HALT;

    StackElem arg = 0;

    for( ; ip < size; ip++ )
    {
        *( ( char* ) &cmnd ) = *( ( char* ) cmnds + ip );

        int cmnd_num = cmnd & 0x1f;

        int arg_type = cmnd >> 5;

        printf( "%d %lld\n", cmnd, arg_type );

        TranslateCmnd( dest, cmnds, &ip );
    }
}