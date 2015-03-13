/***********************************************************
 > File Name: Benchmark.cpp
 > Author: Weidong, ZHANG
 > Mail: wdfnst@pku.edu.cn
 > Created Time: Sat 07 Mar 2015 07:07:58 PM PST
 **********************************************************/
#include "Benchmark.h"

namespace benchmark {

Benchmark::Benchmark()
{ }

///////////Defination of ConnectionTime//////////
ConnectionTime::ConnectionTime()
{

}
void ConnectionTime::test()
{
    
}
///////////Defination of TransFileTime///////////
///////////Defination of NetworkUseRate//////////
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
