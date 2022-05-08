#pragma once

#include <stdint.h>
#include <string.h>

namespace ctoy {
template <typename T, typename Size = unsigned int, Size size = 0> class Fifo {
  // private:
public:
  T data[size];
  Size wrp;
  Size rdp;

  constexpr Size offset_of_pos(Size pos) {
    return (pos) % size;
  }

  constexpr T *addr_of_pos(Size pos) {
    return data + offset_of_pos(pos);
  }

  constexpr T *write_addr(Size pos) {
    return addr_of_pos(wrp);
  }

  constexpr T *read_addr(Size pos) {
    return addr_of_pos(rdp);
  }

public:
  Fifo() : wrp(0), rdp(0) {
  }

  void clean() {
    rdp = wrp;
  }

  Size getCount() {
    return wrp - rdp;
  }

  Size getSpace() {
    return size - getCount();
  }

  constexpr Size getCapacity() {
    return size;
  }

  bool isEmpty() {
    return wrp == rdp;
  }

  bool isFull() {
    return wrp - rdp >= size;
  }

  Size write(T &t) {
    return write(&t, 1);
  }

  Size write(T *t, Size count) {
    Size space = getSpace();
    count = count < space ? count : space;
    if (count) {
      Size offset = offset_of_pos(wrp);
      if ((offset + count) <= size) {
        for (Size i = 0; i < count; ++i) {
          data[offset + i] = t[i];
        }
      } else {
        Size left = size - offset;
        for (Size i = 0; i < left; ++i) {
          data[offset + i] = t[i];
        }
        for (Size i = 0; i < count - left; ++i) {
          data[i] = t[left + i];
        }
      }
      wrp += count;
    }
    return count;
  }

  const T &operator[](Size pos) {
    Size offset = offset_of_pos(rdp + pos);
    return data[offset];
  }

  Size peek(T &t, Size pos = 0) {
    if (getCount() <= pos) {
      return false;
    }
    Size offset = offset_of_pos(rdp + pos);
    t = data[pos];
    return true;
  }

  Size peek(T *t, Size n, Size pos = 0) {
    Size count = getCount();
    if (count < pos) {
      return 0;
    }
    if (count < n + pos) {
      n = count - pos;
    }

    if (n) {
      uint32_t offset = offset_of_pos(rdp + pos);
      if ((offset + n) <= size) {
        for (Size i = 0; i < n; ++i) {
          t[i] = data[offset + i];
        }
      } else {
        uint32_t left = size - offset;
        for (Size i = 0; i < left; ++i) {
          t[i] = data[offset + i];
        }
        for (Size i = 0; i < n - left; ++i) {
          t[left + i] = data[i];
        }
      }
    }
    return n;
  }

  Size read(T &t) {
    return read(&t, 1);
  }

  Size read(T *t, Size count) {
    Size n = peek(t, count);
    rdp += n;
    return n;
  }

  Size drop(Size n = 1) {
    Size count = getCount();
    if (count < n) {
      n = count;
    }
    rdp += n;
    return n;
  }
};
} // namespace ctoy
