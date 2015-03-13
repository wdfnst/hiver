/*************************************************************************
 > File Name: UDPNetworkInterface.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 07 Dec 2014 04:17:07 AM PST
 ************************************************************************/
#include "UDPNetworkInterface.h"

namespace pdfs {

UDPNetworkInterface::UDPNetworkInterface() 
    : NetworkInterface()
{ }

UDPNetworkInterface::UDPNetworkInterface(Cluster &cluster) 
    : NetworkInterface(cluster)
{ }

UDPNetworkInterface::~UDPNetworkInterface()
{ }

ssize_t UDPNetworkInterface::sendPushMetadataRequest(int sockfd, 
                Metadata &metadata)
{
    return 0;
}
ssize_t UDPNetworkInterface::sendPullMetadataRequest(int sockfd,
                Metadata &metadata)
{
    char headerbuf[HEADER_SIZE];
    struct messageheader header = {3, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, BLOCKID_SIZE);
    memset(headerbuf, '\0', HEADER_SIZE);
    memcpy(headerbuf, &header, sizeof(header)); 
    if(UDT::ERROR == UDT::send(sockfd, headerbuf, sizeof(headerbuf), 0))
    {
        std::cerr << "send pullmetadata request:" << 
            UDT::getlasterror().getErrorMessage();
        return -1;
    }
    return HEADER_SIZE;
}
ssize_t UDPNetworkInterface::putMetadata(int sockfd, Metadata &metadata)
{
    char buffer[MESSAGE_SIZE];
    memset(buffer, '\0', MESSAGE_SIZE);
    struct messageheader header = {3, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, sizeof(metadata.filename));
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), &metadata, sizeof(metadata));
    ssize_t sendSize = -1;
    if((sendSize = UDT::send(sockfd, buffer, sizeof(metadata) + 
                    sizeof(header), 0)) < 0)
    {
        std::cerr << "Sent metadata error.\n";
    }
    std::clog << "3)Finish sending metadata(" << metadata.filename << 
        ":" << sendSize << ")-->\n";
    UDT::close(sockfd);
    return sendSize;
}
ssize_t UDPNetworkInterface::getMetadata(int sockfd, Metadata &metadata)
{
    char buffer[MESSAGE_SIZE + 1];
    memset(buffer, '\0', MESSAGE_SIZE + 1);
    ssize_t recvBytes = 0;
    std::clog << "2)Receiving metadata...->";
    if((recvBytes = UDT::recv(sockfd, buffer, MESSAGE_SIZE, 0)) > 0)
    {
        memcpy(&metadata, buffer + sizeof(messageheader), sizeof(metadata));
    }
    std::clog << "3)Finish Receiving metadata(" << metadata.filename <<
        ":" << recvBytes << ")-->\n";
    return recvBytes;
}
ssize_t UDPNetworkInterface::pushMetadata(Metadata &metadata, std::string dest)
{
    unsigned int msgType = 12;
    if (dest == "")
    {
        std::vector<std::string> nodes = _cluster.getMetaNode(
                metadata.filename);
        if (nodes.size() <= 0)
        {
            std::cerr << "No metadata node found.\n";
            return -1;
        }
        dest = nodes[0];
        msgType = 2;
    }
    UDPNetwork net;
    UDTSOCKET sockfd = net.getUDTConnection(dest);
    if (sockfd == -1)
    {
        std::cerr << "Connect to metadata node failed.\n";
        return -1;
    }
    char buffer[MESSAGE_SIZE];
    memset(buffer, '\0', MESSAGE_SIZE);
    struct messageheader header = {msgType, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, sizeof(metadata.filename));
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), &metadata, sizeof(metadata));
    ssize_t sendSize = -1;
    if((sendSize = UDT::send(sockfd, buffer, sizeof(metadata) + 
                    sizeof(header), 0)) < 0)
    {
        std::cerr << "Sent metadata error.\n";
    }
    std::clog << "3)Send to:" << dest << " metadata(" << 
        metadata.filename << ":" << sendSize << ")-->\n";
    UDT::close(sockfd);
    return sendSize;
}

