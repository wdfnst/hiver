/*************************************************************************
 > File Name: pull.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 07 Dec 2014 01:40:53 AM PST
 ************************************************************************/
#include <stdio.h>

#include "Cluster.h"
#include "TCPNetworkInterface.h"
#include "UDPNetworkInterface.h"

using namespace pdfs;

int main(int argc, char**argv)
{
    if(argc != 2)
    {
       fprintf(stderr,"Usage:%s filename\n",argv[0]);
       exit(1);
    }

    // Get start time
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);
    long start_ut = ((long)t_start.tv_sec) * 1000 + (long)t_start.tv_usec / 1000;

    Cluster cluster;
//     NetworkInterface *nfi = new UDPNetworkInterface(cluster);
    NetworkInterface *nfi = new TCPNetworkInterface(cluster);
    nfi->connect();
    (nfi->getCluster()).printClusterMap();
    nfi->pullFile(std::string(argv[1]));

    // Get end time, and publish it
    gettimeofday(&t_end, NULL);
    long end_ut = ((long)t_end.tv_sec) * 1000 + (long)t_end.tv_usec / 1000;
    printf("Time elapsed: %ld milliseconds.\n", end_ut - start_ut);

    delete nfi;
    
    return 0;
}
