#include "Redistribution.h"

namespace pdfs {

void Redistribution::updateNodeStatus(std::string addr, const char *msg) 
{
    // Remove the current active node from expire list
    Cluster::getExpireNodes().remove(addr);
    // Update the node of addr
    NodeStatus status;
    memcpy(&status, msg, sizeof(NodeStatus));
    size_t oldSize = Cluster::getNodeStatusMap().size();
    (Cluster::getNodeStatusMap())[addr] = status;
    size_t newSize = Cluster::getNodeStatusMap().size();
    Cluster::setClusterSize(newSize);
    // New node join in the cluster
    if (newSize > oldSize)
    {
        std::clog << "(++)Node join:" << addr << " is " << 
            (Cluster::isNewNode(addr) ? "new node.\n" : "old node.\n");
        if (!Cluster::isLocalIP(addr) && Cluster::isNewNode(addr))
            redistributeMetadata(addr, NODE_JOIN);
    }
    // Delete node from expire list
}
ssize_t Redistribution::checkExpireNode()
{
    // Sleep 30s to wait all the node join in the cluster
    sleep(NEWNODE_EXPIRETIME);
    // When Startup put all the node 
    auto i = Cluster::getNodeStatusMap().begin();
    for (; i != Cluster::getNodeStatusMap().end(); i++)
        Cluster::getExpireNodes().push_back(i->first);
    while (true)
    {
        sleep(NODE_OFFLINE_TIME);
        // Clear the expired nodes
        auto i = Cluster::getExpireNodes().begin(); 
        for (; i != Cluster::getExpireNodes().end(); i++)
        {
            if (Cluster::getNodeStatusMap().size() > REPLICAS)
            {
                std::cout << "(--)Node lost:" << *i << ",To Redistribute.\n";
                redistributeMetadata(*i, NODE_LOST);
                redistributeFile(*i, NODE_LOST);
            }
            else
                std::cout << "(--)Node lost, no node to Redistribute.\n";
            // Delete *i from _nodeStatusMap
            Cluster::getNodeStatusMap().erase(*i);
        }
        Cluster::getExpireNodes().clear();
        auto j = Cluster::getNodeStatusMap().begin();
        for (; j != Cluster::getNodeStatusMap().end(); j++)
            Cluster::getExpireNodes().push_back(j->first);
    }
    return 0;
}

ssize_t Redistribution::redistributeMetadata(const std::string changeNode,
        ssize_t evolveType)
{
    std::vector<std::string> metaFiles = LocalFileSystem::listDirFiles();
    for (auto iter = metaFiles.begin(); iter != metaFiles.end(); iter++)
    {
        std::vector<std::string> nodes = Cluster::getMetaNode(*iter, 4);
        NETWORK_INTERFACE netInterface;
        Metadata metadata = LocalFileSystem::readMetadata(*iter);
        // Node joint: Send metadatafile to new joined node
        if (evolveType == NODE_JOIN)
        {
            bool redisFlag = false; 
            ssize_t index = 0;
            for (auto j = nodes.begin(); j != nodes.end(); index++, j++)
                if (*j == changeNode && index < REPLICAS)
                    redisFlag = true;
            if (redisFlag)
            {
                if (nodes[0] == changeNode && Cluster::isLocalIP(nodes[1]))
                    netInterface.pushMetadata(metadata, changeNode);
                else if(Cluster::isLocalIP(nodes[0]))
                    netInterface.pushMetadata(metadata, changeNode);
                // Remove the redundant metadata file in 4-th node in nodes
                if (nodes.size() > REPLICAS && Cluster::isLocalIP(
                            nodes[REPLICAS]))
                {
                    if (LocalFileSystem::removeFile(METADATA_FOLDER + *iter) 
                            == 0)
                        std::clog << "Remove metadatafile:" << *iter << "\n";
                }
            }
        }
        // Node lost: Backup metadatafile to new metadatafile node
        if (evolveType == NODE_LOST && (nodes[0] == changeNode || 
                    nodes[1] == changeNode || nodes[2] == changeNode))
        {
            if (nodes[0] == changeNode && Cluster::isLocalIP(nodes[1]))
                netInterface.pushMetadata(metadata, nodes[nodes.size() - 1]);
            else if(Cluster::isLocalIP(nodes[0]))
                netInterface.pushMetadata(metadata, nodes[nodes.size() - 1]);
        }
    }
    return 0;
}

// Redistribute files when some node is down in cluster
ssize_t Redistribution::redistributeFile(const std::string changeNode,
        ssize_t evolveType)
{
    std::vector<std::string> metaFiles = LocalFileSystem::listDirFiles();
    for (auto iter = metaFiles.begin(); iter != metaFiles.end(); iter++)
    {
        LocalFileSystem lfs;
        Metadata metadata = lfs.readMetadata(*iter);
        std::vector<std::string> destnodes;
        // Collect the destination nodes contained in the current metadatafile
        bool redisFlag = false;
        for (int i = 0; i < sizeof(metadata.destnode) / IPSTRLEN; i++)
        {
            std::string destnode(metadata.destnode + i * IPSTRLEN, 
                    metadata.destnode + (i + 1) * IPSTRLEN);
            StringUtil::trim(destnode);
            if (!destnode.empty())
                destnodes.push_back(destnode);
            if (destnode == changeNode)
                redisFlag = true;
        }
        ssize_t index = 0, nodesSize = destnodes.size();
        // Check whether the lost node is contained in the current metafile
        if (nodesSize == 0 && !redisFlag)
            continue;
        // If the lost node is the only one node containing the destination 
        // file, then give up backup file, and log 'file lost'
        if (nodesSize == 1 && changeNode.compare(destnodes[0]) == 0)
        {
            std::clog << "Error: File(" << *iter << ") lost.\n";
            continue;
        }
        // To find the lost node in metadatafile, and replace it with 
        // an other one
        for (auto i = destnodes.begin(); i != destnodes.end(); index++, i++)
        {
            // TRANS_INTERFACE netInterface;
            if (!(i->empty()) && changeNode.compare(*i) == 0)
            {
                NETWORK_INTERFACE netInterface;
                // Find  one node didn't contain the specified file
                std::string to;
                std::vector<std::string> topKNodes = Cluster::getTopKNode(4);
                for (auto m = topKNodes.begin(); m != topKNodes.end(); m++)
                {
                    bool equalFlag = false;
                    for (auto n = destnodes.begin(); n != destnodes.end(); n++)
                    {
                        if (*m == *n)
                        {
                            equalFlag = true;
                            break;
                        }
                    }
                    if (equalFlag == false)
                    {
                        std::clog << "Move file:" << *iter<< " replace from "
                            << changeNode << " with " << *m << std::endl;
                        to = *m;
                        break;
                    }
                }
                // Send a file pass command to the node who possess file
                if (!to.empty())
                {
                    // Replace lost node with "to" node in metadata 
                    memcpy(metadata.destnode + index * IPSTRLEN, to.data(), 
                            IPSTRLEN);
                    // Send on backup of file to "to" node
                    if (Cluster::isLocalIP(destnodes[(index + 1) % nodesSize]))
                        netInterface.pushFile(metadata, to);
                    LocalFileSystem::writeMetadata(metadata);
                    break;
                }
                else
                    std::cout << "Files lost in node:" << changeNode << "\n";
            }
        }
    }
    return 0;
}
} // end of namepsace