ssize_t UDPNetworkInterface::pullMetadata(Metadata &metadata)
{
    std::vector<std::string> nodes = Cluster::getMetaNode(metadata.filename);
    if (nodes.size() <= 0)
    {
        std::cerr << "No metadata node found.\n";
        return -1;
    }
    UDPNetwork net;
    int sockfd = net.getUDTConnection(nodes[0]);
    if (sockfd == -1)
    {
        std::cerr << "Connect to metadata node failed.\n";
        return -1;
    }
    if (sendPullMetadataRequest(sockfd, metadata) == -1)
    {
        std::cerr << "Get metadata node failed.\n";
        memset(metadata.destnode, '\0', sizeof(metadata.destnode)); 
    }
    ssize_t recvSize = getMetadata(sockfd, metadata);
    UDT::close(sockfd);
    return recvSize;
}
//////////////////////File Operation//////////////////////
ssize_t UDPNetworkInterface::sendPushFileRequest(int sockfd, 
        Metadata &metadata, unsigned int headerFlag)
{
    char buffer[MESSAGE_SIZE];
    memset(buffer, '\0', MESSAGE_SIZE);
    struct messageheader header = {headerFlag, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, sizeof(metadata.filename));
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), &metadata, sizeof(metadata));
    if(UDT::ERROR == UDT::send(sockfd, buffer, sizeof(buffer), 0))
    {
        std::cerr << "write faile.\n";
        return -1;
    }
    return sizeof(header) + sizeof(metadata);
}
ssize_t UDPNetworkInterface::sendPullFileRequest(int sockfd, 
        std::string filename)
{
    char headerbuf[HEADER_SIZE];
    memset(headerbuf, '\0', HEADER_SIZE);
    struct messageheader header = {1, "Halloween", 0, 0, 0};
    std::string basename = LocalFileSystem::getBasename(filename);
    memcpy(header.blockid, basename.c_str(), BLOCKID_SIZE);
    memcpy(headerbuf, &header, sizeof(header));
    if(UDT::ERROR == UDT::send(sockfd, headerbuf, sizeof(headerbuf), 0))
    {
        std::cerr << "send pull file request: " << 
            UDT::getlasterror().getErrorMessage();
        return -1;
    }
    return HEADER_SIZE;
}
ssize_t UDPNetworkInterface::putFile(int sockfd, std::string filename)
{
    size_t total_size = 0;
    char buffer[MESSAGE_SIZE];
    memset(buffer, '\0', MESSAGE_SIZE);
    fstream fin(filename, ios::in | ios::binary);
    if (!fin.is_open())
    {
        std::cerr << "Open file failed in putFile().\n";
        return -1;
    }
    std::clog << "5)Sending file...-->\n";
    fin.seekg(0, ios::end);
    int64_t size = fin.tellg();
    fin.seekg(0, ios::beg);
    int64_t offset = 0;
    if (UDT::ERROR == (total_size = UDT::sendfile(sockfd, fin, offset, size)))
    {
        std::cerr << "sendfile: " << UDT::getlasterror().getErrorMessage();
        total_size = -1;
    }
    std::clog << "6)Finish Sending file...-->\n";
    fin.close();
    return total_size;
}
ssize_t UDPNetworkInterface::getFile(int sockfd, std::string filename, 
                size_t file_size)
{
    size_t total_size = 0;
    char buffer[MESSAGE_SIZE + 1];
    memset(buffer, '\0', MESSAGE_SIZE + 1);
    fstream fout(filename, ios::out | ios::binary);
    if (!fout.is_open())
    {
        std::cerr << "Open file failed in getFile().\n";
        return -1;
    }
    std::clog << "5)Receiving file...-->\n"; 
    int64_t offset = 0;
    if (UDT::ERROR == (total_size = UDT::recvfile(sockfd, fout, offset, 
                    file_size)))
    {
        std::cerr << "recvfile: " << UDT::getlasterror().getErrorMessage();
        return -1;
    }
    if (total_size != file_size)
    {
        std::cerr << "++Get file error in getFile(), sendSize: fileSize ?= "
            << total_size << ":" << file_size << std::endl;
        return -1;
    }
    std::clog << "6)Finish Receiving file...-->\n"; 
    return 0;
}

