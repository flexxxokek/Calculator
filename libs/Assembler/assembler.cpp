#include "assembler.h"

// jump monday, friday, sin, cos, <<, >>, abs.

static u_int64_t MatchLabel( const char* ptr, Label* labels, size_t numOfLabels );

static bool fillUndefLabels( void* voidP, Label* labels, size_t numOfLabels, Label* undefLabels, size_t numOfUndefLabels );

static CALC_ERRS TranslateCmnd( void* p, u_int64_t* ip, Fileinf* src, size_t line, 
                                Label* labels, size_t* numOfLabels, Label* undefLabels, size_t* numOfUndefLabels );

static int ParseLine( const char* ptr, char* cmnd,  StackElem* elem, char strArg[], Label* labels, size_t* numOfLabels, u_int64_t* ip );

static int ParseLine( const char* ptr, char* cmnd,  StackElem* elem, char strArg[], Label* labels, size_t* numOfLabels, u_int64_t* ip )
{
    char num = 0;

    char temp[2] = "";

    if( char* temp = ( char* ) strchr( ptr, '#' ) ) //pitonist poganii. -ito norm.
        *temp = '\0';

    if( ( num = sscanf( ptr, LABEL_SPEC "%1s", strArg, temp ) ) >= 1 )
    {
        if( num == 1 )
        {
            char* p = strchr( strArg, ':' );

            if( p != nullptr )
            {
                printf( "%s : is found %s\n", strArg, p );

                if( *( p + 1 ) == '\0' )
                {
                    for( size_t i = 0; strArg[i] != ':'; i++ )
                    {
                        labels[*numOfLabels].name[i] = strArg[i];
                    }

                    labels[*numOfLabels].ip = *ip;

                    ++*numOfLabels;

                    return 4;
                }
            }
        }

       labels[*numOfLabels].name[0] = '\0';
    }

    if( ( num = sscanf( ptr, CMND_SPECIFICATOR "%1s", cmnd, temp ) ) >= 1 )
    {
        if( num == 1 )
        {            
            return 0;
        }
    }

    if( ( num = sscanf( ptr, CMND_SPECIFICATOR "%lld %1s", cmnd, elem, temp ) ) >= 1 )
    {
        if( num == 2 )
        {
            return 1;
        }
    }

    if( ( num = sscanf( ptr, CMND_SPECIFICATOR " [%lld]%1s" , cmnd, elem, temp ) ) >= 1 )
    {
        if( num == 2 )
            return 5;
    }

    if( ( num = sscanf( ptr, CMND_SPECIFICATOR "%4s %1s", cmnd, strArg, temp ) ) >= 1 )
    {
        if( num == 2 )
        {
            return 2;
        }
    }
    
    if( ( num = sscanf( ptr, CMND_SPECIFICATOR LABEL_SPEC "%1s", cmnd, strArg, temp ) ) >= 1 )
    {
        if( num == 2 )
            return 3;
    }

    return -1;
}

static u_int64_t MatchLabel( const char* ptr, Label* labels, size_t numOfLabels )
{
    for( size_t i = 0; i < numOfLabels; i++ )
    {
        if( !strcmp( ptr, labels[i].name ) )
        {
            return labels[i].ip;
        }
    }

    return -1;
}

static bool fillUndefLabels( void* voidP, Label* labels, size_t numOfLabels, Label* undefLabels, size_t numOfUndefLabels )
{
    char *p = ( char* ) voidP;

    for( size_t i = 0; i < numOfUndefLabels; i++ )
    {
        bool isMet = false;

        for( size_t j = 0; j < numOfLabels; j++ )
        {
            if( !strcmp( undefLabels[i].name, labels[j].name ) )
            {
                printf( "on ip(before): %d, ", p[undefLabels[i].ip] );

                *( StackElem* ) ( p + undefLabels[i].ip ) = labels[j].ip;

                printf( "on ip(after): %d, \"%s\" - %llu,   \"%s\" - %llu\n",
                        p[undefLabels[i].ip], labels[j].name, labels[j].ip, undefLabels[i].name, undefLabels[j].ip );

                isMet = true;

                break;
            }
        }

        if( !isMet )
        {
            return 0;
        }
    }

    return 1;
}

