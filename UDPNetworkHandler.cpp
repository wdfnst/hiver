/*************************************************************************
 > File Name: UDPNetworkHandler.cpp
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:24 AM PST
 ************************************************************************/
#include "UDPNetworkHandler.h"

namespace pdfs {

unsigned UDPNetworkHandler::executeThis()
{
    char recvbuf[pdfs::MESSAGE_SIZE + 1];
    struct sockaddr_in client_addr;
    int cli_len = sizeof(client_addr);
    memset(recvbuf, '\0', pdfs::MESSAGE_SIZE + 1);
    UDTSOCKET client = UDT::accept(sock_fd, (sockaddr*)&client_addr, &cli_len);
    if (UDT::ERROR == UDT::recv(client, recvbuf, sizeof(recvbuf), 0))
    {
        std::cerr << "send: " << UDT::getlasterror().getErrorMessage() << endl;
        return -1;
    }

    struct messageheader header;
    memcpy(&header, recvbuf, sizeof(messageheader));
    switch(header.optype)
    {
        // Push file
        case 0:
            {
                UDPNetworkInterface udpnet;
                std::clog << "4)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", Recv pushFile Req(" << header.blockid << ")-->\n";
                Metadata metadata;
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(metadata));
                if (UDT::ERROR != udpnet.getFile(client, UPLOAD_FOLDER + 
                            metadata.filename, metadata.size))
                    DuplicationManager::backupFile(udpnet, metadata);
                else
                    std::cerr << "File receive failed in case 0.\n";
            }
            break;
        // Pull file 
        case 1:
            // TODO: file not exists
            {
                std::clog << "4)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", Recv pullFile Req(" << header.blockid << ")-->\n";
                UDPNetworkInterface udpnet;
                udpnet.putFile(client, UPLOAD_FOLDER + 
                        std::string(header.blockid));
            }
            break;
        // Storing metadata
        case 2:
            {
                std::clog << "1)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", recv pushmetadata(" << header.blockid << ")-->\n";
                Metadata metadata;
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(metadata));
                LocalFileSystem lfs;
                if (-1 != lfs.writeMetadata(metadata))
                {
                    UDPNetworkInterface udpnet;
                    DuplicationManager::backupMetadata(udpnet, metadata);
                }
            }
            break;
        // PutMetadata
        case 3:
            {
                std::clog << "1)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ",recv pullFileMetadata Req(" << header.blockid << ")--> ";
                LocalFileSystem lfs;
                Metadata metadata = lfs.readMetadata(header.blockid);
                if (0 != strcmp(metadata.filename, ""))
                {
                    UDPNetworkInterface udpnetinterface;
                    udpnetinterface.putMetadata(client, metadata);
                }
            }
            break;
        // Backup File
        case 10:
            {
                UDPNetworkInterface udpnet;
                std::clog << "4)From:" << inet_ntoa(client_addr.sin_addr) << 
                    ", Recv backup File Req(" << header.blockid << ")-->\n";
                Metadata metadata;
                memset(&metadata, '\0', sizeof(metadata));
                memcpy(&metadata, recvbuf + sizeof(header), sizeof(metadata));
                if (UDT::ERROR == udpnet.getFile(client, UPLOAD_FOLDER + 
                            metadata.filename, metadata.size))
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
    return(0);
} // end of function executeThis
} // end namespace

