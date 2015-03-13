/*************************************************************************
 > File Name: TCPNetworkInterface.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 07 Dec 2014 04:16:58 AM PST
 ************************************************************************/
#ifndef TCP_NETWORK_INTERFACE_H 
#define TCP_NETWORK_INTERFACE_H 

#include "pdfs.h"
#include "TCPNetwork.h"
#include "NetworkInterface.h"

namespace pdfs{

class TCPNetworkInterface : public NetworkInterface
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
        virtual ssize_t pushFile(Metadata &metadata, std::string dest);
        ////////////////////////////////////////////////////////////////////
        
        /////////////////////Ensure the Transmission////////////////////////
        // To solve the bug of packages sticking together
        ssize_t sendStartCommd(int sockfd); 
        ssize_t recvStartCommd(int sockfd);
        std::string recvCheckFileMD5(int sockfd, const std::string filename);
        ssize_t sendFileMD5(int sockfd, const std::string filename);

        // Constructor and deconstructor
        TCPNetworkInterface(Cluster &cluster);
        TCPNetworkInterface();
        ~TCPNetworkInterface();
};// end of class TCPNetworkInterface 
} // end of namespace pdfs
#endif