static CALC_ERRS TranslateCmnd( void* p, u_int64_t* ip, Fileinf* src, size_t line, 
                                Label* labels, size_t* numOfLabels, Label* undefLabels, size_t* numOfUndefLabels )
{
    assert( src != nullptr );

    if( *src->strData[line].ptr == '\0' )
        return CALC_ERRS::OK;

    char cmnd[MAX_CMND_LEN] = {};

    StackElem arg = 0;

    char strArg[MAX_LABEL_LEN] = "";

    int type = ParseLine( src->strData[line].ptr, cmnd, &arg ,strArg, labels, numOfLabels, ip );

    puts( src->strData[line].ptr );

    printf( "|->%d\n", type );

    if( !strcmp( cmnd, "in" ) )     //in
    {
        *( ( char* ) p + *ip ) = CMNDS::IN;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "push" ) )     //push
    {
        if( type == 2 )
        {
            int reg_num = strArg[1] - 'a' + 1;

            if( toLow( strArg[0] ) == 'r' && toLow( strArg[2] ) == 'x' && 1 <= reg_num && reg_num <= 4 )
            {
                *( ( char* ) p + *ip ) = 2 << 5 | CMNDS::PUSH;

                *( ( char* ) p + ( ++*ip ) ) = reg_num;

                ++*ip;

                return CALC_ERRS::OK;
            }
            else
            {
                *( ( char* ) p + *ip ) = INVALID_SYNTAX;

                ++*ip;

                return CALC_ERRS::SYNTAX_ERR;
            }
        }
        
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::PUSH;
            
            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            printf( "(%lld)\n", arg );

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }

        else if( type == 5 )
        {
            *( ( char* ) p + *ip ) = 3 << 5 | CMNDS::PUSH;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            printf( "(%lld)\n", arg );

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "add" ) )     //addition
    {
        *( ( char* ) p + *ip ) = CMNDS::ADDITION;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "sub" ) )     //subtraction
    {
        *( ( char* ) p + *ip ) = CMNDS::SUBTRACTION;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "mul" ) )     //multiplication
    {
        *( ( char* ) p + *ip ) = CMNDS::MULTIPLICATION;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "div" ) )     //division
    {
        *( ( char* ) p + *ip ) = CMNDS::DIVISION;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "out" ) )     //out
    {
        *( ( char* ) p + *ip ) = CMNDS::OUT;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "hlt" ) )    //hlt
    {
        *( ( char* ) p + *ip ) = CMNDS::HALT;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "pop" ) )    //pop
    {
        if( type == 2 )
        {
            printf( "!!!!!%s\n", strArg );

            int reg_num = strArg[1] - 'a' + 1;

            if( toLow( strArg[0] ) == 'r' && toLow( strArg[2] ) == 'x' && MIN_REG_NUM <= reg_num && reg_num <= MAX_REG_NUM )
            {
                *( ( char* ) p + *ip ) =  2 << 5 | CMNDS::POP;

                ++*ip;

                *( ( char* ) p + *ip ) = reg_num;

                ++*ip;

                return CALC_ERRS::OK;
            }
            else
            {
                *( ( char* ) p + *ip ) = CMNDS::INVALID_SYNTAX;

                return CALC_ERRS::SYNTAX_ERR;
            }
        }
        else if( type == 5 )
        {
            *( ( char* ) p + *ip ) = 3 << 5 | CMNDS::POP;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            printf( "(%lld)\n", arg );

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "ja" ) )     //Jump if above( a > b )
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_ABOVE;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_ABOVE;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "jae" ) )     //Jump if above or equal( a >= b )
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_ABOVE_OR_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_ABOVE_OR_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "jb" ) )     //Jump if below( a < b )
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_BELOW;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_BELOW;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "jab" ) )     //Jump if below or equal( a <= b )
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_BELOW_OR_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_BELOW_OR_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "je" ) )     //Jump if equal( a == b )
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "jne" ) )     //Jump if not equal( a != b )
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_NOT_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_NOT_EQUAL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "jmp" ) )     //Jump
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "call" ) )     //Call
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                printf( "%d\n", undefLabels[*numOfUndefLabels].ip = *ip + 1 );

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::CALL;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "ret" ) )     //ret
    {
        *( ( char* ) p + *ip ) = CMNDS::RETURN;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "sqrt" ) )     //ret
    {
        *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::SQUARE_ROOT;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "jm" ) )     //jm
    {
        if( type == 3 || type == 2 )
        {
            u_int64_t label_ip = MatchLabel( strArg, labels, *numOfLabels );

            printf( "label ip: %d\n", label_ip );

            if( label_ip == -1 )
            {
                for( size_t i = 0; strArg[i] != '\0'; i++ )
                {
                    undefLabels[*numOfUndefLabels].name[i] = strArg[i];

                    printf( "%c", undefLabels[*numOfUndefLabels].name[i] );
                }

                putchar( '\n' );

                undefLabels[*numOfUndefLabels].ip = *ip + 1;

                ++*numOfUndefLabels;
            }
            
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_MONDAYS;

            *( StackElem* ) ( ( char *) p + ++*ip ) = label_ip;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
        else if( type == 1 )
        {
            *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::JUMP_MONDAYS;

            *( StackElem* ) ( ( char *) p + ++*ip ) = arg;

            *ip += sizeof( StackElem );

            return CALC_ERRS::OK;
        }
    }

    if( !strcmp( cmnd, "sin" ) )
    {
        *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::SIN;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "cos" ) )
    {
        *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::SIN;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "bitl" ) )
    {
        *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::BITL;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "bitr" ) )
    {
        *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::BITR;

        ++*ip;

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, "abs" ) )
    {
        *( ( char* ) p + *ip ) = 1 << 5 | CMNDS::ABS;

        ++*ip;

        return CALC_ERRS::OK;
    }
    
    *( ( char* ) p + *ip ) = CMNDS::INVALID_SYNTAX;

    return CALC_ERRS::SYNTAX_ERR;
}

