/*************************************************************************
 > File Name: TCPNetworkHandler.h
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:14 AM PST
 ************************************************************************/
#ifndef TCPNETWORK_HANDLER_H
#define TCPNETWORK_HANDLER_H

#include <iostream>
#include <map>

#include "Handler.h"
#include "TCPNetworkInterface.h"
#include "DuplicationManager.h"

namespace pdfs{

class TCPNetworkHandler : public Handler 
{
    public:
        unsigned virtual executeThis();
        TCPNetworkHandler(int id, int sfd) : Handler(id, sfd)
        {
            //this->clusterMap = clusterMap;
        }
        ~TCPNetworkHandler()
        {
            //cout << "Deleting TCPNetworkHandler" << id << "\t address=" << this << endl;
        }
}; // end class TCPNetworkHandler
} // end namespace

#endif
