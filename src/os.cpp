#include "ctoy/os.hpp"
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>


namespace ctoy {
static void *thread_routine(void *argument) {
  auto thread = reinterpret_cast<Thread *>(argument);
  thread->run();
  return nullptr;
}

int Thread::start(Attr &attr) {
  pthread_attr_t pAttr;
  //sched_param sparam;

  pthread_attr_init(&pAttr);

  if (attr.stack_size) {
    pthread_attr_setstacksize(&pAttr, attr.stack_size);
  }

  // if (attr.stack_area) {
  //  pthread_attr_setstackaddr(&pAttr,attr.stack_area);
  //}

  // pthread_attr_getschedparam(&pAttr,&sparam);
  pthread_t tid;
  pthread_create(&tid, &pAttr, thread_routine, this);
  impl = tid;

  return 0;
}
int Thread::join(void **retval) {
  return 0;
}

Sem::Sem(unsigned count, unsigned init) {
  sem_t *s=new sem_t;
  sem_init(s, 0, init);
  impl = (uintptr_t)s;
}

void Sem::post() {
  auto s=(sem_t*)impl;
  sem_post(s);
}

void Sem::wait() {
  auto s=(sem_t*)impl;
  sem_wait(s);
}

bool Sem::tryWait() {
  auto s=(sem_t*)impl;
  return sem_trywait(s)==0;
}

bool Sem::timedWait(unsigned ms) {
  auto s=(sem_t*)impl;
  timespec t;
  t.tv_sec= ms/1000;
  t.tv_nsec= (ms%1000)*1000000;
  return sem_timedwait(s,&t)==0;
}

Mutex::Mutex() {
  pthread_mutex_t *mtx=new pthread_mutex_t;
  pthread_mutex_init(mtx,nullptr);
  impl=(intptr_t)mtx;
}

void Mutex::lock() {
  auto mtx=(pthread_mutex_t *)impl;
  pthread_mutex_lock(mtx);
}
void Mutex::unlock() {
  auto mtx=(pthread_mutex_t *)impl;
  pthread_mutex_unlock(mtx);
}

bool Mutex::tryLock() {
  auto mtx=(pthread_mutex_t *)impl;
  return 0 == pthread_mutex_trylock(mtx);
}
bool Mutex::timedLock(unsigned ms) {
  pthread_mutex_t s;
  timespec t;
  s.__align=impl;
  t.tv_sec= ms/1000;
  t.tv_nsec= (ms%1000)*1000000;
  return pthread_mutex_timedlock(&s,&t)==0;
}

uint32_t Time::milli_uptime() {
  timespec t;
  clock_gettime(CLOCK_MONOTONIC,&t);
  return t.tv_sec*1000+t.tv_nsec/1000000;
}

void Time::sleep(Time ms) {
  usleep(ms.value*1000);
}

void Time::update() {
  value = milli_uptime();
}

} // namespace ctoy

extern "C" {
  int bsp_initial_board();
}
int initialize();
int main() {
  bsp_initial_board();
  return initialize();
}