void Assemble( const char* destFileName )
{
    Fileinf src = {};

    fillFileinf( &src );

    FILE* dest = fopen( destFileName, "wb" );

    void* p = calloc( 1, src.trueSize );

    u_int64_t ip = 0;

    Label labels[8] = {};

    size_t numOfLabels = 0;

    Label undefLabels[8] = {};

    size_t numOfUndefLabels = 0;

    if( p == nullptr )
    {
        perror( "calloc error" );

        return;
    }

    for( size_t line = 0; line < src.nlines; line++ )
    {
        TranslateCmnd( p, &ip, &src, line, labels, &numOfLabels, undefLabels, &numOfUndefLabels );
    }

    for( size_t i = 0; i < numOfLabels; i++ )
    {
        printf( "num of labels: %d -> label number %d: \"%s\" ip: %llu\n", numOfLabels, i, labels[i].name, labels[i].ip );
    }
    
    for( size_t i = 0; i < numOfUndefLabels; i++ )
    {
        printf( "num of undefined labels: %d -> label number %d: \"%s\" ip: %llu\n", numOfUndefLabels, i, undefLabels[i].name, undefLabels[i].ip );
    }

    bool isAllLabels = fillUndefLabels( p, labels, numOfLabels, undefLabels, numOfUndefLabels );

    printf( "true or false: %d\n", isAllLabels );

    if( !isAllLabels )
        return;

    printf( "!!!!!!!!!!!!!!!!!!!!!!!\n" );

    for( size_t i = 0; i < numOfLabels; i++ )
    {
        printf( "num of labels: %d -> label number %d: \"%s\" ip: %llu\n", numOfLabels, i, labels[i].name, labels[i].ip );
    }
    
    for( size_t i = 0; i < numOfUndefLabels; i++ )
    {
        printf( "num of undefined labels: %d -> label number %d: \"%s\" ip: %llu\n", numOfUndefLabels, i, undefLabels[i].name, undefLabels[i].ip );
    }

    fwrite( p, ip, 1, dest );

    fclose( dest );
}