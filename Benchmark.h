/************************************************************
 > File Name: BenchMark.h
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sat 07 Mar 2015 07:06:13 PM PST
 ***********************************************************/
#include <iostream>
#include <ctime>
#include <sys/time.h>

namespace benchmark {

const ssize_t TCP_MODE = 0;
const ssize_t UDP_MODE = 1;

class Benchmark {
    public:
        Benchmark();
        virtual void test() = 0;
        virtual ~Benchmark(){}
};// end of class Benchmark

class ConnectionTime : public Benchmark {
    public:
        ConnectionTime();
        void test();

    private:
        //ssize_t mode;

}; // end of class ConnectionTimeTest

class TransFileTime : public Benchmark {
    public:
        TransFileTime();
        void test();

};

class NetworkUseRate : public Benchmark {
    public:
        NetworkUseRate();
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
