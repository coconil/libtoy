#include "fifo.hpp"
#include "os.hpp"

namespace ctoy {

template <typename T, unsigned size> class MsgPool {
private:
  Fifo<T, unsigned, size> fifo;
  Mutex mtx;

public:
  bool put(const T &t) {
    LockGard lg(mtx);
    if (fifo.write(t) < 1) {
      return false;
    } else {
      return true;
    }
  }

  bool get(T &t) {
    LockGard lg(mtx);
    if (fifo.read(t) < 1) {
      return false;
    } else {
      return true;
    }
  }
};

template <typename Pool> struct MsgLink { Pool *next; };

template <typename T> class MsgPublisher {
private:
  Mutex mtx;

public:
  void subscribe();
  void unsubscribe();
};
} // namespace ctoy
