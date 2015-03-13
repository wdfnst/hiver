/********************************************************
 > File Name: BroadcastHandler.cpp
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:24 AM PST
 ********************************************************/

#include "BroadcastHandler.h"

namespace pdfs {

// Listen the fixed port and send status info periodic
unsigned BroadcastHandler::executeThis()
{
    int sockfd;
    struct sockaddr_in bc_addr;
    bzero(&bc_addr, sizeof(bc_addr));
    char statusBuf[MESSAGE_SIZE];
    int so_broadcast = 1;

    bc_addr.sin_family = AF_INET;
    bc_addr.sin_port = htons(CLUSTER_PORT);
    bc_addr.sin_addr.s_addr = inet_addr(BROADCAST_ADDR.c_str());
    bzero(&(bc_addr.sin_zero), 8);

    if((sockfd = (socket(AF_INET, SOCK_DGRAM, 0))) == -1) {
        perror("socket");
        exit(1);
    }
    setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast,
            sizeof(so_broadcast));
    
    // Wait TCP Epoll initiate, or the TCP mode will generate error
    // of "connection refused"
    sleep(1);
    while(1)
    {
        Cluster::getLocalStatus(localNodeStatus);
        memcpy(statusBuf, &localNodeStatus, sizeof(NodeStatus));

        sendto(sockfd, statusBuf, sizeof(NodeStatus), 
                0, (struct sockaddr *)&bc_addr, sizeof(bc_addr));
        sleep(1);
        Cluster::printClusterMap();
    }

    close(sockfd);
    return(0);
} // end of function executeThis

} // end namespace

