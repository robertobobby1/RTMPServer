#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-use-auto"
#include "RandomByteGenerator.h"
#include <cstdlib>
#include <fstream>
#include <sstream>


void RandomByteGenerator::run(){
    //auto *random_byte = (unsigned char*)malloc(sizeof(unsigned char));
    /*unsigned char *ArrayByte = (unsigned char*)malloc(sizeof(unsigned char[1536]));
    for (int i = 0; i < 1537; ++i) {
        auto random_byte = (unsigned char*)malloc(sizeof(unsigned char));
        *random_byte = generateRandomByte();
        memcpy(ArrayByte + (sizeof(&random_byte) * i), (unsigned char*)random_byte, sizeof(&random_byte));
    }
    unsigned char finalArray[1536];
    *finalArray = *ArrayByte;
    int x = 0;
}

unsigned char RandomByteGenerator::generateRandomByte(){
    int myseed = 1234;
    std::default_random_engine rng(myseed);
    std::uniform_int_distribution<int> rng_dist(0, 255);
    unsigned char mychar = static_cast<unsigned char>(rng_dist(rng)); // NOLINT(cert-msc50-cpp,modernize-use-auto)
    return mychar;


    FILE * file = fopen("/home/roberto/CLionProjects/RTMPServer/docs/randbyte", "r+");
    if (file == nullptr) return;
    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    fclose(file);
// Reading data to array of unsigned chars
    file = fopen("/home/roberto/CLionProjects/RTMPServer/docs/randbyte", "r+");
    unsigned char *in = (unsigned char *) malloc(size);
    unsigned char *res[1536];
    res = file.;
    int bytes_read = fread(in, sizeof(unsigned char), size, file);
    fclose(file);*/
    unsigned char *res[1537];
    std::ostringstream sstream;
    std::ifstream fs("test.txt");
    sstream << fs.rdbuf();
    const std::string str(sstream.str());
    const char* ptr = str.c_str();
}
