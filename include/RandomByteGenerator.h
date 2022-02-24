#ifndef RTMPSERVER_RANDOMBYTEGENERATOR_H
#define RTMPSERVER_RANDOMBYTEGENERATOR_H
#include <cstddef>

class RandomByteGenerator{
public:
    static std::byte* run();
    static std::byte* randomByteGenerator();
};

#endif //RTMPSERVER_RANDOMBYTEGENERATOR_H
