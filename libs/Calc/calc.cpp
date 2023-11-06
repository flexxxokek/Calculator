#include "calc.h"

static StackElem PopToReg( SPU* spu, int reg_num );

static StackElem PushFromReg( SPU* spu, int reg_num );

static void SPU_Dump( SPU* spu, int64_t size );

static CALC_ERRS DoCommand( SPU* spu, size_t line );

static int SPU_Verify( SPU* spu )
{
    int err = 0;

    if( spu == nullptr )            return err |= SPU_ERRS::SPU_IS_NULL;

    if( spu->commands == nullptr )  err |= SPU_ERRS::COMMANDS_IS_NULL;

    if( StackVerify( &spu->stk ) )  err |= SPU_ERRS::STACK_IS_CURSED;

    return err;
}

static void PrintCommands( SPU* spu, int64_t size )
{
    printf( "\tcommands array:\n");

    printf( "\t{\n" );

    u_int64_t len = 0;

    char* p = ( char* ) spu->commands;

    const char* sep = " ";

    for( int64_t i = 0; i < size; p++, i++ )
    {
        char cmnd_num = *p & 0x1f;

        char arg_type = *p >> 5;    

        if( ( u_int64_t ) i == spu->ip )
        {
            sep = GREEN_COLOR ">";
        }

        if( ( cmnd_num == CMNDS::POP || ( cmnd_num == CMNDS::PUSH && arg_type == 2 ) ) && ( u_int64_t ) i == spu->ip - sizeof( char ) - 1 )
        {
            sep = GREEN_COLOR ">";
        }

        if( ( cmnd_num == CMNDS::PUSH || cmnd_num == CMNDS::POP ||
              cmnd_num == CMNDS::JUMP || cmnd_num == CMNDS::JUMP_ABOVE || cmnd_num == CMNDS::JUMP_ABOVE_OR_EQUAL ||
              cmnd_num == CMNDS::JUMP_BELOW || cmnd_num == CMNDS::JUMP_BELOW_OR_EQUAL ||
              cmnd_num == CMNDS::JUMP_EQUAL || cmnd_num == CMNDS::JUMP_NOT_EQUAL || cmnd_num == CMNDS::CALL
              && ( arg_type == 1 ) ) && i == spu->ip - sizeof( StackElem ) - 1 )
        {
            sep = GREEN_COLOR ">";
        }

        printf( "\t%6scommand: %02X %01X", sep, cmnd_num, arg_type );

        if( ( cmnd_num == CMNDS::PUSH || cmnd_num == CMNDS::POP ||
              cmnd_num == CMNDS::JUMP || cmnd_num == CMNDS::JUMP_ABOVE || cmnd_num == CMNDS::JUMP_ABOVE_OR_EQUAL ||
              cmnd_num == CMNDS::JUMP_BELOW || cmnd_num == CMNDS::JUMP_BELOW_OR_EQUAL ||
              cmnd_num == CMNDS::JUMP_EQUAL || cmnd_num == CMNDS::JUMP_NOT_EQUAL || cmnd_num == CMNDS::CALL ) && ( arg_type == 1 || arg_type == 3 ) )
        {
            printf( "  arg: %lld", *( StackElem* ) ( p + 1 ) );

            p += sizeof( StackElem );

            i += sizeof( StackElem );
        }

        if( ( cmnd_num == CMNDS::POP || cmnd_num == CMNDS::PUSH ) && arg_type == 2 )
        {
            printf( "  arg: %d", *( char* ) ( p + 1 ) );

            p += sizeof( char );

            i += sizeof( char );
        }

        putchar( '\n' );

        SET_DEFAULT_COLOR;

        sep = " ";
    }

    printf( "\t}\n" );
}

