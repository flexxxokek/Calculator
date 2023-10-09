#include "calc.h"

const char* CMNDS_NAME[NUM_OF_CMNDS] = { "in",
                                         "push",
                                         "add",
                                         "sub",
                                         "mul",
                                         "div",
                                         "out",
                                         "hlt",
                                         "pop"  };

static StackElem PopToReg( SPU* spu, int reg_num );

static StackElem PushFromReg( SPU* spu, int reg_num );

static void PrintCommands( Fileinf* cmnds, SPU* spu );

static void ParseBinary( FILE* src );

static void ParseBinary( FILE* src )
{
    
}

static int SPU_Verify( SPU* spu )
{
    int err = 0;

    if( spu == nullptr ) err |= SPU_ERRS::SPU_IS_NULL;

    if( spu->commands == nullptr ) err |= SPU_ERRS::COMMANDS_IS_NULL;

    if( &spu->stk == nullptr ) err |= SPU_ERRS::STACK_IS_NULL;

    if( spu->fi < 0 ) err |= SPU_ERRS::FI_IS_LOWER_THAN_NULL;

    return err;
}

static void PrintCommands( Fileinf* cmnds, SPU* spu )
{
    u_int64_t len = 0;

    char* p = ( char* ) spu->commands;

    char sep = ' ';

    for( int i = 0; i < cmnds->trueSize; p++ )
    {
        if( i == spu->fi )
            sep = '>';

        printf( "%c%d", sep, *p );

        if( *p & 0xf == CMNDS::PUSH || *p & 0xf == CMNDS::POP_REG )
        {
            printf( "%lld", *( StackElem* ) ( p + 1 ) );

            i += sizeof( StackElem );
        }

        putchar( '\n' );

        sep = ' ';

        i++;
    }
}

static void SPU_Dump( SPU* spu )
{
    printf( "SPU\n"
            "{\n"
            "\t" );
}

static StackElem PopToReg( SPU* spu, int reg_num )
{    
    StackElem a = 0;

    switch( reg_num )
    {
        case 1:
            StackPop( &spu->stk, &a );

            spu->rax = a;

            return spu->rax;
        
        case 2:
            StackPop( &spu->stk, &a );

            spu->rbx = a;

            return spu->rbx;
        
        case 3:
            StackPop( &spu->stk, &a );

            spu->rcx = a;

            return spu->rcx;

        case 4:
            StackPop( &spu->stk, &a );

            spu->rdx = a;

            return spu->rdx;

        default:
            return 0xdeaddead;
    }
}

static StackElem PushFromReg( SPU* spu, int reg_num )
{
    switch( reg_num )
    {
        case 1:
            StackPush( &spu->stk, spu->rax );

            return spu->rax;
        
        case 2:
            StackPush( &spu->stk, spu->rbx );

            return spu->rbx;
        
        case 3:
            StackPush( &spu->stk, spu->rcx );

            return spu->rcx;

        case 4:
            StackPush( &spu->stk, spu->rdx );

            return spu->rdx;

        default:
            return 0xdeaddead;
    }
}

void SPU_Ctor( SPU* spu, u_int64_t comandsBuffSize )
{   
    spu->fi = 0;

    spu->commands = calloc( 1, comandsBuffSize );

    if( !spu->commands )
    {
        perror( "calloc error:" );

        abort();
    }

    spu->rax = 0;

    spu->rbx = 0;

    spu->rcx = 0;

    spu->rdx = 0;

    STACK_CTOR( &spu->stk );

    return;
}

