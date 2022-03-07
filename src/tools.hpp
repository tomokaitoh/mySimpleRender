#pragma once
#include<ctime>
#define for_i_in_range(start,end) for(int i=(start);i<(end);++i)
#define for_j_in_range(start,end) for(int j=(start);j<(end);++j)
#define tlog(message) std::cout<< message <<std::endl
#define tlogv(var) std::cout<< #var <<": "<<var<<std::endl
#define tlogf(format,...) printf(format,__VA_ARGS__)

class Timer{
public:
    static time_t startTime;
    static void set(){startTime=0;};
};