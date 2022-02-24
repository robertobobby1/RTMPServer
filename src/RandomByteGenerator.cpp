#include "RandomByteGenerator.h"
#include <stdlib.h>
#include <fstream>

std::byte* RandomByteGenerator::run(){
    return randomByteGenerator();
}

std::byte* RandomByteGenerator::randomByteGenerator() {
    auto* pBytes = new std::byte[1535];
    // Fill random value.
    srand( 255 );
    for( int i = 0; i < 1536; i++ )
    {
        if (i == 0) {
            pBytes[i] = (std::byte)3;
        }
        pBytes[ i ] = ( std::byte ) rand();
    }
    return (std::byte*)pBytes;
}
