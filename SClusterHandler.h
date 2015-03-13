/*************************************************************************
 > File Name: SClusterHandler.h
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:14 AM PST
 ************************************************************************/
#ifndef SCLUSTERHANDLER_H 
#define SCLUSTERHANDLER_H 

#include <iostream>
#include <map>
#include <thread>

#include "pdfs.h"
#include "Handler.h"
#include "UDPNetwork.h"
#include "Cluster.h"
#include "Redistribution.h"

namespace pdfs{

class SClusterHandler : public Handler 
{
    public:
        unsigned virtual executeThis();
        SClusterHandler(int id, int sfd) : Handler(id, sfd)
        {
            //this->cluster= cluster;
        }
        ~SClusterHandler()
        {
            //cout << "Deleting " << id << "\t address=" << this << endl;
        }
}; // end class SClusterHandler
} // end namespace
#endif
