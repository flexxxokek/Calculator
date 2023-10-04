#include <stdlib.h>


#include "stack.h"

#define CANARY_
#define HASH_

//#undef CANARY_
//#undef HASH_

static long long HashCalc( const void* stack, size_t size );

static void HashRefresh( struct Stack* stack );

static bool HashCheck( struct Stack* stack );

static bool DataHashCheck( struct Stack* stack );

static void PrintFileinf( const struct Stack* stack, const char* fileName,
                         const char* funcName, const size_t line );

static STACK_ERRORS PrintError( enum STACK_ERRORS error );




static long long HashCalc( const void* stack, size_t size )
{
    long long hash = 5483;

    for( size_t i = 0; i < size; i++ )
    {
        hash += ( hash << 5 ) + * ( ( char* ) stack + i );
    }

    return hash;
}

static void HashRefresh( struct Stack* stack )
{
    stack->hash = 0;

    stack->dataHash = 0;

    long long newHash = HashCalc( stack, sizeof( *stack ) );

    long long newDataHash = HashCalc( stack->data, sizeof( StackElem ) * stack->capacity );

    stack->hash = newHash;

    stack->dataHash = newDataHash;
}

static bool HashCheck( struct Stack* stack )
{
    long long oldHash = stack->hash;

    long long dataHash = stack->dataHash;

    stack->hash = 0;

    stack->dataHash = 0;

    long long newHash = HashCalc( stack, sizeof( *stack ) );

    if( newHash != oldHash )
    {
        return false;
    }

    stack->hash = newHash;

    stack->dataHash = dataHash;

    return true;
}

static bool DataHashCheck( struct Stack* stack )
{
    long long oldDataHash = stack->dataHash;

    stack->dataHash = 0;

    long long newDataHash = HashCalc( stack->data, sizeof( StackElem ) * stack->capacity );

    if( newDataHash != oldDataHash )
    {
        return false;
    }

    stack->dataHash = newDataHash;

    return true;
}

static bool CheckDataRightChicken( Stack* stack )
{
    return ( *( ( unsigned long long* ) ( stack->data + stack->capacity ) ) == RIGHT_CHICKEN_DEFAULT_NUM );
}

static bool CheckDataLeftChicken( Stack* stack )
{
    return *( ( unsigned long long* ) stack->data - 1 ) == LEFT_CHIKEN_DEFAULT_NUM;
}

static bool CheckLeftChicken( Stack* stack )
{
    return stack->leftChicken == LEFT_CHIKEN_DEFAULT_NUM;
}

static bool CheckRightChicken( Stack* stack )
{
    return stack->rightChicken == RIGHT_CHICKEN_DEFAULT_NUM;
}

static void PrintFileinf( const struct Stack* stack, const char* fileName,
                         const char* funcName, const size_t line )
{
    if( fileName == nullptr )
        fileName = "NOT DEFINED";

    if( funcName == nullptr )
        fileName = "NOT DEFINED";
    
    printf( "Called from %s in function: \"%s\", line: %zu \n", fileName, funcName, line );

    printf( "Variable \"%s\" from %s in function: \"%s\", line: %zu \n", 
            stack->varName, stack->fileName, stack->funcName, stack->line );   
}


void StackCtor( Stack* stack, const char* varName, const char* fileName,
                         const char* funcName, const size_t line )
{
    stack->leftChicken = LEFT_CHIKEN_DEFAULT_NUM;

    stack->rightChicken = RIGHT_CHICKEN_DEFAULT_NUM;

    stack->varName = varName;

    stack->fileName = fileName;

    stack->funcName = funcName;

    stack->line = line;

    stack->size = 0;

    stack->capacity = STACK_DEFAULT_CAPACITY;

    #ifdef CANARY_
    StackElem* dp = ( StackElem* ) calloc(  1, sizeof( StackElem ) * STACK_DEFAULT_CAPACITY +
                                            sizeof( unsigned long long ) * 2 );
    #else
    StackElem* dp = ( StackElem* ) calloc( 1, sizeof( StackElem ) * STACK_DEFAULT_CAPACITY );
    #endif

    if( dp == nullptr )
    {   
        SET_RED_COLOR;

        perror( "CALLOC ERROR" );

        SET_DEFAULT_COLOR;

        dp = nullptr;

        stack->data = nullptr;
    }
        
    stack->data = dp;

    #ifdef CANARY_
    *( unsigned long long* ) stack->data = LEFT_CHIKEN_DEFAULT_NUM;

    stack->data = ( StackElem* ) ( ( unsigned long long* ) stack->data + 1 );

    *( unsigned long long* ) ( stack->data + stack->capacity ) = RIGHT_CHICKEN_DEFAULT_NUM;
    #endif

    #ifdef HASH_
    HashRefresh( stack );
    #endif

    return;
}

