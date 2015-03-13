/********************************************************
 > File Name: SClusterHandler.cpp
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:24 AM PST
 ********************************************************/
#include "SClusterHandler.h"

namespace pdfs {
// Listen the fixed port and send status info periodic
unsigned SClusterHandler::executeThis()
{
    // Get UDP listener
    pdfs::UDPNetwork udpnet;
    int listener = udpnet.getListener(CLUSTER_PORT); 
    int epfd = udpnet.getEpollfd(listener);
    struct epoll_event events[pdfs::MAXEPOLLSIZE];

    // Initialize the receive structure
    char recvbuf[pdfs::MESSAGE_SIZE + 1];
    bzero(recvbuf, pdfs::MESSAGE_SIZE + 1);
    struct sockaddr_in client_addr;
    socklen_t cli_len = sizeof(client_addr);

    // Start a timer thread to process expire list
    std::thread checkExpire(Redistribution::checkExpireNode);
    checkExpire.detach();

    // Using bitset to indicate sending progress
    while (true) 
    {
        // Wait event to happy
        int nfds = epoll_wait(epfd, events, 10000, -1);
        if (nfds == -1)
        {
            perror("epoll_wait");
            break;
        }
        // Add the work to work thread pool queue
        for (int n = 0; n < nfds; ++n)
        {
            if (events[n].data.fd == listener) 
            {
                // Receive message from Client
                size_t recvbytes = recvfrom(listener, recvbuf, MESSAGE_SIZE, 
                        0, (struct sockaddr *)&client_addr, &cli_len);
                if (recvbytes > 0)
                    // Display the basic info message
                    Redistribution::updateNodeStatus(inet_ntoa(
                                client_addr.sin_addr), recvbuf);
                else
                    perror("receive broadcast info");
            }
            bzero(recvbuf, MESSAGE_SIZE);
        }
    }
    close(epfd);
    close(listener);
    return(0);
} // end of function executeThis
} // end namespace
