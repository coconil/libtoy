#pragma once
#include <fstream>
#include <stdio.h>

#if 0 //NDEBUG
#define DEBUG_LOG(format,...) printf(format"(%s:%d)\n",##__VA_ARGS__,__FILE__,__LINE__)
#else 
#define DEBUG_LOG(format,...)
#endif 

namespace ctoy{
  class Logger{
    public:
    Logger(const char*path){}
  };
}
