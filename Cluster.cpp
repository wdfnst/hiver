#include "Cluster.h"
#include "UDPNetwork.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>

namespace pdfs {

std::map<std::string, NodeStatus> Cluster::_nodeStatusMap;
std::list<std::string> Cluster::_expireNodes;
size_t Cluster::clusterSize = 0;

Cluster::Cluster(const std::map<std::string, float> &clusterMap)
{ 
}

void Cluster::getLocalIPv4Addr(string localIPv4Addr)
{
    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;
    string localIP;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct != NULL) {
        char addressBuffer[INET6_ADDRSTRLEN];
        memset(addressBuffer, '\0', INET6_ADDRSTRLEN);
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) { 
            // check it is IP4 is a valid IP4 Address
            tmpAddrPtr = 
                &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            //char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            localIPv4Addr = addressBuffer;
            if (localIPv4Addr.find("192.168") == 0 
                    || localIPv4Addr.find("10.") == 0)
                return;
        } else if (ifAddrStruct->ifa_addr->sa_family == AF_INET6) { 
            // check it is IP6 is a valid IP6 Address
            tmpAddrPtr = 
                &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        } 
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
}

void Cluster::getBroadcastIPAddr(string broadcastIPAddr)
{
   string localIPAddr; 
   getLocalIPv4Addr(localIPAddr);
   localIPAddr = localIPAddr.substr(0, localIPAddr.find_last_of("."));
   broadcastIPAddr = localIPAddr + ".255";
}

void Cluster::updateNodeStatus(std::string addr, const char *msg) 
{
    // Remove the current active node from expire list
    _expireNodes.remove(addr);
    // Update the node of addr
    NodeStatus status;
    memcpy(&status, msg, sizeof(NodeStatus));
    size_t oldSize = _nodeStatusMap.size();
    _nodeStatusMap[addr] = status;
    size_t newSize = _nodeStatusMap.size();
    clusterSize = newSize;
    // New node join in the cluster
    if (newSize > oldSize)
    {
        std::clog << "(+)Node join:" << addr << std::endl;
    }
}

ssize_t Cluster::checkExpireNode()
{
    // Sleep 30s to wait all the node join in the cluster
    sleep(NEWNODE_EXPIRETIME);
    // When Startup put all the node 
    auto i = _nodeStatusMap.begin();
    for (; i != _nodeStatusMap.end(); i++)
        _expireNodes.push_back(i->first);
    while (true)
    {
        sleep(NODE_OFFLINE_TIME);
        // Clear the expired nodes
        auto i = _expireNodes.begin(); 
        for (; i != _expireNodes.end(); i++)
        {
            // Delete *i from _nodeStatusMap
            _nodeStatusMap.erase(*i);
            std::cout << "(-)Node lost:" << *i << std::endl;
        }
        _expireNodes.clear();
        auto j = _nodeStatusMap.begin();
        for (; j != _nodeStatusMap.end(); j++)
            _expireNodes.push_back(j->first);
    }
    return 0;
}

NodeStatus& Cluster::getLocalStatus(NodeStatus &status) 
{
    struct sysinfo meminfo;
    sysinfo(&meminfo);
    status.memUsage = 1.0 - (float)meminfo.freeram/meminfo.totalram;

    struct statvfs diskinfo;
    statvfs("/", &diskinfo);
    status.diskUsage = 1.0 - (float)diskinfo.f_bfree/diskinfo.f_blocks;

    FILE *fd;
    char buff[256];
    fd = fopen ("/proc/loadavg", "r"); 
    fgets (buff, sizeof(buff), fd);
    sscanf (buff, "%f", &status.cpuUsage);

    status.overall = CPU_RATIO * status.cpuUsage + 
        MEM_RATIO * status.memUsage + DISK_RATIO * status.diskUsage;

    fclose(fd);
    return status;
}

std::map<std::string, NodeStatus> &Cluster::getNodeStatusMap()
{
    return _nodeStatusMap;
}

std::list<std::string> &Cluster::getExpireNodes()
{
    return _expireNodes;
}