#define ERR_CASE( error )                                   \
case error:                                                 \
    printf( "ERROR %d " #error "\n", error );               \
                                                            \
    SET_DEFAULT_COLOR;                                      \
                                                            \
    return error;                                           \

static STACK_ERRORS PrintError( enum STACK_ERRORS error )
{
    SET_RED_COLOR;

    switch( error )
    {
    ERR_CASE( STACK_PTR_IS_NULL )

    ERR_CASE( NOT_ENOUGH_MEMORY )
    
    ERR_CASE( DATA_PTR_IS_NULL )

    ERR_CASE( SIZE_IS_LOWER_THAN_NULL )

    ERR_CASE( CAPACITY_IS_LOWER_THAN_NULL )

    ERR_CASE( SIZE_IS_GREATER_THAN_CAPACITY )

    ERR_CASE( LEFT_CHIKEN_HAS_FALLEN )

    ERR_CASE( RIGHT_CHIKEN_HAS_FALLEN )

    ERR_CASE( DATA_LEFT_CHIKEN_HAS_FALLEN )

    ERR_CASE( DATA_RIGHT_CHIKEN_HAS_FALLEN )

    ERR_CASE( HASH_HAS_FALLEN )

    ERR_CASE( DATA_HASH_HAS_FALLEN )

    default:
        SET_DEFAULT_COLOR;

        return STACK_ALLRIGHT;
    }
}

#undef ERR_CASE

STACK_ERRORS StackDtor( struct Stack* stack )
{
    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );

        putchar( '\n' );

        return error;
    }

    for( size_t i = 0; i < stack->size; i++ )
        stack->data[i] = -1;
    
    stack->capacity = -1;

    stack->size = -1;

    stack->leftChicken = 0;

    stack->rightChicken = 0;

    stack->varName = nullptr;

    stack->fileName = nullptr;

    stack->funcName = nullptr;

    stack->line = -1;

    free( ( unsigned long long* ) stack->data - 1 );

    return STACK_ALLRIGHT;
}

static STACK_ERRORS StackResize( struct Stack* stack, const size_t destCap )
{   
    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );

        putchar( '\n' );

        return error;
    }
    
    stack->capacity = destCap;


    #ifdef CANARY_
    StackElem* dp = ( StackElem* ) realloc( ( unsigned long long* ) stack->data - 1,
                                   sizeof( StackElem ) * stack->capacity + sizeof( unsigned long long ) * 2 );
    #else
    StackElem* dp = ( StackElem* ) realloc( stack->data, sizeof( StackElem ) * stack->capacity );
    #endif

    if( dp == nullptr )
    {
        perror( "CALLOC ERROR" );

        return NOT_ENOUGH_MEMORY;
    }

    stack->data = dp;
    
    #ifdef CANARY_
    *( unsigned long long* ) stack->data = LEFT_CHIKEN_DEFAULT_NUM;

    stack->data = ( StackElem* ) ( ( unsigned long long* ) stack->data + 1 );

    *( unsigned long long* ) ( stack->data + stack->capacity ) = RIGHT_CHICKEN_DEFAULT_NUM;
    #endif

    #ifdef HASH_
    HashRefresh( stack );
    #endif

    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );

        putchar( '\n' );

        return error;
    }

    return STACK_ALLRIGHT;
}

STACK_ERRORS StackVerify( const struct Stack* stack )
{
    if( stack == nullptr ) return NOT_ENOUGH_MEMORY;

    if( stack->data == nullptr ) return DATA_PTR_IS_NULL;
    if( stack->size < 0 ) return SIZE_IS_LOWER_THAN_NULL;
    if( stack->size > stack->capacity ) return SIZE_IS_GREATER_THAN_CAPACITY;

    #ifdef CANARY_
    if( !CheckLeftChicken( ( Stack* ) stack ) ) return LEFT_CHIKEN_HAS_FALLEN;
    if( !CheckRightChicken( ( Stack* ) stack ) ) return RIGHT_CHIKEN_HAS_FALLEN;

    if( !CheckLeftChicken( ( Stack* ) stack ) ) return DATA_LEFT_CHIKEN_HAS_FALLEN;
    if( !CheckRightChicken( ( Stack* ) stack ) ) return DATA_RIGHT_CHIKEN_HAS_FALLEN;
    #endif

    #ifdef HASH_
    if( !HashCheck( ( Stack* ) stack ) ) return HASH_HAS_FALLEN;
    if( !DataHashCheck( ( Stack* ) stack ) ) return DATA_HASH_HAS_FALLEN;
    #endif

    return STACK_ALLRIGHT;
}

