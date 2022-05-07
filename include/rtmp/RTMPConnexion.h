#ifndef RTMPSERVER_RTMPCONNEXION_H
#define RTMPSERVER_RTMPCONNEXION_H

#include <bitset>
#include "RTMPHeaders.h"
#include "utils/Buffer.h"

class RTMPConnexion {

    public:

        explicit RTMPConnexion(int clientFD);
        void processRequest();
        void processNewPacket();
        [[nodiscard]] Buffer receive() const;
        void processBody(Buffer*, RTMPHeaders* headers);
        void processAMFCommand(Buffer* buff, RTMPHeaders* headers);
        void sendWindowACKSize();

        int clientFD;
        bool window_ack_size = false;
        bool connect = false;
};


#endif //RTMPSERVER_RTMPCONNEXION_H
