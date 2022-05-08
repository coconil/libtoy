#pragma once

#include <stdint.h>
#include <stddef.h>
#include "fifo.hpp"

namespace ctoy {
template <typename Sync, typename Head, typename Size = unsigned int, Size max_data_len = 0> class Packet {
public:
  using T = uint8_t;
  using PacketFifo = Fifo<T, Size, max_data_len + sizeof(Head) + sizeof(Sync)>;
  enum class Result {
    MORE_DATA,
    READY,
    ERROR,
  };

  Packet() {
  }

  // private:
public:
  using Checksum = decltype(((Head *)0)->checksum);
  enum class ParseState {
    SYNC = 0,
    HEAD,
    DATA,
    CHECK,
    READY,
  };
  PacketFifo fifo;
  Size len;
  Checksum chk;
  ParseState stat;

public:
  Result resync() {
    Size count = 0;

    for (;;) {
      Head head;
      count = 0;
      stat = ParseState::SYNC;

      // search sync char
      for (;;) {
        if (fifo.getCount() < sizeof(Sync)) {
          goto _parse_abort;
        }

        Sync m;
        fifo.peek(reinterpret_cast<T *>(&m), sizeof(m));

        if (m.isCorrect())
          break;
        fifo.drop();
      }

      count = fifo.getCount();
      stat = ParseState::HEAD;
      if (count < sizeof(Sync) + sizeof(Head)) {
        goto _parse_abort;
      }

      fifo.peek(reinterpret_cast<T *>(&head), sizeof(head), sizeof(Sync));

      if ((head.len + sizeof(Head)) > fifo.getCapacity()) {
        fifo.drop();
        continue;
      }

      stat = ParseState::DATA;
      if (count < (sizeof(Head) + head.len)) {
        goto _parse_abort;
      }

      stat = ParseState::CHECK;

      Checksum chk_cacu;

      for (Size i = 0; i < head.len; ++i) {
        chk_cacu.caculate(fifo[sizeof(Head) + sizeof(Sync) + i]);
      }

      if (chk_cacu != head.checksum) {
        fifo.drop();
        continue;
      }

      // need parse
      stat = ParseState::READY;
      return Result::READY;
    }

  _parse_abort:
    return Result::MORE_DATA;
  }

  Result input(T &c) {
    Result result = Result::MORE_DATA;

    switch (stat) {
    case ParseState::SYNC:
      fifo.write(c);
      if (fifo.getCount() >= sizeof(Sync)) {
        Sync m;
        fifo.peek(reinterpret_cast<T *>(&m), sizeof(m));
        if (m.isCorrect()) {
          stat = ParseState::HEAD;
        } else {
          fifo.drop();
        }
      }
      break;
    case ParseState::HEAD:
      fifo.write(c);
      if (fifo.getCount() >= sizeof(Head) + sizeof(Sync)) {
        Head h;
        fifo.peek(reinterpret_cast<T *>(&h), sizeof(h), sizeof(Sync));
        len = h.len;
        chk = h.checksum;
        if (len > max_data_len) {
          result = Result::ERROR;
        } else if (len == 0) {
          stat = ParseState::CHECK;
        } else {
          stat = ParseState::DATA;
        }
      }
      break;
    case ParseState::DATA:
      fifo.write(c);
      if (fifo.getCount() >= sizeof(Head) + len) {
        stat = ParseState::CHECK;
      }
      break;
    case ParseState::CHECK:
      fifo.write(c);
      if (fifo.getCount() >= sizeof(Head) + len + sizeof(Checksum)) {
        Checksum chk_cacu;
        for (Size i = 0; i < len; ++i) {
          chk_cacu.caculate(fifo[sizeof(Head) + sizeof(Sync) + i]);
        }

        if (chk == chk_cacu) {
          stat = ParseState::READY;
          result = Result::READY;
        } else {
          stat = ParseState::SYNC;
          result = Result::ERROR;
        }
      }
      break;
    default:
      break;
    }
    return result;
  }

  Size read(Head &head, T *buf, Size size) {
    Size len;
    if ((stat != ParseState::READY) || fifo.peek(reinterpret_cast<T *>(&head), sizeof(head), sizeof(Sync)) != sizeof(head)) {
      return 0;
    }

    len = head.len;

    if (sizeof(head) + len > fifo.getCount()) {
      return 0;
    }

    // skip header
    fifo.drop(sizeof(Head) + sizeof(Sync));

    if (size < len) {
      fifo.read(buf, size);
    } else {
      fifo.read(buf, len);
    }

    if (!fifo.isEmpty()) {
      fifo.clean();
    }

    stat = ParseState::SYNC;
    return len;
  }

  static Size writeTo(PacketFifo &fifo, Head &head, const T *buf, Size len) {
    Checksum checksum;
    Size space = fifo.getSpace();

    int size = len + sizeof(head);

    if (size > space) {
      return 0;
    }

    for (Size i = 0; i < head.len; ++i) {
      head.checksum.caculate(buf[i]);
    }

    fifo.write(&Sync::value, sizeof(Sync));
    fifo.write(&head, sizeof(head));
    fifo.write(buf, len);

    return len + sizeof(Head) + sizeof(Sync);
  }
};
} // namespace ctoy
