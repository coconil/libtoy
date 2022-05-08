#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

namespace ctoy {
class Thread {
private:
  uintmax_t impl;
public:
  enum class Priority {
    REALTIME,
    HIGH,
    NORMAL,
    LOW,
  };
  struct Attr {
    const char*name;
    void *stack_area;
    size_t stack_size;
    Priority priority;
  };
  int start(Attr &attr);
  int join(void **retval);
  virtual int run()=0;
};

class Mutex {
private:
  uintmax_t impl;

public:
  Mutex();
  void lock();
  void unlock();
  bool tryLock();
  bool timedLock(unsigned ms);
};

  class LockGard{
  private:
    Mutex &mtx;
  public:
    LockGard(Mutex &mtx):mtx(mtx) {
      mtx.lock();
    }
    ~LockGard() {
      mtx.unlock();
    }
  };

class Sem {
private:
  uintmax_t impl;
public:
  Sem(unsigned count, unsigned init);
  void post();
  void wait();
  bool tryWait();
  bool timedWait(unsigned ms);
};

class Timer {
private:
  uintmax_t impl;
  using Routine = int (*)(void *);

public:
  void start();
};

class Time{
private:
  uint32_t value;
  static uint32_t milli_uptime();
public:
  Time() : value(0) {
  }
  Time(const uint32_t v) : value(v) {
  }
  Time(const Time&t){
    value = t.value;
  }
  Time(const int v):value((const uint32_t)v){}
  void update();
  Time operator+(const uint32_t v){
     return value+v;
  }
  bool operator>(const Time&t) const{
    return this->value>t.value;
  }
  bool operator==(const Time&t) const{
    return this->value==t.value;
  }

  uint32_t getValue() const{
     return value;
  }
  void setValue(const uint32_t v){
    value=v;
  }

  void operator=(uint32_t v){
    this->value=v;
  }

  void operator=(const Time&&t){
    this->value=t.value;
  }
  static Time now(){
    return Time(milli_uptime());
  }
  static void sleep(Time ms);
};
} // namespace ctoy
