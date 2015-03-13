/*************************************************************************
 > File Name: TCPNetworkHandler.cpp
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:24 AM PST
 ************************************************************************/
#include "TCPNetworkHandler.h"

namespace pdfs {

unsigned TCPNetworkHandler::executeThis()
{
    char recvbuf[MESSAGE_SIZE + 1];
    int recvSize = -1;
    struct sockaddr_in client_addr;
    socklen_t client_address_len = sizeof(client_addr);
    int client = accept(sock_fd, (struct sockaddr*)&client_addr,
                    &client_address_len);
    if ((recvSize = recv(client, recvbuf, MESSAGE_SIZE, 0)) <= 0) {
        perror("recv failed.\n");
        return 0;
    }
    struct messageheader header;
    memcpy(&header, recvbuf, sizeof(header));
    switch(header.optype)
    {
        // Push file
        case 0:
            {
                TCPNetworkInterface tcpnet;
                std::clog << "4)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", Recv pushFile Req(" << header.blockid << ")-->\n";
                Metadata metadata;
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(metadata));
                tcpnet.sendStartCommd(client);
                if ( -1 != tcpnet.getFile(client, UPLOAD_FOLDER +
                            header.blockid, metadata.size))
                    DuplicationManager::backupFile(tcpnet, metadata);
                else
                    std::cerr << "File receive failed in case 0.\n";
            }
            break;
        // Pull block 
        case 1:
            // TODO: file not exists
            // Start to pull a from servers
            {
                std::clog << "4)Recv pullFile Req(fileID:" << 
                    header.blockid << ")--> ";
                TCPNetworkInterface tcpnet;
                tcpnet.putFile(client, UPLOAD_FOLDER + 
                        std::string(header.blockid));
            }
            break;
        // Storing file metadata 
        case 2:
            {
                std::clog << "1)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", recv pushmetadata(" << header.blockid << ")-->\n";
                Metadata metadata;
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(Metadata));
                LocalFileSystem lfs;
                if (-1 != lfs.writeMetadata(metadata))
                {
                    TCPNetworkInterface tcpnet;
                    DuplicationManager::backupMetadata(tcpnet, metadata);
                }
            }
            break;
        // Getting metadata 
        case 3:
            {
                std::clog << "1)From:" << inet_ntoa(client_addr.sin_addr)
                    << ", recv pullFile Req(" << header.blockid
                    << ")--> ";
                LocalFileSystem lfs;
                Metadata metadata = lfs.readMetadata(header.blockid);
                if (strcmp(metadata.filename, ""))
                {
                    TCPNetworkInterface tcpnet;
                    tcpnet.putMetadata(client, metadata);
                }
            }
            break;
        // Backup File
        case 10:
            {
                TCPNetworkInterface tcpnet;
                std::clog << "4)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", Recv backup File Req(" << header.blockid << ")-->\n";
                Metadata metadata;
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(metadata));
                tcpnet.sendStartCommd(client);
                if ( -1 == tcpnet.getFile(client, UPLOAD_FOLDER +
                            header.blockid, metadata.size))
                    perror("backup file");
            }
            break;
        // Backup metadata
        case 12:
            {
                std::clog << "1)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", recv backup metadata(" << header.blockid << ")-->\n";
                Metadata metadata;
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(metadata));
                LocalFileSystem lfs;
                if (-1 == lfs.writeMetadata(metadata))
                    perror("backup metadata");
            }
            break;
        default:
            printf("Coming soon...\n");
            break;
    }
    close(client);
    return(0);
} // end of function executeThis
} // end namespace

