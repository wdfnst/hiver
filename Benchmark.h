/************************************************************
 > File Name: BenchMark.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sat 07 Mar 2015 07:06:13 PM PST
 ***********************************************************/
#include <iostream>
#include <iomanip>
#include <ctime>
#include <memory>
#include <sys/time.h>

#include "pdfs.h"
#include "LocalFileSystem.h"
#include "Cluster.h"
#include "TCPNetworkInterface.h"
#include "UDPNetworkInterface.h"

using namespace std;
using namespace pdfs;

namespace benchmark {
const ssize_t TCP_MODE = 0;
const ssize_t UDP_MODE = 1;

class TimeMeasure;
class Benchmark {
    public:
        Benchmark();
        virtual void test() = 0;
        virtual ~Benchmark();

    protected:
        shared_ptr<Cluster> _cluster;
        shared_ptr<TimeMeasure> _tm;
        shared_ptr<NetworkInterface> _nfi;
};// end of class Benchmark

class ConnectionTime : public Benchmark {
    public:
        ConnectionTime();
        ConnectionTime(Cluster &cluster);
        void test();

    private:
        ssize_t mode;

}; // end of class ConnectionTimeTest

class TransFileTime : public Benchmark {
    public:
        TransFileTime();
        TransFileTime(Cluster &cluster);
        void test();
        void testTransFile(string filename);
        void testTransDir(string dirname);
};

class NetworkUseRate : public Benchmark {
    public:
        NetworkUseRate();
        NetworkUseRate(Cluster &cluster);
        void test();

};

class TimeMeasure {
    public:
        TimeMeasure();
        struct timeval start();
        struct timeval stop();
        double getRuntime();
        struct timeval getBegintime(); 
        struct timeval getEndtime();
        
    private:
        struct timeval begin_time, end_time;
}; // end of class TimeMeasure
} // end of namespace
