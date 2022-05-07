#ifndef RTMPSERVER_AMFDATAPACKET_H
#define RTMPSERVER_AMFDATAPACKET_H

#include <string>
#include <map>
#include <unordered_map>
#include <vector>

class AMFDataPacket {

    public:

        AMFDataPacket();

        void add(const std::string&, bool);
        void add(const std::string&, const std::string&);
        void add(const std::string&, double);
        void add(const AMFDataPacket&);

        bool getBool(const std::string& key);
        std::string getString(const std::string& key);
        double getDouble(const std::string& key);

        void pprint();
        void pprint(const std::string &line_prefix);

        std::unordered_map<std::string, bool> Booleans;
        std::unordered_map<std::string, std::string> Strings;
        std::unordered_map<std::string, double> Doubles;
        std::vector<AMFDataPacket> objectList;
        int byteError = 0;
};


#endif //RTMPSERVER_AMFDATAPACKET_H
