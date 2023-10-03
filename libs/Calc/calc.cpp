#include "calc.h"

const char* CMNDS[NUM_OF_CMNDS] = { "push",
                                    "add",
                                    "sub",
                                    "mul",
                                    "div",
                                    "out"   };

static CALC_ERRS RunCommand( FILE* fp, Stack* stk, const char* cmnd );

static CALC_ERRS RunCommand( FILE* fp, Stack* stk, const char* cmnd )
{
    int err = 0;

    if( !strcmp( cmnd, CMNDS[0] ) )     //push
    {
        StackElem a = 0;

        if( fscanf( fp, "%lld", &a ) != 1 )
        {
            printf( "SYNTAX ERROR\n" );

            abort();
        }

        err += StackPush( stk, a * MULTIPLYER );

        if( !err )
            printf( "successfully pushed %lld \n", a );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS[1] ) )     //addition
    {
        StackElem a = 0;

        StackElem b = 0;

        err += StackPop( stk, &a );

        err += StackPop( stk, &b );

        err += StackPush( stk, a + b );

        if( !err )
            printf( "successfully completed addtion: %lld + %lld = %lld \n", b, a, a + b );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS[2] ) )     //subtraction
    {
        StackElem a = 0;

        StackElem b = 0;

        err += StackPop( stk, &a );

        err += StackPop( stk, &b );

        err += StackPush( stk, a - b );

        if( !err )
            printf( "successfully completed subtraction: %lld - %lld = %lld \n", b, a, a - b );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS[3] ) )     //multiplication
    {
        StackElem a = 0;

        StackElem b = 0;

        err += StackPop( stk, &a );

        err += StackPop( stk, &b );

        err += StackPush( stk, a * b / MULTIPLYER );

        if( !err )    
            printf( "successfully completed multiplication: %lld * %lld = %lld \n", b, a, a * b / MULTIPLYER );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS[4] ) )     //division
    {
        StackElem a = 0;

        StackElem b = 0;

        err += StackPop( stk, &a );

        err += StackPop( stk, &b );

        StackPush( stk, ( b * MULTIPLYER ) / a );

        if( !err )
            printf( "successfully completed subtraction: %lld / %lld = %lld \n", b, a, ( b * MULTIPLYER ) / a );

        return CALC_ERRS::OK;
    }

    if( !strcmp( cmnd, CMNDS[5] ) )     //output
    {
        StackElem a = 0;

        err += StackPop( stk, &a );

        if( err )
        {
            printf( "error occured in the output function\n" );
        }
        double res = ( double ) a / MULTIPLYER;

        printf( "final result of calculation: %g\n", res);

        return CALC_ERRS::OK;
    }

    printf( "SYNTAX ERROR\n" );

    return CALC_ERRS::SYNTAX_ERR;
}

void Calculate( FILE* fp )
{
    char cmnd[MAX_CMND_LEN] = {};

    Stack stk = {};

    STACK_CTOR( &stk );

    while( fscanf( fp, "%s", cmnd ) != EOF )
    {
        RunCommand( fp, &stk, cmnd );
    }
}