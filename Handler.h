/*************************************************************************
 > File Name: Handler.h
 > Author: Weidong, ZHANG 
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sun 30 Nov 2014 10:32:14 AM PST
 ************************************************************************/
#ifndef HANDLER_H
#define HANDLER_H

#include <iostream>
#include <map>

#include "ThreadPool.h"
#include "Cluster.h"
#include "LocalFileSystem.h"

namespace pdfs{

class Handler : public WorkerThread
{
    public:
        int id;
        int sock_fd;

        unsigned virtual executeThis()
        {
            return 0;
        }
        Handler(int id, int sfd) 
            : WorkerThread(id), id(id), sock_fd(sfd)
        {
            //cout << "Creating Handler" << id << "\t address=" << this << endl;
        }
        ~Handler()
        {
            //cout << "Deleting Handler" << id << "\t address=" << this << endl;
        }
}; // end class Handler
} // end namespace
#endif
