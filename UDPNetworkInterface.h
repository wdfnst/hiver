/*************************************************************************
 > File Name: UDPNetworkInterface.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 07 Dec 2014 04:16:58 AM PST
 ************************************************************************/
#ifndef UDP_NETWORK_INTERFACE_H
#define UDP_NETWORK_INTERFACE_H 

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <fstream>
#include <iostream>

#include "UDPNetwork.h"
#include "NetworkInterface.h"
#include "thirdparty/include/udt.h"

namespace pdfs{

class UDPNetworkInterface : public NetworkInterface 
{
    public:
        // Step 1: Send push/pull metadata request
        virtual ssize_t sendPushMetadataRequest(int sockfd, Metadata &metadata);
        virtual ssize_t sendPullMetadataRequest(int sockfd, Metadata &metadata); 
        // Step 2: Send/receive metadata to/from a socket
        virtual ssize_t putMetadata(int sockfd, Metadata &metadata);
        virtual ssize_t getMetadata(int sockfd, Metadata &metadata);
        // Get/Put file metadata:pair<filename, destIP>
        virtual ssize_t pushMetadata(Metadata &metadata, std::string dest = "");
        virtual ssize_t pullMetadata(Metadata &metadata);
        ////////////////////////////////////////////////////////////////////
            
        /////////////////////File Operation/////////////////////////////////
        // Step 1: Send push/pull file request
        virtual ssize_t sendPushFileRequest(int sockfd, Metadata &metadata,
                unsigned int headerFlag = 0);
        virtual ssize_t sendPullFileRequest(int sockfd, std::string filename);
        // Step 2: Send/receive a file to/from a socket
        virtual ssize_t putFile(int sockfd, std::string filename);
        virtual ssize_t getFile(int sockfd, std::string filename, 
                size_t file_size);
        // Step 3: 1 + 2 => Push/Pull a file to destination
        virtual ssize_t pushFile(std::string filename); 
        virtual ssize_t pullFile(std::string filename);
        ////////////////////////////////////////////////////////////////////

        ////////////////////// Backup file ////////////////////////////////
        ssize_t pushFile(Metadata &metadata, std::string dest);
        ssize_t passFileFromTo(Metadata &metadata, std::string from,
                std::string to);
        ////////////////////////////////////////////////////////////////////
        
        /////////////////////Ensure the Transmission////////////////////////
        // To solve the bug of packages sticking together
        std::string recvCheckFileMD5(int sockfd, const std::string filename);
        ssize_t sendFileMD5(int sockfd, const std::string filename);
        
        // Constructor and deconstructor
        UDPNetworkInterface();
        UDPNetworkInterface(Cluster &cluster);
        ~UDPNetworkInterface();
};// end of class FileNetInterface
} // end of namespace pdfs
#endif
