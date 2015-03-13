/*************************************************************************
 > File Name: pdfs.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sat 27 Dec 2014 11:25:36 AM PST
 ************************************************************************/
#ifndef PDFS_H 
#define PDFS_H 

#include <iostream>
#include <bitset>
#include <set>
#include <stdlib.h>
#include "thirdparty/include/udt.h"

// #define UDT_MEDIA
// #define NETWORK pdfs::UDPNetwork
// #define NETWORK_INTERFACE pdfs::UDPNetworkInterface 
// #define NETWORK_HANDLER pdfs::UDPNetworkHandler
#define NETWORK pdfs::TCPNetwork
#define NETWORK_INTERFACE pdfs::TCPNetworkInterface 
#define NETWORK_HANDLER pdfs::TCPNetworkHandler

namespace pdfs {
    // Thread pool size
    const int THREADPOOL_SIZE = 100;
    // Network settings
    const std::string BROADCAST_ADDR  = "10.0.0.255";
    const unsigned short SERVER_PORT  = 2300;
    const unsigned short ACK_PORT     = 2301;
    const unsigned short CLUSTER_PORT = 2302;
    const unsigned short MAXEPOLLSIZE = 48769;
    const size_t RETRY                = 200;
    // 20 is head of IP data packet, 8 is for UDP data packet
    const int MESSAGE_SIZE  = (64 * 1024 - 1) - 20 - 8;
    const int BLOCKID_SIZE  = 32;
    const int HEADER_SIZE   = 48; // sizeof(struct pdfs::messageheader);
    const int MAX_DATA_SIZE = MESSAGE_SIZE - HEADER_SIZE;
    // Max file size 2G
    const unsigned int MAX_FILE_SIZE = 0x10000000;
    // Max number of messaages contained in a file 2G
    // MAX_MESSAGES = ceil(1.0 * MAX_FILE_SIZE / MAX_DATA_SIZE);
    const unsigned int MAX_MESSAGES = 32807;
    // Replicas amount 
    const unsigned short REPLICAS = 3;
    // Folder of upload file
    const std::string UPLOAD_FOLDER = "upload/";
    const std::string METADATA_FOLDER = "metadata/";
    // Node lost expire time: 60'
    const unsigned int NODE_OFFLINE_TIME = 15;
    // New_node expire time: 12' after joining in, node will not new.
    const unsigned int NEWNODE_EXPIRETIME = 12;
    // Define IP address length
    const unsigned int IPSTRLEN = 15;

    enum class msgtype {
        // Push file message
        DATA_PUSH = 0,
        // Pull file message
        DATA_PULL,
        // ls nodes command message
        COMM_LS_NODE,
        // echo test message
        COMM_ECHO
    }; // end of enum class msgtype

    // Definition of message header
    struct messageheader {
        unsigned int optype;
        char blockid[32];
        unsigned int blockdatasize;
        unsigned int msgdataoffset;
        unsigned int msgdatasize;
    }; // end struct msgheader 

    struct Metadata {
        char destnode[IPSTRLEN * REPLICAS];
        char filename[32];
        char owner[64];
        char  md5[32];
        unsigned int size;
        unsigned int ctime;
        unsigned int mtime;
    };

    // Log function, definiation in Cluster.cpp
    const int MSG_LEVEL  = 1;
    const int MSG_INFO   = 0;
    const int MSG_NOTICE = 1;
    const int MSG_WARN   = 2;
    const int MSG_EXCEPT = 3;
    const int MSG_ERROR  = 4;
    void log(int msglevel, std::string msg);
} // end of namespace pdfs
#endif
