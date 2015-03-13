/*************************************************************************
 > File Name: TCPNetworkInterface.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 07 Dec 2014 04:17:07 AM PST
 ************************************************************************/
#include "TCPNetworkInterface.h"

namespace pdfs {

TCPNetworkInterface::TCPNetworkInterface() 
    : NetworkInterface()
{ }

TCPNetworkInterface::TCPNetworkInterface(Cluster &cluster) 
    : NetworkInterface(cluster)
{ }

TCPNetworkInterface::~TCPNetworkInterface()
{ }

ssize_t TCPNetworkInterface::sendPushMetadataRequest(int sockfd, 
        Metadata &metadata)
{
    return 0;
}
ssize_t TCPNetworkInterface::sendPullMetadataRequest(int sockfd, 
        Metadata &metadata)
{
    char headerbuf[HEADER_SIZE];
    struct messageheader header = {3, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, BLOCKID_SIZE);
    bzero(headerbuf, HEADER_SIZE);
    memcpy(headerbuf, &header, sizeof(header));
    if(-1 == write(sockfd, headerbuf, HEADER_SIZE))
    {
        std::cerr << "write failed.\n";
        return -1;
    }
    return HEADER_SIZE;
}
ssize_t TCPNetworkInterface::putMetadata(int sockfd, Metadata &metadata)
{
    ssize_t sendBytes = 0;
    char buffer[MESSAGE_SIZE];
    bzero(buffer, MESSAGE_SIZE);
    memcpy(buffer, reinterpret_cast<char*>(&metadata), sizeof(metadata));
    std::clog << "2)Sending metadata...-->\n";
    if((sendBytes = send(sockfd, buffer, sizeof(metadata), 0)) < 0)
    {
        std::cerr << "Sent metadata error.\n";
    }
    std::clog << "3)Finish sending metadata(" << sendBytes << "B)-->\n" 
        << std::endl;
    return sendBytes;
}

ssize_t TCPNetworkInterface::getMetadata(int sockfd, Metadata &metadata) 
{
    char buffer[MESSAGE_SIZE + 1];
    bzero(buffer, MESSAGE_SIZE + 1);
    ssize_t recvBytes = 0;
    std::clog << "2)Receiving metadata...-->\n";
    if((recvBytes = recv(sockfd, buffer, MESSAGE_SIZE, 0)) > 0)
    {
        memcpy(&metadata, buffer, recvBytes);
    }
    std::clog << "3)Finish Receiving metadata(size:" << recvBytes <<
        ")-->\n" << std::endl;
    return recvBytes;
}

ssize_t TCPNetworkInterface::pushMetadata(Metadata &metadata, std::string dest)
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
    TCPNetwork net;
    int sockfd = net.getConnection(dest);
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
    memcpy(buffer + sizeof(header), &metadata, sizeof(Metadata));
    ssize_t sendSize = -1;
    if((sendSize = send(sockfd, buffer, sizeof(metadata) + sizeof(header), 0))
            < 0)
        perror("send metadata");
    std::clog << "3)Send to:" << dest << " metadata(" << 
        metadata.filename << ":" << sendSize << "B)-->\n";
    close(sockfd);
    return sendSize;
}

ssize_t TCPNetworkInterface::pullMetadata(Metadata &metadata)
{
    std::vector<std::string> nodes = _cluster.getMetaNode(metadata.filename);
    if (nodes.size() <= 0)
    {
        std::cerr << "No metadata node found.\n";
        return -1;
    }
    TCPNetwork net;
    int sockfd = net.getConnection(nodes[0]);
    if (sockfd == -1)
    {
        std::cerr << "Connect to metadata node failed.\n";
        return -1;
    }
    memcpy(metadata.destnode, nodes[0].c_str(), nodes[0].size());
    if (sendPullMetadataRequest(sockfd, metadata) == -1)
    {
        std::cerr << "Get metadata node failed.\n";
        bzero(metadata.destnode, sizeof(metadata.destnode)); 
        return -1;
    }
    ssize_t recvBytes = getMetadata(sockfd, metadata);
    close(sockfd);
    return recvBytes;
}