static void SPU_Dump( SPU* spu, int64_t size )
{
    printf( "SPU\n"
            "{\n" );

    printf( "\tfi = %llu\n", spu->ip );
    
    PrintCommands( spu, size );

    printf( "\trax = %lld\n" 
            "\trbx = %lld\n"
            "\trcx = %lld\n"
            "\trdx = %lld\n",
            spu->regs.names.rax, spu->regs.names.rbx, spu->regs.names.rcx, spu->regs.names.rdx );

    printf( "}\n" );
}

static StackElem PopToReg( SPU* spu, int reg_num )
{    
    StackElem a = 0;

    if( reg_num < MIN_REG_NUM || reg_num > MAX_REG_NUM )
    {
        return DEAD_INT;
    }

    StackPop( &spu->stk, &a );

    spu->regs.buff[reg_num - 1] = a;

    return a;
}

static StackElem PushFromReg( SPU* spu, int reg_num )
{
    if( reg_num < MIN_REG_NUM || reg_num > MAX_REG_NUM )
    {
        return DEAD_INT;
    }

    StackPush( &spu->stk, spu->regs.buff[reg_num - 1] );

    return spu->regs.buff[reg_num - 1];
}

static void SPU_Ctor( SPU* spu, int64_t comandsBuffSize )
{   
    spu->ip = 0;

    spu->commands = calloc( 1, comandsBuffSize );

    if( !spu->commands )
    {
        perror( "Lack of memory for spu->commands" );

        abort();
    }

    spu->RAM = ( StackElem* ) calloc( NUM_OF_RAM_ELEMS, sizeof( StackElem ) );

    if( !spu->RAM )
    {
        free( spu->commands );

        perror( "Lack of memory for spu->RAM" );

        abort();
    }

    for( unsigned long long i = 0; i < sizeof( spu->regs ) / sizeof( StackElem ); i++ )
        spu->regs.buff[i] = 0;

    for( unsigned long long i = 0; i < NUM_OF_RAM_ELEMS; i++ )
        spu->RAM[i] = 0;

    STACK_CTOR( &spu->stk );

    return;
}

static void SPU_Dtor( SPU* spu, u_int64_t comandsBuffSize )
{   
    spu->ip = 0;

    if( !spu->commands )
    {
        free( spu->commands );
    }

    spu->commands = nullptr;

    for( int i = 0; i < sizeof( spu->regs ) / sizeof( StackElem ); i++ )
        spu->regs.buff[i] = 0 ;

    return;
}

#define _COMMAND_( name, num, args, code )              \
code

