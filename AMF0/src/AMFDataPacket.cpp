#include <iostream>
#include "AMFDataPacket.h"
#include "utils/NotFoundExcepcion.h"
#include "../../include/utils/Logger.h"


/*
 * AMFDataPacket will contain full AMF0 data packet, also,
 * will represent a normal AMF_OBJECT, in this case objectList will be a nullptr
 */
AMFDataPacket::AMFDataPacket() = default;

void AMFDataPacket::add(const std::string& key, bool value) {
    Booleans.insert({key, value});
}

void AMFDataPacket::add(const std::string& key, const std::string& value) {
    Strings.insert({key, value});
}

void AMFDataPacket::add(const std::string& key, double value) {
    Doubles.insert({key, value});
}

void AMFDataPacket::add(const AMFDataPacket& object) {
    objectList.push_back(object);
}

bool AMFDataPacket::getBool(const std::string& key) {
    if (Booleans.find(key) == Booleans.end())
        throw NotFoundExcepcion("Couldn't find the value for " + key);

    return Booleans.find(key)->second;
}

std::string AMFDataPacket::getString(const std::string& key) {
    if (Strings.find(key) == Strings.end())
        throw NotFoundExcepcion("Couldn't find the value for " + key);

    return Strings.find(key)->second;
}

double AMFDataPacket::getDouble(const std::string& key) {
    if (Doubles.find(key) == Doubles.end())
        throw NotFoundExcepcion("Couldn't find the value for " + key);

    return Doubles.find(key)->second;
}

/*
 * pretty Prints full AMF packet
 */
void AMFDataPacket::pprint() {
    try{
        pprint(std::string(""));
    }catch (const NotFoundExcepcion &e) {
        Logger::log(Logger::SOCKET_ERROR_LOG, e.get_error_message());
    }
}

/*
 * pretty Prints full AMF packet
 */
void AMFDataPacket::pprint(const std::string& line_prefix) {
    const std::string basic_prefix("    ");
    std::cout << line_prefix << "{" << std::endl;
    for (auto const& j : Doubles) {
        std::cout << basic_prefix << line_prefix << j.first << " " << getDouble(j.first) << std::endl;
    }
    for (auto const& j : Strings) {
        std::cout << basic_prefix << line_prefix << j.first << " " << getString(j.first) << std::endl;
    }
    for (auto const& j : Booleans) {
        std::cout << basic_prefix << line_prefix << std::boolalpha << j.first << " " << getBool(j.first) << std::endl;
    }
    if (objectList.empty()) {
        std::cout << line_prefix << "}" << std::endl;
        return;
    }
    for (AMFDataPacket j: objectList) {
        j.pprint("    ");
    }
    std::cout << line_prefix << "}" << std::endl;
}