CALC_ERRS Calculate( Fileinf* cmnds, SPU* spu )
{
    int cmnd = CMNDS::HALT;

    StackElem arg = 0;

    int num = 0;
    
    char ch = 0;

    size_t line = 0;

    for( ; ( num = sscanf( cmnds->strData[line].ptr, "%d %lld %1s", &cmnd, &arg, &ch ) ) > 0; line++ )
    {
        *( ( char* ) spu->commands + spu->fi++ ) = *( ( char* ) &cmnd );

        int cmnd_num = cmnd & 0xf;

        int arg_type = ( cmnd >> 4 ) & 0xf;

        printf( "%d %d\n", cmnd, arg );

        switch( cmnd_num )
        {
            case CMNDS::IN:
                if( num == 1 )
                {
                    printf( "-------------------------------------------------------------------\n"
                            "Please, enter number: " );

                    if( scanf( "%lld", &arg ) != 1 )
                    {
                        printf( "SYNTAX ERROR in line: %zu\n"
                                "-------------------------------------------------------------------\n\n",
                                line );

                        abort();
                    }

                    StackPush( &spu->stk, arg * MULTIPLYER );

                    printf( "successfully pushed %lld \n"
                            "-------------------------------------------------------------------\n\n", arg );

                    break;
                }
                else
                {
                    printf( "SYNTAX ERROR in line: %zu\n"
                                "-------------------------------------------------------------------\n\n",
                                line );

                    abort();
                }

            case CMNDS::PUSH:
                if( num == 2 )
                {
                    if( arg_type == 1 )     //pushing a number
                    {
                        StackPush( &spu->stk, arg * MULTIPLYER );

                        printf( "-------------------------------------------------------------------\n"
                                "successfully pushed %lld \n"
                                "-------------------------------------------------------------------\n\n", arg );

                        *( ( StackElem* ) ( spu->commands + spu->fi ) ) = arg; //or arg * MULTIPLYER?????????

                        spu->fi += sizeof( StackElem );

                        goto swp_end;
                    }
                    else if( arg_type == 2 )    //pushing the number from register
                    {
                        if( 5 <= arg || arg <= 0 )
                        {
                            printf( "-------------------------------------------------------------------\n"
                                    "SYNTAX ERROR in line: %zu\n"
                                    "-------------------------------------------------------------------\n\n",
                                    line );

                            abort();
                        }

                        StackElem temp = PushFromReg( spu, arg );

                        printf( "-------------------------------------------------------------------\n"
                                "successfully pushed %lld from register r%cx \n"
                                "-------------------------------------------------------------------\n\n",
                                temp, 'a' + ( int ) arg - 1 );

                        *( ( char* ) spu->commands + spu->fi++ ) = *( char* ) &cmnd;

                        goto swp_end;
                    }
                    else
                    {
                        printf( "--------------------------------------------------------------------\n"
                                "SYNTAX ERROR in line: %zu\n"
                                "-------------------------------------------------------------------\n\n",
                                line );

                        abort();
                    }
                }
                else
                {
                    printf( "SYNTAX ERROR in line: %zu\n"
                            "-------------------------------------------------------------------\n\n",
                            line );

                    abort();
                }

            case CMNDS::ADDITION:
            {
                StackElem a = 0;

                StackElem b = 0;

                StackPop( &spu->stk, &a );

                StackPop( &spu->stk, &b );

                StackPush( &spu->stk, b + a );

                printf( "-------------------------------------------------------------------\n"
                        "successfully completed addtion: %lld + %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n", 
                        b, a, b + a );

                goto swp_end;
            }

            case CMNDS::SUBTRACTION:
            {
                StackElem a = 0;

                StackElem b = 0;            

                StackPop( &spu->stk, &a );

                StackPop( &spu->stk, &b );

                StackPush( &spu->stk, b - a );

                printf( "-------------------------------------------------------------------\n"
                        "successfully completed subtraction: %lld - %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n",
                        b, a, b - a );

                goto swp_end;
            }

            case CMNDS::MULTIPLICATION:
            {
                StackElem a = 0;

                StackElem b = 0;

                StackPop( &spu->stk, &a );

                StackPop( &spu->stk, &b );

                StackPush( &spu->stk, a * b / MULTIPLYER );

                printf( "-------------------------------------------------------------------\n"
                        "successfully completed multiplication: %lld * %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n",
                        b, a, a * b / MULTIPLYER );

                goto swp_end;
            }

            case CMNDS::DIVISION:
            {
                StackElem a = 0;

                StackElem b = 0;
            
                StackPop( &spu->stk, &a );

                StackPop( &spu->stk, &b );

                if( a == 0 )
                {
                    printf( "-------------------------------------------------------------------\n"
                            "error: division by zero, line: %zu\n"
                            "-------------------------------------------------------------------\n\n" );

                    goto end;
                }

                StackPush( &spu->stk, ( b * MULTIPLYER ) / a );

                printf( "-------------------------------------------------------------------\n"
                        "successfully completed division: %lld / %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n", 
                        b, a, ( b * MULTIPLYER ) / a );

                goto swp_end;
            }

            case CMNDS::OUT:
            {
                StackElem a = 0;

                StackPop( &spu->stk, &a );

                double res = ( double ) a / MULTIPLYER;
                
                printf( ">------------------------------------------------------------------\n"
                        "result of calculation: %g\n"
                        ">------------------------------------------------------------------\n\n",
                        res);

                goto swp_end;
            }

            case CMNDS::HALT:
                printf( "x-----------------------------------------------------------------\n"
                        "HLT in line %zu, running away...\n"
                        "x-----------------------------------------------------------------\n\n",
                        line );
                
                goto end;

            case CMNDS::POP_REG:
            {
                StackElem temp = PopToReg( spu, arg );

                printf( "------------------------------------------------------------------\n"
                        "successfully poped %lld to register r%cx\n"
                        "------------------------------------------------------------------\n\n",
                        temp, arg + 'a' - 1 );

                *( ( char* ) spu->commands + spu->fi++ ) = *( char* ) &arg;

                goto swp_end;
            }

            default:
                printf( "-------------------------------------------------------------------\n"
                        "SYNTAX ERROR in line: %zu\n"
                        "-------------------------------------------------------------------\n\n",
                        line );

                return CALC_ERRS::SYNTAX_ERR;

            swp_end:
                break;
        }

        arg = 0;
    }

    end:
        /*for( size_t i = 0; i < spu->fi; i++ )
        {
            printf( "%zu: 0X: %X dec: %d\n", i, *( ( unsigned char* ) spu->commands + i ), *( ( unsigned char* ) spu->commands + i ) );
        }*/

        PrintCommands( cmnds, spu );

        return CALC_ERRS::OK;
}
