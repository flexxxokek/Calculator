#include "calc.h"

static CALC_ERRS RunCommand( Fileinf* cmnds, size_t line, Stack* stk, CMNDS cmnd );

#define SEP    \
printf( "-------------------------------------------------------------------\n" );

static CALC_ERRS RunCommand( Fileinf* cmnds, size_t line, Stack* stk, CMNDS cmnd )
{
    int err = 0;
    
    StackElem a = 0;

    StackElem b = 0;

    switch( cmnd )
    {
        case CMNDS::IN:
            printf( "-------------------------------------------------------------------\n"
                    "Please, enter number: " );

            if( scanf( "%lld", &a ) != 1 )
            {
                printf( "SYNTAX ERROR in line: %zu\n"
                        "-------------------------------------------------------------------\n\n",
                        line );

                abort();
            }

            err += StackPush( stk, a * MULTIPLYER );

            if( !err )
                printf( "successfully pushed %lld \n"
                        "-------------------------------------------------------------------\n\n", a );

            return CALC_ERRS::OK;

        case CMNDS::PUSH:
            if( sscanf( cmnds->strData[line].ptr, "%lld", &a ) != 1 )
            {
                printf( "-------------------------------------------------------------------\n"
                        "SYNTAX ERROR in line: %zu\n"
                        "-------------------------------------------------------------------\n\n",
                        line );

                abort();
            }

            err += StackPush( stk, a * MULTIPLYER );

            if( !err )
                printf( "-------------------------------------------------------------------\n"
                        "successfully pushed %lld \n"
                        "-------------------------------------------------------------------\n\n", a );

            return CALC_ERRS::OK;
        
        case CMNDS::ADDITION:
            err += StackPop( stk, &a );

            err += StackPop( stk, &b );

            err += StackPush( stk, b + a );

            if( !err )
                printf( "-------------------------------------------------------------------\n"
                        "successfully completed addtion: %lld + %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n", 
                        b, a, b + a );

            return CALC_ERRS::OK;

        case CMNDS::SUBTRACTION:
            err += StackPop( stk, &a );

            err += StackPop( stk, &b );

            err += StackPush( stk, b - a );

            if( !err )
                printf( "-------------------------------------------------------------------\n"
                        "successfully completed subtraction: %lld - %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n",
                        b, a, b - a );

            return CALC_ERRS::OK;

        case CMNDS::MULTIPLICATION:
            err += StackPop( stk, &a );

            err += StackPop( stk, &b );

            err += StackPush( stk, a * b / MULTIPLYER );

            if( !err )    
                printf( "-------------------------------------------------------------------\n"
                        "successfully completed multiplication: %lld * %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n",
                        b, a, a * b / MULTIPLYER );

            return CALC_ERRS::OK;

        case CMNDS::DIVISION:
            err += StackPop( stk, &a );

            err += StackPop( stk, &b );

            StackPush( stk, ( b * MULTIPLYER ) / a );

            if( !err )
                printf( "-------------------------------------------------------------------\n"
                        "successfully completed subtraction: %lld / %lld = %lld \n"
                        "-------------------------------------------------------------------\n\n", 
                        b, a, ( b * MULTIPLYER ) / a );

            return CALC_ERRS::OK;

        case CMNDS::OUT:
        {
            err += StackPop( stk, &a );

            if( err )
            {
                printf( "-------------------------------------------------------------------\n"
                        "error occured in the output function\n"
                        "-------------------------------------------------------------------\n\n" );
            }

            double res = ( double ) a / MULTIPLYER;
            
            printf( "-------------------------------------------------------------------\n"
                    "final result of calculation: %g\n"
                    "-------------------------------------------------------------------\n\n",
                    res);

            return CALC_ERRS::OK;
        }

        default:
            printf( "-------------------------------------------------------------------\n"
                    "SYNTAX ERROR in line: %zu\n"
                    "-------------------------------------------------------------------\n\n",
                    line );

            return CALC_ERRS::SYNTAX_ERR;
    }

    return CALC_ERRS::OK;
}

void Calculate( Fileinf* cmnds )
{
    CMNDS cmnd = CMNDS::INVALID_SYNTAX;

    Stack stk = {};

    STACK_CTOR( &stk );

    for( size_t line = 0; sscanf( cmnds->strData[line].ptr, "%d", &cmnd ) == 1; line++ )
    {
        RunCommand( cmnds, line, &stk, cmnd );
    }
}