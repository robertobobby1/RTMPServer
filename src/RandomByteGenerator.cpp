#include "RandomByteGenerator.h"
#include <stdlib.h>

unsigned char pBytes[1537];

unsigned char* RandomByteGenerator::run(){
    // Fill random value.
    srand( 255 );
    // first byte (0x03)
    pBytes[0] = (unsigned char)3;
    for( int i = 1; i < 1536; i++ )
    {
        pBytes[i] = (unsigned char)rand();
    }
    return pBytes;
}