static CALC_ERRS DoCommand( SPU* spu, size_t line )
{   
    StackElem arg = 0;

    int cmnd = 0;

    *( ( char* ) &cmnd ) = *( ( char* ) spu->commands + spu->ip );

    ++spu->ip;

    int cmnd_num = cmnd & 0x1f;

    int arg_type = cmnd >> 5;

    printf( "%d %lld\n", cmnd, arg_type );

    switch( cmnd_num )
    {
        case CMNDS::IN:
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

        case CMNDS::PUSH:
            if( arg_type == 1 )     //pushing a number
            {
                memcpy( &arg, ( char* ) spu->commands + spu->ip, sizeof( StackElem ) );

                spu->ip += sizeof( StackElem );

                StackPush( &spu->stk, arg * MULTIPLYER );

                printf( "-------------------------------------------------------------------\n"
                        "successfully pushed %lld \n"
                        "-------------------------------------------------------------------\n\n", arg );

                break;
            }
            else if( arg_type == 2 )    //pushing the number from register
            {
                arg = *( ( char* ) spu->commands + spu->ip );
                
                ++spu->ip;

                if( arg < MIN_REG_NUM || arg > MAX_REG_NUM )
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

                break;
            }
            else if( arg_type == 3 )    //pushing number from RAM
            {
                memcpy( &arg, ( char* ) spu->commands + spu->ip, sizeof( StackElem ) );

                spu->ip += sizeof( StackElem );

                printf( "ip: %lld\n", spu->ip );

                StackPush( &spu->stk, spu->RAM[arg] );

                printf( "-------------------------------------------------------------------\n"
                        "successfully pushed %lld from RAM on position %lld ip:%lld\n"
                        "-------------------------------------------------------------------\n\n",
                        spu->RAM[arg], arg, spu->ip );

                break;
            }
            else
            {
                printf( "--------------------------------------------------------------------\n"
                        "SYNTAX ERROR in line: %zu\n"
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

            break;
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

            break;
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

            break;
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

                return CALC_ERRS::SYNTAX_ERR;
            }

            StackPush( &spu->stk, ( b * MULTIPLYER ) / a );

            printf( "-------------------------------------------------------------------\n"
                    "successfully completed division: %lld / %lld = %lld \n"
                    "-------------------------------------------------------------------\n\n", 
                    b, a, ( b * MULTIPLYER ) / a );

            break;
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

            break;
        }

        case CMNDS::HALT:
            printf( "x-----------------------------------------------------------------\n"
                    "HLT in line %zu, running away...\n"
                    "x-----------------------------------------------------------------\n\n",
                    line );
            
            abort();

            return CALC_ERRS::OK;

        case CMNDS::POP:
        {
            if( arg_type == 2 )
            {
                arg = *( ( char* ) spu->commands + spu->ip );

                ++spu->ip;

                StackElem temp = PopToReg( spu, arg );

                printf( "------------------------------------------------------------------\n"
                        "successfully poped %lld to register r%cx\n"
                        "------------------------------------------------------------------\n\n",
                        temp, arg + 'a' - 1 );

                break;
            }
            else if( arg_type == 3 )
            {
                memcpy( &arg, ( char* ) spu->commands + spu->ip, sizeof( StackElem ) );

                spu->ip += sizeof( int64_t );

                printf( "ip: %lld\n", spu->ip );

                StackElem a = 0;

                StackPop( &spu->stk, &a );

                spu->RAM[arg] = a;

                printf( "-------------------------------------------------------------------\n"
                        "successfully pushed %lld from RAM on position %lld ip:%lld\n"
                        "-------------------------------------------------------------------\n\n",
                        spu->RAM[arg], arg, spu->ip );

                break;
            }

            return CALC_ERRS::SYNTAX_ERR;
        }

        case CMNDS::JUMP_ABOVE:             //Jump if a > b
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackElem a = 0;

            StackElem b = 0;

            StackPop( &spu->stk, &a );

            StackPop( &spu->stk, &b );

            if( a > b )
            {
                printf( "-------------------------------------------------------------------\n"
                        "JA %lld > %lld successfully jumped to ip -> %llu \n"
                        "-------------------------------------------------------------------\n\n",
                        a, b, spu->ip );

                spu->ip = arg;
            }

            break;
        }

        case CMNDS::JUMP_ABOVE_OR_EQUAL:             //Jump if a >= b
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackElem a = 0;

            StackElem b = 0;

            StackPop( &spu->stk, &a );

            StackPop( &spu->stk, &b );

            if( a >= b )
            {
                printf( "-------------------------------------------------------------------\n"
                        "JAE %lld >= %lld successfully jumped to ip -> %llu \n"
                        "-------------------------------------------------------------------\n\n",
                        a, b, spu->ip );
            
                spu->ip = arg;
            }
            break;
        }

        case CMNDS::JUMP_BELOW:             //Jump if a < b
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackElem a = 0;

            StackElem b = 0;

            StackPop( &spu->stk, &a );

            StackPop( &spu->stk, &b );

            if( a < b )
            {
                spu->ip = arg;

                printf( "-------------------------------------------------------------------\n"
                        "JB %lld < %lld successfully jumped to ip -> %llu \n"
                        "-------------------------------------------------------------------\n\n",
                        a, b, spu->ip );
            }

            break;
        }

        case CMNDS::JUMP_BELOW_OR_EQUAL:             //Jump if a <= b
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackElem a = 0;

            StackElem b = 0;

            StackPop( &spu->stk, &a );

            StackPop( &spu->stk, &b );

            if( a <= b )
            {
                spu->ip = arg;

                printf( "-------------------------------------------------------------------\n"
                        "JBE %lld <= %lld successfully jumped to ip -> %llu \n"
                        "-------------------------------------------------------------------\n\n",
                        a, b, spu->ip );
            }

            break;
        }

        case CMNDS::JUMP_EQUAL:             //Jump if a == b
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackElem a = 0;

            StackElem b = 0;

            StackPop( &spu->stk, &a );

            StackPop( &spu->stk, &b );

            if( a == b )
            {
                spu->ip = arg;

                printf( "-------------------------------------------------------------------\n"
                        "JE %lld == %lld successfully jumped to ip -> %llu \n"
                        "-------------------------------------------------------------------\n\n",
                        a, b, spu->ip );
            }

            break;
        }

        case CMNDS::JUMP_NOT_EQUAL:             //Jump if a != b
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackElem a = 0;

            StackElem b = 0;

            StackPop( &spu->stk, &a );

            StackPop( &spu->stk, &b );

            if( a != b )
            {
                spu->ip = arg;

                printf( "-------------------------------------------------------------------\n"
                        "JNE %lld != %lld successfully jumped to ip -> %lu \n"
                        "-------------------------------------------------------------------\n\n",
                        a, b, spu->ip );
            }

            break;
        }

        case CMNDS::JUMP:             //Jump
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            spu->ip = arg;

            printf( "-------------------------------------------------------------------\n"
                    "JMP successfully jumped to ip -> %llu \n"
                    "-------------------------------------------------------------------\n\n",
                    spu->ip );

            break;
        }

        case CMNDS::CALL:             //Call
        {
            if( arg_type != 1 )
            {
                printf( "CALL INVALID ARGUMENT\n" );

                return CALC_ERRS::SYNTAX_ERR;
            }

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            StackPush( &spu->stk, spu->ip );

            spu->ip = arg;

            printf( "-------------------------------------------------------------------\n"
                    "CALL successfully jumped to ip -> %llu \n"
                    "-------------------------------------------------------------------\n\n",
                    spu->ip );

            break;
        }

        case CMNDS::RETURN:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );            

            spu->ip = a;

            printf( "-------------------------------------------------------------------\n"
                    "RET successfully jumped back to ip -> %llu \n"
                    "-------------------------------------------------------------------\n\n",
                    spu->ip );

            break;
        }

        case CMNDS::SQUARE_ROOT:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );      

            StackElem root = a;      

            root = sqrt( ( double ) a / MULTIPLYER ) * MULTIPLYER;

            StackPush( &spu->stk, root );

            printf( "-------------------------------------------------------------------\n"
                    "successfully calculated square root of %lld -> %lld \n"
                    "-------------------------------------------------------------------\n\n",
                    a, root);

            break;
        }

        case CMNDS::BITL:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );      

            StackElem res = a << 1;

            StackPush( &spu->stk, res );

            printf( "-------------------------------------------------------------------\n"
                    "successfully calculated  %lld << 1 \n"
                    "-------------------------------------------------------------------\n\n",
                    a );

            break;
        }

        case CMNDS::BITR:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );      

            StackElem res = a >> 1;

            StackPush( &spu->stk, res );

            printf( "-------------------------------------------------------------------\n"
                    "successfully calculated  %lld >> 1 \n"
                    "-------------------------------------------------------------------\n\n",
                    a );

            break;
        }

        case CMNDS::SIN:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );      

            StackElem sinus = sin( ( double ) a / MULTIPLYER ) * MULTIPLYER;

            StackPush( &spu->stk, sinus );

            printf( "-------------------------------------------------------------------\n"
                    "successfully calculated  sin( %lld ) = %lld \n"
                    "-------------------------------------------------------------------\n\n",
                    a, sinus );

            break;
        }

        case CMNDS::COS:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );      

            StackElem cosinus = cos( ( double ) a / MULTIPLYER ) * MULTIPLYER;

            StackPush( &spu->stk, cosinus );

            printf( "-------------------------------------------------------------------\n"
                    "successfully calculated  sin( %lld ) = %lld \n"
                    "-------------------------------------------------------------------\n\n",
                    a, cosinus );

            break;
        }

        case CMNDS::ABS:
        {
            StackElem a = 0;

            StackPop( &spu->stk, &a );      

            StackElem absel = abs( a );

            StackPush( &spu->stk, absel );

            printf( "-------------------------------------------------------------------\n"
                    "successfully calculated abs( %lld ) = %lld \n"
                    "-------------------------------------------------------------------\n\n",
                    a, absel );

            break;
        }

        case CMNDS::JUMP_MONDAYS:
        {
            if( arg_type != 1 )
                return CALC_ERRS::SYNTAX_ERR;

            time_t timer = {};

            tm* calendar = NULL;

            do
            {
                time( &timer );

                calendar = gmtime( &timer );

                printf( "cruely monday come!\n" );
            }
            while( calendar->tm_wday != 1 );

            arg = *( StackElem* ) ( ( char* ) spu->commands + spu->ip );

            spu->ip += sizeof( StackElem );

            spu->ip = arg;

            printf( "-------------------------------------------------------------------\n"
                    "JMP successfully jumped to ip -> %llu \n"
                    "-------------------------------------------------------------------\n\n",
                    spu->ip );

            break;
        }

        default:
            printf( "-------------------------------------------------------------------\n"
                    "SYNTAX ERROR in line: %zu\n"
                    "-------------------------------------------------------------------\n\n",
                    line );

            return CALC_ERRS::SYNTAX_ERR;
    }

    return CALC_ERRS::SYNTAX_ERR;
}

