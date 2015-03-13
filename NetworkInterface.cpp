/*************************************************************************
 > File Name: NetworkInterface.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Thu 01 Jan 2015 06:50:05 AM PST
 ************************************************************************/
#include "NetworkInterface.h"

namespace pdfs {

NetworkInterface::NetworkInterface()
{
    _workerPool = nullptr;
}

NetworkInterface::NetworkInterface(Cluster &cluster){
    _cluster = cluster;
}

ssize_t NetworkInterface::connect()
{
    struct timeval start, end;
    long seconds, useconds, elapse;
    gettimeofday(&start, NULL);
    /////////////////////////////////
    std::clog << "Connecting...\n";
    _workerPool = new ThreadPool(3);
    _workerPool->initializeThreads();
    _workerPool->assignWork(new ClusterHandler(1, 0));
    size_t cluster_size = std::numeric_limits<int>::max();
    // Wait until two Cluster's size equal
    while (true)
    {
        size_t newClusterSize = _cluster.getClusterSize();
        //std::clog << cluster_size << ":" << newClusterSize << std::endl;
        if (cluster_size == newClusterSize)
        {
            gettimeofday(&end, NULL);
            seconds = end.tv_sec - start.tv_sec;
            useconds = end.tv_usec - start.tv_usec;
            elapse = seconds * 1000 * 1000 + useconds;
            //cout << "Recv time Consumption:" << elapse << "(us)" << endl;
            return 1;
        }
        cluster_size = newClusterSize;
        usleep(600001);
    }
    return -1;
}
} // end of namespace pdfs
