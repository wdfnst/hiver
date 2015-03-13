/*************************************************************************
 > File Name: DuplicationManager.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Tue 09 Dec 2014 07:10:15 AM PST
 ************************************************************************/
#ifndef DUPLICATION_MANAGER_H
#define DUPLICATION_MANAGER_H

#include <iostream>
#include <vector>

#include "pdfs.h"
#include "Cluster.h"
#include "UDPNetworkInterface.h"
#include "TCPNetworkInterface.h"

namespace pdfs {

class DuplicationManager
{
    private:

    public:
        static ssize_t backupMetadata(NetworkInterface &netinterface, Metadata 
                &metadata);
        static ssize_t backupFile(NetworkInterface &netinterface, Metadata 
                &metadata);
        static ssize_t addMetadata(NetworkInterface &netinterface, Metadata
                &metadata, std::string dest);
        static ssize_t addFile(NetworkInterface &netinterface, Metadata 
                &metdata, std::string dest);

        DuplicationManager();
        ~DuplicationManager();

}; // end of class DuplicationManager
} // end of namespace pdfs
#endif
