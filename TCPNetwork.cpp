/*************************************************************************
 > File Name: TCPNetwork.cpp
 > Author: test
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 07:28:11 AM PST
 ************************************************************************/
#include "TCPNetwork.h"

namespace pdfs{

int TCPNetwork::setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
	{
		return -1;
	}
	return 0;
}

struct sockaddr_in 
TCPNetwork::getSockAddr(unsigned long ip, unsigned short port)
{
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = PF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = ip;
    return sock_addr;
}

struct sockaddr_in 
TCPNetwork::getSockAddr(std::string ip, unsigned short port)
{
    struct sockaddr_in sock_addr;
    bzero(&sock_addr, sizeof(sock_addr));
    sock_addr.sin_family = PF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    return sock_addr;
}

int TCPNetwork::getRawSocket()
{
    int listener = -1;
    /* 开启 socket 监听 */
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket create failed.\n");
        exit(1);
    }

    struct timeval timeout = {3, 0};
    setsockopt(listener, SOL_SOCKET, SO_SNDTIMEO, 
            (char*)&timeout, sizeof(struct timeval));
    setsockopt(listener, SOL_SOCKET, SO_RCVTIMEO, 
            (char*)&timeout, sizeof(struct timeval));
    /*设置socket属性，端口可以重用*/
//     int opt = SO_REUSEADDR;
//     setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//     setnonblocking(listener);
    return listener;
}

int TCPNetwork::getListener(unsigned short port, unsigned long ip)
{
    /*获取并设置socket属性，端口可以重用*/
    int listener = getRawSocket();
    int opt = SO_REUSEADDR;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setnonblocking(listener);
    /*获取监听的sockaddr_in*/
    struct sockaddr_in sock_addr = getSockAddr(ip, port);
    if (bind(listener, (struct sockaddr *) &sock_addr, 
                sizeof(struct sockaddr)) == -1) 
    {
        perror("bind");
        return -1;
    } 
    listen(listener, 128);
    return listener;
}

int TCPNetwork::getEpollfd(int udp_sfd)
{
    struct epoll_event ev;
    //   struct epoll_event events[MAXEPOLLSIZE];
    int epfd = epoll_create(MAXEPOLLSIZE);
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = udp_sfd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, udp_sfd, &ev) < 0) 
    {
        fprintf(stderr, "epoll set insertion error: fd=%d\n", udp_sfd);
        return -1;
    }
    return epfd;
}

int TCPNetwork::getConnection(std::string destIP, unsigned short destPort)
{
    int sockfd = getRawSocket();
    struct sockaddr_in server_addr = getSockAddr(destIP, destPort);
	if(-1 == connect(sockfd,(struct sockaddr *)(&server_addr), 
                sizeof(struct sockaddr)))
	{
		perror("connect fail");
		return -1;
	}
    return sockfd;
}
} // end namespace pdfs