int Calculate( const char* name )
{
    FILE* cmnds = fopen( name, "rb" );
    int64_t size = getFileSize( name );

    printf( "%lld\n", size );

    SPU spu = {};
    SPU_Ctor( &spu, size );

    fread( spu.commands, 1, size, cmnds );
    
    size_t line = 0;

    SPU_Dump( &spu, size );

    for( ; spu.ip < ( u_int64_t ) size; line++ )
    {
        printf( "line: %zu\n", line );

        if( int err = SPU_Verify( &spu ) )
        {
            SPU_Dump( &spu, size );

            printf( "0x%x\n", err );

            return err;
        }

        DoCommand( &spu, line );
    }

    SPU_Dtor( &spu, size );

    return CALC_ERRS::OK;
}

void Draw()
{
    SDL_DisplayMode displayMode;
    
    if( SDL_Init( SDL_INIT_EVERYTHING ) )
    {
        printf( "SDL ERROR DRAWING UNSUCCESSFUL\n" );

        return;
    }

    int err = SDL_GetDesktopDisplayMode( 0 /*number of monitor*/, &displayMode );

    SDL_Window* win = SDL_CreateWindow( "title", displayMode.w / 2, displayMode.h / 2, displayMode.w / 2, displayMode.h / 2, SDL_WINDOW_SHOWN );

    SDL_Renderer* render = SDL_CreateRenderer( win, -1, SDL_RENDERER_PRESENTVSYNC );

    SDL_SetRenderDrawColor( render, 255, 255, 0, 255 );

    SDL_Event event = {};

    //SDL_PollEvent( )

    for( int i = 0; i < 1 << 10; i++ )
    {
        //ConditionMachine

        SDL_RenderDrawPoint( render, i, sin( i/50.0 ) * 100 + 200 );

        SDL_RenderPresent( render );

        SDL_Delay( 10 );
    }

    SDL_Rect dot
    {
        120, 120,
        120, 120
    };

    
}