/////////////////////////File Operation/////////////////////////////
ssize_t TCPNetworkInterface::getFile(int sockfd, 
        std::string filename, size_t file_size)
{
    ssize_t recvSize  = 0;
    size_t total_size = 0;
    char buffer[MESSAGE_SIZE + 1];
    bzero(buffer, MESSAGE_SIZE + 1);
    ofstream fout(filename);
    if (!fout.is_open())
    {
        std::cerr << "Open file failed in getFile().\n";
        return -1;
    }
    std::clog << "5)Receiving file...-->\n"; 
    while((recvSize = recv(sockfd, buffer, MESSAGE_SIZE, 0)) > 0)
    {
        fout.write(buffer, recvSize);
        total_size += recvSize;
    }
    fout.close();
    std::clog << "6)Finish Receiving(" << total_size << "B)" << std::endl; 
    if (total_size != file_size)
    {
        std::cerr << "++Get file error in getFile(), sendSize: fileSize ?= "
            << total_size << ":" << file_size << std::endl;
        return -1;
    }
    return total_size;
} // end of function recvFile

ssize_t TCPNetworkInterface::putFile(int sockfd, std::string filename)
{
    size_t total_size = 0;
    char buffer[MESSAGE_SIZE];
    bzero(buffer, MESSAGE_SIZE);
    std::ifstream fin(filename);
    if (!fin.is_open())
    {
        std::cerr << "Open file failed in putFile().\n";
        return -1;
    }
    std::clog << "5)Sending file...-->\n";
    ssize_t sendSize = -1;
    while(!fin.eof())
    {
        fin.read(buffer, MESSAGE_SIZE); 
        if((sendSize = send(sockfd, buffer, fin.gcount(), 0)) < 0)
        {
            std::cerr << "send file error in putFile().\n";
            break;
        }
        total_size += sendSize;
        bzero(buffer, MESSAGE_SIZE);
    }
    std::clog << "6)Finish Sending file(" << total_size << 
        "B)" << std::endl;
    fin.close();
    return total_size;
}

