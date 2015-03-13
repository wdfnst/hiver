/*************************************************************************
 > File Name: UDPNetworkHandler.h
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:14 AM PST
 ************************************************************************/
#ifndef UDPNETWORK_HANDLER_H
#define UDPNETWORK_HANDLER_H

#include "UDPNetwork.h"
#include "NetworkInterface.h"
#include "UDPNetworkInterface.h"
#include "Handler.h"
#include "thirdparty/include/udt.h"
#include "DuplicationManager.h"

#include <iostream>
#include <map>
#include <thread>

namespace pdfs{

class UDPNetworkHandler : public Handler 
{
    public:
        unsigned virtual executeThis();
        UDPNetworkHandler(int id, int sfd) : Handler(id, sfd)
        {
            //this->clusterMap = clusterMap;
        }
        ~UDPNetworkHandler()
        {
        }
}; // end class UDPNetworkHandler
} // end namespace
#endif
