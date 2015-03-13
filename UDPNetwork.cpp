/*************************************************************************
 > File Name: UDPNetwork.cpp
 > Author: test
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 07:28:11 AM PST
 ************************************************************************/
#include "UDPNetwork.h"
#include "Handler.h"
namespace pdfs{

int UDPNetwork::setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
	{
		return -1;
	}
	return 0;
}
struct sockaddr_in 
UDPNetwork::getSockAddr(unsigned long ip, unsigned short port)
{
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = PF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = ip;
    return sock_addr;
}

struct sockaddr_in 
UDPNetwork::getSockAddr(std::string ip, unsigned short port)
{
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = PF_INET;
    sock_addr.sin_port = htons(port);
    //sock_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    inet_pton(AF_INET, ip.c_str(), &sock_addr.sin_addr.s_addr);
    return sock_addr;
}
int UDPNetwork::getRawSocket()
{
    int listener = -1;
    if ((listener = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket create failed ！");
        exit(1);
    }
    int opt = SO_REUSEADDR;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setnonblocking(listener);
    return listener;
}
int UDPNetwork::getUDTRawSocket()
{
    UDTSOCKET listener = -1;
    if ((listener = UDT::socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket create failed ！");
        exit(1);
    }
    return listener;
}
int UDPNetwork::getListener(unsigned short port, unsigned long ip)
{
    int listener = getRawSocket();
    int opt = SO_REUSEADDR;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setnonblocking(listener);
    struct sockaddr_in sock_addr = getSockAddr(ip, port);
    if (bind(listener, (struct sockaddr *) &sock_addr, 
                sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return -1;
    } 
    return listener;
}
int UDPNetwork::getUDTListener(unsigned short port, unsigned long ip)
{
    UDTSOCKET listener = getUDTRawSocket();
    sockaddr_in sock_addr = getSockAddr(ip, port);
    if (UDT::bind(listener, (struct sockaddr *) &sock_addr, 
                sizeof(struct sockaddr)) == -1)
    {
        perror("bind");
        return -1;
    }
    UDT::listen(listener, 10);
    return listener;
}
int UDPNetwork::getEpollfd(int udp_sfd)
{
    struct epoll_event ev;
    int epfd = epoll_create(MAXEPOLLSIZE);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = udp_sfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, udp_sfd, &ev) < 0) 
    {
        perror("epoll_ctl_add");
    }
    return epfd;
}
int UDPNetwork::getUDTEpollfd(UDTSOCKET udp_sfd)
{
    int eid = UDT::epoll_create();
    int events = EPOLLIN;
    if (UDT::epoll_add_usock(eid, udp_sfd, &events) == UDT::ERROR)
    {
        perror("add_usock");
    }
    return eid;
}
int UDPNetwork::getConnection(std::string destIP, unsigned short destPort)
{
    int sockfd = getRawSocket();
    struct sockaddr_in server_addr = getSockAddr(destIP, destPort);
	if(-1 == connect(sockfd, (struct sockaddr *)(&server_addr), 
                sizeof(struct sockaddr)))
	{
        perror("connect");
	}
    return sockfd;
}
int UDPNetwork::getUDTConnection(std::string destIP, unsigned short destPort)
{
    UDTSOCKET sockfd = getUDTRawSocket();
    struct sockaddr_in server_addr = getSockAddr(destIP, destPort);
	if(-1 == UDT::connect(sockfd, (struct sockaddr *)(&server_addr), 
                sizeof(server_addr)))
	{
        std::cerr << "connect: " << UDT::getlasterror().getErrorMessage();
	}
    return sockfd;
}
int UDPNetwork::replyAck(struct sockaddr_in &addr, char *replyMsg)
{
    // Reply the ack message
    int ack_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(ack_sockfd < 0)
    {  
       perror("get socket");
    }
    addr.sin_port = htons(pdfs::ACK_PORT);
    sendto(ack_sockfd, replyMsg, HEADER_SIZE, 0, 
            (struct sockaddr*)&addr, 
            sizeof(struct sockaddr_in));
    close(ack_sockfd);
    //size_t offset = *((int*)(replyMsg + 40)) * 1.0 / pdfs::MAX_DATA_SIZE;
    //printf(">>send ack, offset: %d\n", offset);
    return 0;
}
} // end namespace pdfs
