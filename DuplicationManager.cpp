/*************************************************************************
 > File Name: DuplicationManager.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Tue 09 Dec 2014 07:10:25 AM PST
 ************************************************************************/
#include "DuplicationManager.h"

namespace pdfs {

DuplicationManager::DuplicationManager()
{ }

DuplicationManager::~DuplicationManager()
{ }

ssize_t DuplicationManager::addMetadata(NetworkInterface &netinterface, 
        Metadata& metadata, std::string dest)
{ 
    if(-1 == netinterface.pushMetadata(metadata, dest)) return -1;
    return 0;
}

ssize_t DuplicationManager::backupMetadata(NetworkInterface &netinterface, 
        Metadata &metadata)
{
    std::vector<std::string> nodes = Cluster::getMetaNode(metadata.filename);
    for(size_t i = 0; i < nodes.size(); i++)
        if (!Cluster::isLocalIP(nodes[i]))
            addMetadata(netinterface, metadata, nodes[i]);
    return 0;
}

ssize_t DuplicationManager::addFile(NetworkInterface &netinterface, 
        Metadata &metadata, std::string dest)
{
    if (-1 == netinterface.pushFile(metadata, dest))
        return -1;
    return 0;
}

ssize_t DuplicationManager::backupFile(NetworkInterface &netinterface, 
        Metadata &metadata)
{
    for (int i = 0; i < sizeof(metadata.destnode) / IPSTRLEN; i++)
    {
        std::string destnode(metadata.destnode + i * IPSTRLEN, 
                metadata.destnode + (i + 1) * IPSTRLEN);
        StringUtil::trim(destnode);
        if (!destnode.empty() && !Cluster::isLocalIP(destnode))
            addFile(netinterface, metadata, destnode);
    }
    return 0;
}
} // end of namespace pdfs
