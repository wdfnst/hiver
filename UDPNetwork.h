/*************************************************************************
 > File Name: UDPNetwork.h
 > Author: test
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 07:20:16 AM PST
 ************************************************************************/
#ifndef UDPNETWORK_H
#define UDPNETWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
//#include <openssl/ssl.h>
//#include <openssl/err.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <assert.h>

#include <bitset>
#include <cmath>

#include "pdfs.h"
#include "thirdparty/include/udt.h"

namespace pdfs {

    class UDPNetwork {
        public:
            int setnonblocking(int sockfd);

            // Return a sockaddr_in with specified ip and port
            struct sockaddr_in getSockAddr(unsigned long ip = INADDR_ANY, 
                    unsigned short port = 2300);

            struct sockaddr_in getSockAddr(std::string ip = "127.0.0.1", 
                    unsigned short port = 2300);

            // Retrun a regular socket
            int getRawSocket();
            int getUDTRawSocket();
            // Get a socket binded a sockaddr_in
            int getListener(unsigned short port = 2300,
                    unsigned long ip = INADDR_ANY);
            int getUDTListener(unsigned short port = 2300,
                    unsigned long ip = INADDR_ANY);
            // Get epoll fd
            int getEpollfd(int udp_sfd);
            int getUDTEpollfd(int udp_sfd);

            int getConnection(std::string destIP = "127.0.0.1", 
                    unsigned short port = 2300);
            int getUDTConnection(std::string destIP = "127.0.0.1", 
                    unsigned short port = 2300);
            // Send a message with message header only
            int sendMessage(struct messageheader header, std::string destIP, 
                    unsigned short destPort);

            // send Ack message
            int replyAck(struct sockaddr_in &addr, char *replyMsg);

            // Function of thread to receive ack message
            static void *recvAckThread(void *sack);
    }; // end class UDPNetwork

} // end namespace pdfs

#endif
