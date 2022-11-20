#ifndef RTMPSERVER_RTMPCONNEXION_H
#define RTMPSERVER_RTMPCONNEXION_H

#include <bitset>
#include <list>
#include "RTMPHeaders.h"
#include "utils/Buffer.h"
#include "AMFDataPacket.h"

class RTMPConnexion {

    public:

        explicit RTMPConnexion(int clientFD);
        void processRequest();
        void receiveAndProcessConnect();
        [[nodiscard]] Buffer receive() const;
        void processBody(Buffer*, RTMPHeaders* headers);
        void AMFConnectCommand(Buffer* buff, RTMPHeaders* headers);

        int sendWindowACKSize() const;
        int sendSetPeerBandwidth() const;
        int sendResulCommand() const;
        int sendSetChunkSize(Buffer* buff) const;

        int clientFD;
        AMFDataPacket connect_packet;


};


#endif //RTMPSERVER_RTMPCONNEXION_H
