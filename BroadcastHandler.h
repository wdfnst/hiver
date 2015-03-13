/*************************************************************************
 > File Name: BroadcastHandler.h
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:14 AM PST
 ************************************************************************/
#ifndef BROADCAST_HANDLER_H 
#define BROADCAST_HANDLER_H 

#include <arpa/inet.h>
#include <iostream>
#include <map>

#include "pdfs.h"
#include "Handler.h"
#include "Cluster.h"

namespace pdfs{

class BroadcastHandler: public Handler 
{
    public:
        unsigned virtual executeThis();
        BroadcastHandler(int id, int sfd) : Handler(id, sfd)
        {
            //this->clusterMap = clusterMap;
            localNodeStatus.starttime = time(0);
        }
        ~BroadcastHandler()
        {
            //cout << "Deleting BroadcastHandler" << id << "\t address=" << this << endl;
        }
    private:
        NodeStatus localNodeStatus;
}; // end class BroadcastHandler 
} // end namespace

#endif
