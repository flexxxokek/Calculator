#include <stdio.h>
#include <assert.h>

#include "stroki.h"

int MyPuts( const char* str )
{
    assert( str != NULL );
    
    int nchar = 0;

    for( ; str[nchar]; nchar++ )
        putchar( str[nchar] );

    printf( "\n" );

    return nchar;
}

char* MyStrchr( const char* str, int ch )
{
    assert( str != NULL);
    // ( str )
    for( size_t i = 0; str[i]; i++ )
        if( str[i] == ch )
            return ( char*) str + i;

    return NULL;
}

size_t MyStrlen( const char* str )
{
    assert( str != NULL);

    size_t i = 0;

    for( ; str[i] != '\0'; i++)
        ;

    return i;
}

char* MyStrcpy( char* dest, char* src )
{
    assert( dest != NULL );
    assert( src != NULL );
    assert( src != dest );

    while( *src )
        *dest++ = *src++;

    *dest = '\0';

    return dest;
}

void MyStrncpy( char* dest, char* src, size_t count )
{
    assert( dest != NULL);
    assert( src != NULL);
    assert( src != dest);

    for( size_t i = 0; i < count; i++)
    {
        if( !*src ) 
        { 
            *dest++ = *src++;
            putchar( '1' );
        }
        else
        {
            for( int j = i; j < count; i++ )
                *dest++ = '\0';
            
            return;
        }
    }
}

void MyStrcat( char* dest, char* src )
{
    dest += MyStrlen( dest );

    MyStrcpy( dest, src );
}

/*int MyReverseStrcmp( const void* a, const void* b )
{
    struct String ap = *( String* ) a;
    struct String bp = *( String* ) b;

    for( int i = 1, j = 1; ap.len >= i && bp.len >= j ; )
    {
        if( !isLetter( ap.ptr[ap.len - i] ) )
        {
            i++;

            continue;
        }

        if( !isLetter( bp.ptr[bp.len - j] ) )
        {
            j++;

            continue;
        }

        if( toLow( ap.ptr[ap.len - i] ) > toLow( bp.ptr[bp.len - j] ) )
            return 1;
        else if( toLow( ap.ptr[ap.len - i] ) < toLow( bp.ptr[bp.len - j] ) )
            return -1;

        i++;
        j++;
    }
    
    if( ap.len == bp.len )
        return 0;
    if( ap.len > bp.len)
        return 1;
    else
        return -1;
}*/



void MyStrncat( char* dest, char* src, size_t count )
{
    while( *dest++ )
        ;
    
    dest--;

    MyStrncpy( dest, src, count );
}

int fgets( FILE* fp, char* str )
{
    assert( fp != NULL );
    assert( str != NULL );

    size_t len = 0;

    for( int ch = 0; ( ch = getc( fp ) ) != EOF && ch != '\n'; len++ )
        *str++ = ch;
    
    *str = '\0';

    return len;
}

int isNum( char ch )
{
    if( ch <= '9' && ch >= '0' )
        return true;
    
    return false;
}

int isLetter( char ch )
{
    if( ch <= 'z' && ch >= 'a' ||
        ch <= 'Z' && ch >= 'A' )
        return 1;
    
    return 0;
}

int toLow( char ch )
{
    if( isLetter( ch ) )
        return ( ch - 'a' < 0 )? ch - 'A' + 'a' : ch;
    
    return -1;
}

int charToNum( char ch )
{
    return ch - '0';
}

int fskipTabs( FILE* fp )
{
    int ch = 0;

    while( ( ch = getc( fp ) ) != EOF && ( ch == ' ' || ch == '\t' || ch == '\n' ) )
        ;

    return ch;
}

int fgetInt( FILE* fp )
{
    int ch = fskipTabs( fp );
    int sign = 1;
    int number = 0;

    if( ch == '-' )
        sign = -1;
    else if( isNum( ch ) )
        number += charToNum( ch );
    else
        return NULL;
     
    while( isNum( ch = getc( fp ) ) )
        number = number * 10 + charToNum( ch );
    
    return sign * number;
}
