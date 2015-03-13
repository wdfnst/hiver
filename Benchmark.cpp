/***********************************************************
 > File Name: Benchmark.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sat 07 Mar 2015 07:07:58 PM PST
 **********************************************************/
#include "Benchmark.h"

using namespace benchmark;

namespace benchmark {
///////////Defination of Benchmark//////////
Benchmark::Benchmark()
{
    _cluster = make_shared<Cluster>();
    _nfi = make_shared<NETWORK_INTERFACE>(*_cluster);
    _tm = make_shared<TimeMeasure>();
}

Benchmark::~Benchmark()
{
}
///////////Defination of ConnectionTime//////////
ConnectionTime::ConnectionTime() : Benchmark()
{
}

void ConnectionTime::test()
{
    _tm->start();
    if (_nfi->connect() == -1)
    {
        std::cout << "No Resource Founded ... ";
        return;
    }
    _tm->stop();
    cout << "Connect consumption:" << _tm->getRuntime() << "s" << endl;
}
///////////Defination of TransFileTime///////////
TransFileTime::TransFileTime() : Benchmark()
{
    if (_nfi->connect() == -1)
        std::cout << "Connect failed...\n";
}

void TransFileTime::test()
{
    testTransDir("testDir");
}

void TransFileTime::testTransFile(string filename)
{
    _tm->start();
    _nfi->pushFile(filename);
    _tm->stop();
    cout << "File:" << setw(12)<< left << filename << " size:" << 
       setw(8) << LocalFileSystem::getFileSize(filename) <<
       " Elapse:" << setw(8) << _tm->getRuntime() << "s" << endl;
}

void TransFileTime::testTransDir(string dirname)
{
    vector<string> filenames = LocalFileSystem::listDirFiles(dirname);
    auto iter = filenames.begin();
    for (; iter != filenames.end(); iter++)
        testTransFile("testDir/" + *iter);
}

///////////Defination of NetworkUseRate//////////
NetworkUseRate::NetworkUseRate() : Benchmark()
{ }

void NetworkUseRate::test()
{
   cout << "Network Usage Rate:0.0%" << endl; 
}
///////////Defination of TimeMeasure/////////////
TimeMeasure::TimeMeasure()
{ }

struct timeval TimeMeasure::start()
{
    gettimeofday(&begin_time, NULL);
    return begin_time;
}
struct timeval TimeMeasure::stop()
{
    gettimeofday(&end_time, NULL);
    return end_time;
}
double TimeMeasure::getRuntime()
{
    long seconds = end_time.tv_sec - begin_time.tv_sec;
    long useconds = end_time.tv_usec - begin_time.tv_usec;
    return double(seconds * 1000 * 1000 + useconds) / 
        CLOCKS_PER_SEC;
}
struct timeval TimeMeasure::getBegintime()
{
    return begin_time;
}
struct timeval TimeMeasure::getEndtime()
{
    return end_time;
}
} // end of namespace benchmark

int main()
{
    // Connection time test
    ConnectionTime ct;
    ct.test();

    // Transfer time test
    TransFileTime tft;
    tft.test();

    // Network usage rate test
    NetworkUseRate nur;
    nur.test();

	return 0;
}
