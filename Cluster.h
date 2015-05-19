#ifndef CLUSTER_H
#define CLUSTER_H 

#include "ThreadPool.h"
#include <utility>
#include <map>
#include <vector>
#include <list>
#include <algorithm>
#include <ctime>

#include <ifaddrs.h>
#include <netinet/in.h>

namespace pdfs {

const float CPU_RATIO = 1.0;
const float MEM_RATIO = 1.0; 
const float DISK_RATIO = 1.0;

struct NodeStatus {
    int starttime;
    float overall;
    float cpuUsage;
    float memUsage;
    float diskUsage;
};

typedef std::pair<std::string, pdfs::NodeStatus> clustermapType; 
struct CmpClustermapByValue {  
    bool operator()(const clustermapType& lhs, const clustermapType& rhs) {
        return lhs.second.overall < rhs.second.overall; 
    }  
};

class Cluster {
    public:
        Cluster(){}
        Cluster(const std::map<std::string, float> &clusterMap);
        static NodeStatus &getLocalStatus(NodeStatus &status);
        static void updateNodeStatus(std::string addr, const char *msg);
        static std::vector<std::string> getTopKNode(unsigned short num = 3);
        static std::vector<std::string> getMetaNode(std::string filename, 
                unsigned short num = 3);
        static unsigned short isLocalIP(std::string ip); 
        static bool isNewNode(std::string node);

        static void getLocalIPv4Addr(string localIPv4Addr);
        static void getBroadcastIPAddr(string broadcastIPAddr);
        static std::map<std::string, NodeStatus> &getNodeStatusMap();
        static std::list<std::string> &getExpireNodes(); 
        static void printClusterMap();
        static long FNVHash1(std::string data);

        static ssize_t checkExpireNode();
        static size_t getClusterSize();
        static void setClusterSize(size_t size);
    private:
        static std::map<std::string, NodeStatus> _nodeStatusMap;
        static std::list<std::string> _expireNodes;
        static size_t clusterSize;
};
class StringUtil {
    public:
        static std::string& trim(std::string &str);
};
} // end of namepsace pdfs
#endif