ssize_t TCPNetworkInterface::sendPushFileRequest(int sockfd, 
        Metadata &metadata, unsigned int headerFlag)
{
    char buffer[MESSAGE_SIZE];
    bzero(buffer, MESSAGE_SIZE);
    struct messageheader header = {headerFlag, "Halloween", 0, 0, 0};
    memcpy(header.blockid, metadata.filename, BLOCKID_SIZE);
    memcpy(buffer, &header, sizeof(header));
    memcpy(buffer + sizeof(header), &metadata, sizeof(Metadata));
    if(-1 == write(sockfd, buffer, sizeof(header) + sizeof(Metadata)))
    {
        std::cerr << "write faile.\n";
        return -1;
    }
    return HEADER_SIZE;
}
ssize_t TCPNetworkInterface::sendPullFileRequest(int sockfd, std::string 
        filename)
{
    char headerbuf[HEADER_SIZE];
    bzero(headerbuf, HEADER_SIZE);
    struct messageheader header = {1, "Halloween", 0, 0, 0};
    std::string basename = LocalFileSystem::getBasename(filename);
    memcpy(header.blockid, basename.c_str(), BLOCKID_SIZE);
    memcpy(headerbuf, &header, sizeof(header));
    if(-1 == write(sockfd, headerbuf, HEADER_SIZE))
    {
        perror("socket write failed");
        return -1;
    }
    
    return HEADER_SIZE;
}
ssize_t TCPNetworkInterface::pushFile(std::string filename)
{
    std::vector<std::string> topK = _cluster.getTopKNode();
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
    TCPNetwork net;
    int sockfd = net.getConnection(topK[0]);
    if (sockfd == -1)
    {
        std::cerr << "Connect to file node failed.\n";
        close(sockfd);
        return -1;
    }
    if (-1 == sendPushFileRequest(sockfd, metadata, 0))
    {
        std::cerr << "Send push Request failed.\n";
        close(sockfd);
        return -1;
    }
    if (-1 == recvStartCommd(sockfd))
    {
        std::cerr << "recv start put command failed in pushFile().\n";
        close(sockfd);
        return -1;
    }
    ssize_t sendBytes = putFile(sockfd, filename);
    if ("" == recvCheckFileMD5(sockfd, filename))
    {
        std::cerr << "recv&check push request failed in pushFile().\n";
    }
    close(sockfd);
    return sendBytes;
}
ssize_t TCPNetworkInterface::pushFile(Metadata &metadata, std::string dest) 
{
    TCPNetwork net;
    int sockfd = net.getConnection(dest);
    if (sockfd == -1)
    {
        std::cerr << "Connect to file node failed.\n";
        close(sockfd);
        return -1;
    }
    if (-1 == sendPushFileRequest(sockfd, metadata, 10))
    {
        std::cerr << "Send push Request failed.\n";
        close(sockfd);
        return -1;
    }
    if (-1 == recvStartCommd(sockfd))
    {
        std::cerr << "recv start put command failed in pushFile().\n";
        close(sockfd);
        return -1;
    }
    ssize_t sendBytes = putFile(sockfd, UPLOAD_FOLDER + metadata.filename);
    if ("" == recvCheckFileMD5(sockfd, metadata.filename))
    {
        std::cerr << "recv&check push request failed in pushFile().\n";
    }
    close(sockfd);
    return sendBytes;
}
ssize_t TCPNetworkInterface::pullFile(std::string filename)
{
    std::string basename = LocalFileSystem::getBasename(filename);
    Metadata metadata = {"", "", "", "", 0, 0, 0};
    memcpy(metadata.filename, filename.c_str(), filename.size());
    if (-1 == pullMetadata(metadata))
    {
        std::cerr << "Pull MetaData error.\n";
        return -1;
    }
    TCPNetwork net;
    int sockfd = net.getConnection(metadata.destnode);
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
    ssize_t recvbytes = getFile(sockfd, filename, metadata.size);
            //LocalFileSystem::getFileSize(filename));
    if (-1 == recvbytes)
    {
        std::cerr << "Send file failed in pullFile().\n";
        close(sockfd);
        return -1;
    }
    if (-1 == sendFileMD5(sockfd, filename))
    {
        std::cerr << "Send File MD5 failed in pullFile().\n";
    }
    close(sockfd);
    return recvbytes;
}

// The closed following 4 functions are used to confirm the sent file
ssize_t TCPNetworkInterface::sendStartCommd(int sockfd)
{
    ssize_t sendSize = -1;
    char startFlag[1];
    std::clog << "Send start Command-->\n";
    if((sendSize = send(sockfd, startFlag, sizeof(startFlag), 0)) < 0)
    {
        std::cerr << "Send start Command failed in sendStartCommd().\n";
    }
    return sendSize;
}
ssize_t TCPNetworkInterface::recvStartCommd(int sockfd)
{
    ssize_t recvSize = -1;
    char buffer[1];
    std::clog << "4)Start to recv Command-->\n";
    if ((recvSize = recv(sockfd, buffer, sizeof(buffer), 0)) <= 0)
    {
        std::cerr << "Recv start Command failed in recvStartCommd().\n";
        return -1;
    }
    return recvSize;
}
std::string TCPNetworkInterface::recvCheckFileMD5(int sockfd, 
        const std::string filename)
{
    return "test";
}
ssize_t TCPNetworkInterface::sendFileMD5(int sockfd, 
        const std::string filename)
{
    return 1;
}

} // end of namespace pdfs
