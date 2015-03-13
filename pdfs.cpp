/*************************************************************************
 > File Name: pdfs.cpp
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 01:44:36 PM PST
 ************************************************************************/
#include "pdfs.h"
#include "ThreadPool.h"
#include "Handler.h"
#include "TCPNetworkHandler.h"
#include "UDPNetworkHandler.h"
#include "UDPNetwork.h"
#include "TCPNetwork.h"
#include "BroadcastHandler.h"
#include "Cluster.h"
#include "SClusterHandler.h"
#include "LocalFileSystem.h"

int main()
{
    // Create file upload folder 
    pdfs::LocalFileSystem lfs;
    if (lfs.checkFileExists(pdfs::UPLOAD_FOLDER) < 0)
    {
        lfs.createDir(pdfs::UPLOAD_FOLDER);
    }
    if (lfs.checkFileExists(pdfs::METADATA_FOLDER) < 0)
            lfs.createDir(pdfs::METADATA_FOLDER);

    // cluster to maintain global cluster info
    pdfs::Cluster cluster;

    // Worker thread pool
    ThreadPool* myPool = new ThreadPool(pdfs::THREADPOOL_SIZE);
    myPool->initializeThreads();

    // Initialize the epoll, get transfer mediums
    NETWORK net;

    // start monitoring node status
    size_t thread_id = 0;
    myPool->assignWork(new pdfs::SClusterHandler(thread_id++, 0));
    myPool->assignWork(new pdfs::BroadcastHandler(thread_id++, 0));
#ifndef UDT_MEDIA 
    int listener = net.getListener(); 
    int epfd = net.getEpollfd(listener);
    struct epoll_event events[pdfs::MAXEPOLLSIZE];
    while (1) 
    {
        // Wait event to happy
        int nfds = epoll_wait(epfd, events, 10000, -1);
        if (nfds == -1)
        {
            perror("epoll wait err");
            break;
        }
        // Add the work to work thread pool queue
        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == listener)
                myPool->assignWork(new NETWORK_HANDLER(thread_id++, listener));
        }
    }
#else
    int listener = net.getUDTListener(); 
    int epfd = net.getUDTEpollfd(listener);
    std::set<UDTSOCKET> readfds, writefds;
    while (1)
    {
        int ret = UDT::epoll_wait(epfd, &readfds, &writefds, -1, NULL);
        if (ret > 0)
        {
            for (auto i = readfds.begin(); i != readfds.end(); i++)
            {
                myPool->assignWork(new NETWORK_HANDLER(thread_id++, listener));
            }
        }
    }
#endif
    close(listener);
    close(epfd);
    myPool->destroyPool(2);
    delete myPool;
    return 0;
}
