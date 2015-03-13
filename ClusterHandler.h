/*************************************************************************
 > File Name: ClusterHandler.h
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:14 AM PST
 ************************************************************************/
#ifndef CLUSTERHANDLER_H 
#define CLUSTERHANDLER_H 

#include <iostream>
#include <map>
#include <thread>

#include "pdfs.h"
#include "Handler.h"
#include "UDPNetwork.h"
#include "Cluster.h"

namespace pdfs{
class ClusterHandler : public Handler 
{
    public:
        unsigned virtual executeThis();
        ClusterHandler(int id, int sfd) : Handler(id, sfd)
        {
            //this->cluster= cluster;
        }
        ~ClusterHandler()
        {
            //cout << "Deleting " << id << "\t address=" << this << endl;
        }
}; // end class ClusterHandler
} // end namespace
#endif
