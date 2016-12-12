#include "CalculateHash.h"

char* CalculateHash::calculate(char* input)
{
    unsigned char hash[20];
    static char hex_str[41];
    
    sha1::calc(input, strlen(input), hash);
    sha1::toHexString(hash, hex_str);
    return hex_str;
}
