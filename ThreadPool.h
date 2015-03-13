/*************************************************************************
 > File Name: threadpoll.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Thu 04 Dec 2014 08:55:22 PM PST
 ************************************************************************/

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <unistd.h>

#include <semaphore.h>
#include <iostream>
#include <vector>

using namespace std;
/*
WorkerThread class
This class needs to be sobclassed by the user.
*/
class WorkerThread{
public:
    int id;

    unsigned virtual executeThis()
	{
		return 0;
	}

    WorkerThread(int id) : id(id) {}
    virtual ~WorkerThread(){}
};

/*
ThreadPool class manages all the ThreadPool related activities. 
This includes keeping track of idle threads and ynchronizations between all threads.
*/
class ThreadPool{
public:
    ThreadPool();
    ThreadPool(int maxThreadsTemp);
    virtual ~ThreadPool();
	
	void destroyPool(int maxPollSecs);

    bool assignWork(WorkerThread *worker);
    bool fetchWork(WorkerThread **worker);

	void initializeThreads();
	
    static void *threadExecute(void *param);
    
    static pthread_mutex_t mutexSync;
    static pthread_mutex_t mutexWorkCompletion;
    
private:
    int maxThreads;
    
//     pthread_cond_t  condCrit;
    sem_t availableWork;
    sem_t availableThreads;

    //WorkerThread ** workerQueue;
    vector<WorkerThread *> workerQueue;

    int topIndex;
    int bottomIndex;
	
	int incompleteWork;

    int queueSize;

};

#endif

