#ifndef STROKI_
#define STROKI_

#include <stdio.h>

int MyPuts( const char* str);

char* MyStrchr( const char* str, int ch);

size_t MyStrlen( const char* str);

char* MyStrcpy( char* dest, char* src );

void MyStrncpy( char* dest, char* src, size_t count );

void MyStrcat( char* dest, char* src );

void MyStrncat( char* dest, char* src, size_t count );

int isNum( char ch );

int fgetInt( FILE* fp );

int toLow( char ch );

int isLetter( char ch );

int fgets( FILE* fp, char* s);

int fngets( FILE* fp, char* s, size_t maxlen );

#endif