std::vector<std::string> Cluster::getTopKNode(unsigned short num)
{
    std::vector<std::string> topK;
    std::vector<clustermapType> gClustermapvec(_nodeStatusMap.begin(),
            _nodeStatusMap.end());
    size_t size = gClustermapvec.size() > REPLICAS ? 
        pdfs::REPLICAS : gClustermapvec.size();
    std::partial_sort(gClustermapvec.begin(), 
            gClustermapvec.begin() + size,
            gClustermapvec.end(), CmpClustermapByValue());
    auto iter = gClustermapvec.begin();
    for ( int i = 0; i < std::max(REPLICAS, num) && 
            iter != gClustermapvec.end(); i++, iter++)
        topK.push_back(iter->first);

    return std::move(topK);
}

// parameter: num is the number of wanted nodes
std::vector<std::string> Cluster::getMetaNode(std::string filename, 
        unsigned short num)
{
    size_t slashPos = filename.find_last_of("/");
    if ( slashPos != std::string::npos)
        filename = filename.substr(slashPos + 1, 
                filename.size() - slashPos);
    std::vector<std::string> metaNodes;
    std::map<long, std::string> nodesHashMap;
    for (auto iter = _nodeStatusMap.begin(); 
            iter != _nodeStatusMap.end(); iter++)
        nodesHashMap.insert(std::make_pair(
                    FNVHash1(iter->first + filename), iter->first));
    auto iter = nodesHashMap.begin();
    for (int i = 0; i < std::max(REPLICAS, num) && iter != nodesHashMap.end();
            i++, iter++)
        metaNodes.push_back(iter->second);
    return std::move(metaNodes);
}

size_t Cluster::getClusterSize()
{
    return clusterSize;
}

void Cluster::setClusterSize(size_t size)
{
    clusterSize = size;
}

// The improved 32 bit FNV algorithm 1
long Cluster::FNVHash1(std::string data)
{
    int p = 16777619;
    long hash = 2166136261L;
    for(unsigned i = 0; i < data.size(); i++)
        hash = (hash ^ data[i]) * p;
    hash += hash << 13;
    hash ^= hash >> 7;
    hash += hash << 3;
    hash ^= hash >> 17;
    hash += hash << 5;
    return hash;
}

unsigned short Cluster::isLocalIP(std::string ip)
{
    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct != NULL) {
        char addressBuffer[INET6_ADDRSTRLEN];
        memset(addressBuffer, '\0', INET6_ADDRSTRLEN);
        if (ifAddrStruct->ifa_addr->sa_family == AF_INET) { 
            // check it is IP4 is a valid IP4 Address
            tmpAddrPtr = 
                &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            //char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
        } else if (ifAddrStruct->ifa_addr->sa_family == AF_INET6) { 
            // check it is IP6 is a valid IP6 Address
            tmpAddrPtr = 
                &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
        } 
        if (strcmp(ip.c_str(), addressBuffer) == 0)
            return 1;
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
    return 0;
}

bool Cluster::isNewNode(std::string node)
{
    return time(0) - _nodeStatusMap[node].starttime < NEWNODE_EXPIRETIME;
}

void Cluster::printClusterMap()
{
    int counter = 1;
    if (MSG_INFO >= MSG_LEVEL)
    {

        std::cout << "----------------------------------Cluster Info"
            "-----------------------------------" << std::endl;
        std::cout << "        IP\t        Overall\t\tcpuUsage\t"
            "memeUsage\tdiskUsage" << std::endl;
        for (auto iter = _nodeStatusMap.begin();
                iter != _nodeStatusMap.end(); iter++)
        {
            printf("(%d)%s\t%1.6f\t%1.6f\t%1.6f\t%1.6f\n", counter, 
                    iter->first.c_str(), iter->second.overall,
                    iter->second.cpuUsage, iter->second.memUsage, 
                    iter->second.diskUsage);
            counter++;
        }
    }
}

///////////////// StringUtil ///////////////////
std::string &StringUtil::trim(std::string &str)
{
    if (str.empty())return str;
    str.erase(0, str.find_first_not_of('\0'));
    str.erase(str.find_last_not_of('\0') + 1);
    return str;
}
void log(int msglevel, std::string msg)
{
    if (msglevel >= MSG_ERROR)
        perror(msg.data());
    else if (msglevel > MSG_LEVEL)
        std::clog << msg << std::endl;
    else
        std::clog << "undfined msg level:" << msg << std::endl;
}
} // end of namepsace
