#ifndef DATA_CONSTANTS_
#define DATA_CONSTANTS_

const int MAX_STR_LENGTH = 1024;
const int MAX_STR_NUM = 1024;
const int MAX_FLAG_LENGTH = 16;
const int MAX_FILE_NAME_LENGTH = 16;

enum FILEINF_ERRORS
{
    FILE_NAME_IS_NULL = 123,
    FILE_SIZE_IS_NULL = 321,
    FILE_BUFF_IS_NULL = 231,
    FILE_STRDATA_IS_NULL = 213,
    FILEINF_ALLRIGHT = 0
};

enum FilePointerErrors
{
    FILE_POINTER_IS_NULL = 102, NO_PERMISSION_FOR_FILE = 404
};

struct String
{
    char* ptr;

    size_t len;
};

struct Fileinf
{
    char filename[MAX_FILE_NAME_LENGTH];
    
    size_t size;

    size_t trueSize;

    size_t nlines;

    char* buff;

    String* strData;
};

#endif