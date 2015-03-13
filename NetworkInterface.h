/*************************************************************************
 > File Name: NetworkInterface.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 07 Dec 2014 04:16:58 AM PST
 ************************************************************************/
#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "Cluster.h"
#include "ClusterHandler.h"
#include "pdfs.h"

namespace pdfs{
class NetworkInterface
{
    protected:
        Cluster _cluster;
        ThreadPool* _workerPool;
    public:
        /////////////////////MetaData Operation////////////////////////////
        // Step 1: Send push/pull metadata request by sockfd
        virtual ssize_t sendPushMetadataRequest(int sockfd, 
                Metadata &metadata) = 0;
        virtual ssize_t sendPullMetadataRequest(int sockfd,
                Metadata &metadata) = 0;
        // Step 2: Send/receive metadata to/from a socket by sockfd
        // Invoked by pdfs
        virtual ssize_t putMetadata(int sockfd, Metadata &metadata) = 0;
        virtual ssize_t getMetadata(int sockfd, Metadata &metadata) = 0;
        // Get/Put file metadata:pair<filename, destIP> from/to somewhere
        virtual ssize_t pushMetadata(Metadata &metadata, std::string dest) = 0;
        virtual ssize_t pullMetadata(Metadata &metadata) = 0;
        ////////////////////////////////////////////////////////////////////
            
        /////////////////////File Operation/////////////////////////////////
        // Step 1: Send push/pull file request by sockfd
        // Invoked by pdfs, filename without any path info
        virtual ssize_t sendPushFileRequest(int sockfd, Metadata &metadata,
                unsigned int headerFlag = 0) = 0;
        virtual ssize_t sendPullFileRequest(int sockfd, 
                std::string filename) = 0;
        // Step 2: Send/receive a file to/from a socket
        // Invoked by pdfs
        virtual ssize_t putFile(int sockfd, std::string filename) = 0;
        virtual ssize_t getFile(int sockfd, std::string filename, 
                size_t file_size) = 0;
        // Step 3: 1 + 2 => Push/Pull a file to/from somewhere 
        // Invoked by pdfsc(as clients)
        virtual ssize_t pushFile(std::string filename) = 0; 
        virtual ssize_t pullFile(std::string filename) = 0;
        ////////////////////////////////////////////////////////////////////
        
        ////////////////////// Backup file ////////////////////////////////
        virtual ssize_t pushFile(Metadata &metadata, std::string dest) = 0;
        ////////////////////////////////////////////////////////////////////

        // Verify the transmission
        //virtual ssize_t putFileMDCheck() = 0;
        //virtual ssize_t getFileMDCheck() = 0;
        
        // Connect for client to connect 
        ssize_t connect();
        
        // Constructor and Reconstructor
        NetworkInterface(); 
        NetworkInterface(Cluster &cluster);
        virtual ~NetworkInterface(){
            if (nullptr != _workerPool)
            {
                _workerPool->destroyPool(2);
            }
        }
        Cluster& getCluster()
        {
            return _cluster;
        }
};// end of class NetworkInterface 
} // end of namespace pdfs
#endif