STACK_ERRORS StackPop( struct Stack* stack, StackElem* elem )
{
    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );

        putchar( '\n' );

        return error;
    }
    
    if( stack->size > 0 )
    {
        stack->size--;

        if( elem != nullptr )
            *elem = stack->data[stack->size];
        
        stack->data[stack->size] = 0;
    }
    else
    {
        SET_RED_COLOR;

        printf( "STACK HAS NO ELEMENTS TO POP\n" );

        STACK_DUMP( stack );

        SET_DEFAULT_COLOR;
    }

    #ifdef HASH_
    HashRefresh( stack );
    #endif

    if( stack->size == stack->capacity )
    {
        StackResize( ( Stack* ) stack, stack->capacity * 2 );

        #ifdef HASH_
        HashRefresh( stack );
        #endif  
    }
    else if( stack->size < stack->capacity / 4 )
    {
        StackResize( ( Stack* ) stack, stack->capacity / 2 );

        #ifdef HASH_
        HashRefresh( stack );
        #endif
    }

    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );

        putchar( '\n' );

        return error;
    }

    return STACK_ALLRIGHT;
}

STACK_ERRORS StackPush( struct Stack* stack, StackElem elem )
{
    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );

        putchar( '\n' );

        return error;
    }

    stack->data[stack->size++] = elem;

    #ifdef HASH_
    HashRefresh( stack );
    #endif

    if( stack->size == stack->capacity )
    {
        StackResize( ( Stack* ) stack, stack->capacity * 2 );

        #ifdef HASH_
        HashRefresh( stack );
        #endif
    }
    else if( stack->size < stack->capacity / 4 )
    {
        StackResize( ( Stack* ) stack, stack->capacity / 2);

        #ifdef HASH_
        HashRefresh( stack );
        #endif
    }

    if( STACK_ERRORS error = StackVerify( stack ) )
    {
        STACK_DUMP( stack );
     
        putchar( '\n' );

        return error;
    }

    return STACK_ALLRIGHT;
}

void _StackDump( const struct Stack* stack, const char* fileName,
                         const char* funcName, const size_t line )
{
    SET_GRAY_COLOR;

    if( stack == nullptr )
    {
        PrintError( STACK_PTR_IS_NULL );

        return;
    }

    PrintFileinf( stack, fileName, funcName, line );    

    #ifdef HASH_
    if( !HashCheck( ( Stack* ) stack ) ) 
    {
        PrintError( HASH_HAS_FALLEN );

        return;
    }
    #endif
    
    #ifdef CANARY_
    if( !CheckLeftChicken( ( Stack* ) stack ) )
    {
        PrintError( LEFT_CHIKEN_HAS_FALLEN );

        return;
    }
    else if( !CheckRightChicken( ( Stack* ) stack ) )
    {
        PrintError( RIGHT_CHIKEN_HAS_FALLEN );

        return;
    }
    #endif

    printf( "{\n" );
    
    printf( "\tCapacity = %zu\n", stack->capacity );

    printf( "\tSize = %zu\n", stack->size );
    
    printf( "\tStack data ptr = %p\n", stack->data );

    #ifdef HASH_
    if( !DataHashCheck( ( Stack* ) stack ) )
    {
        printf( "}\n" );

        PrintError( DATA_HASH_HAS_FALLEN );

        return;
    }
    #endif

    #ifdef CANARY_
    if( !CheckDataLeftChicken( ( Stack* ) stack ) )
    {
        printf( "}\n" );

        PrintError( DATA_LEFT_CHIKEN_HAS_FALLEN );

        return;
    }
    else if( !CheckDataRightChicken( ( Stack* ) stack ) )
    {
        printf( "}\n" );

        PrintError( DATA_RIGHT_CHIKEN_HAS_FALLEN );

        return;
    }
    #endif

    if( stack->data == nullptr )
    {
        printf( "}\n" );

        PrintError( DATA_PTR_IS_NULL );

        return;
    }

    SET_GREEN_COLOR;

    if( stack->size <= 0 )
    {
        printf( "\tStack has no elements...\n" );
    }
    else
    {
        printf( "\tStack elements:\n" "\t{\n" );

        for( size_t i = 0; i < stack->capacity; i++ )
        {                 
            if( i < stack->size )                                           
                printf( "\t\t" "*[%zu] = %lld\n", i, stack->data[i] ); 
            else
                printf( "\t\t" " [%zu] = %lld\n", i, stack->data[i] );
        }  
        
        printf( "\t}\n\n" );
    }
    
    SET_GRAY_COLOR;

    printf( "}\n" );   

    SET_DEFAULT_COLOR;

    return;
}