ssize_t UDPNetworkInterface::pushFile(std::string filename) 
{
    std::vector<std::string> topK = Cluster::getTopKNode();
    if (topK.size() <= 0)
    {
        std::cerr << "Find topK failed.\n";
        return -1;
    }
    std::string basename = LocalFileSystem::getBasename(filename);
    std::string owner = LocalFileSystem::getFileOwner(filename);
    std::string md5   = LocalFileSystem::getFileMD5(filename);
    Metadata metadata = {"", "", "", "", 
        LocalFileSystem::getFileSize(filename), 0, 0};
    for (int i = 0; i < topK.size(); i++)
        memcpy(metadata.destnode + i * IPSTRLEN, topK[i].c_str(), 
                topK[i].size());
    memcpy(metadata.filename, basename.c_str(), basename.size());
    memcpy(metadata.owner, owner.c_str(), owner.size());
    memcpy(metadata.md5, md5.c_str(), md5.size());
    if (-1 == pushMetadata(metadata))
    {
        std::cerr << "Push MetaData error.\n";
        return -1;
    }
    UDPNetwork net;
    std::string dest(metadata.destnode, metadata.destnode + IPSTRLEN);
    UDTSOCKET sockfd = net.getUDTConnection(dest);
    if (sockfd == -1)
    {
        std::cerr << "Connect to file node failed.\n";
    }
    if (-1 == sendPushFileRequest(sockfd, metadata))
    {
        std::cerr << "Send push Request failed.\n";
    }
    ssize_t sendBytes = putFile(sockfd, filename);
    if ("" == recvCheckFileMD5(sockfd, filename))
        std::cerr << "recv&check push request failed in pushFile().\n";
    UDT::close(sockfd);
    return sendBytes;
}

// Used to backup file to dest node, fetch file from "upload/"
ssize_t UDPNetworkInterface::pushFile(Metadata &metadata, std::string dest) 
{
    UDPNetwork net;
    UDTSOCKET sockfd = net.getUDTConnection(dest);
    if (sockfd == -1)
    {
        std::cerr << "Connect to file node jailed.\n";
    }
    if (-1 == sendPushFileRequest(sockfd, metadata, 10))
    {
        std::cerr << "Send push Request failed.\n";
    }
    ssize_t sendBytes = putFile(sockfd, UPLOAD_FOLDER + metadata.filename);
    if ("" == recvCheckFileMD5(sockfd, metadata.filename))
        std::cerr << "recv&check push request failed in pushFile().\n";
    UDT::close(sockfd);
    return sendBytes;
}

ssize_t UDPNetworkInterface::pullFile(std::string filename)
{
    std::string basename = LocalFileSystem::getBasename(filename);
    Metadata metadata = {"", "", "", "", 0, 0, 0};
    memcpy(metadata.filename, filename.c_str(), filename.size());
    if (-1 == pullMetadata(metadata))
    {
        std::cerr << "Pull MetaData error.\n";
        return -1;
    }
    UDPNetwork net;
    std::string dest(metadata.destnode, metadata.destnode + IPSTRLEN);
    UDTSOCKET sockfd = net.getUDTConnection(dest);
    if (sockfd == -1)
    {
        std::cerr << "Connect to file node failed.\n";
        close(sockfd);
        return -1;
    }
    if (-1 == sendPullFileRequest(sockfd, filename))
    {
        std::cerr << "Send pull Request failed.\n";
        close(sockfd);
        return -1;
    }
    ssize_t recvbytes = -1;
    if (-1 == getFile(sockfd, filename, metadata.size))
    {
        std::cerr << "Send file failed in pullFile().\n";
    }
    if (-1 == sendFileMD5(sockfd, filename))
        std::cerr << "Send File MD5 failed in pullFile().\n";
    UDT::close(sockfd);
    return recvbytes;
}

// Send a pass file command
ssize_t UDPNetworkInterface::passFileFromTo(Metadata &metadata, 
        std::string from, std::string to)
{
    char buffer[MESSAGE_SIZE];
    memset(buffer, '\0', MESSAGE_SIZE);
    struct messageheader header = {13, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, sizeof(metadata.filename));
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), &metadata, sizeof(Metadata));
    ssize_t sendSize = -1;
    UDPNetwork net;
    UDTSOCKET sockfd = net.getUDTConnection(from);
    if (sockfd == -1)
    {
        std::cerr << "Connect to from node failed.\n";
        return -1;
    }
    if((sendSize = UDT::send(sockfd, buffer, sizeof(Metadata) + 
                    sizeof(header), 0)) < 0)
    {
        std::cerr << "Sent passFileMsg error.\n";
    }
    std::clog << "3)Finish sending passFileMsg(" << metadata.filename <<
        ":" << sendSize << ")-->\n";
    UDT::close(sockfd);
    return sendSize;
}

// The closed following 4 functions are used to confirm the sent file
std::string UDPNetworkInterface::recvCheckFileMD5(int sockfd, 
        const std::string filename)
{
    return "test";
}
ssize_t UDPNetworkInterface::sendFileMD5(int sockfd, 
        const std::string filename)
{
    return 1;
}
} // end of namespace pdfs
