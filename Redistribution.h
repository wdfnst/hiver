#ifndef REDISTRIBUTION_H  
#define REDISTRIBUTION_H

#include <map>
#include <vector>
#include <algorithm>

#include "pdfs.h"
#include "Cluster.h"
#include "UDPNetworkInterface.h"
#include "TCPNetworkInterface.h"

namespace pdfs {
const ssize_t NODE_JOIN = 1;
const ssize_t NODE_LOST = 2;
class Redistribution {
    public:
        static void updateNodeStatus(std::string addr, const char *msg);
        static ssize_t checkExpireNode();

        // Redistribution function
        static ssize_t redistributeMetadata(const std::string changeNode,
                ssize_t evolveType);
        static ssize_t redistributeFile(const std::string changeNode, 
                ssize_t evolveType);
};
} // end of namepsace pdfs
#endif
