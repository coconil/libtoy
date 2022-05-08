#pragma once
#include <stdint.h>
#include <time.h>
#include <stdbool.h>

namespace ctoy {

class File {
private:
  long long priv;
public:
  enum class Type {
    DIR,
    REG,
  };
  File();
  ~File();
  enum class Mode:uint8_t { READ = 1, WRITE = 2, CREATE = 4, TRUNC = 8 };
  enum class Seek { BEG, CUR, END };

  bool open(const char *path, Mode mode);
  int write(const void *buf, int len);
  int read(void *buf, int len);
  int seek(int offset, Seek fromwhere);
  int tell();
  int flush();
  void close();
  int eof();

  static bool exists(const char *path);
  static bool isdir(const char *path);
  static int64_t mtime(const char *path);
  static int64_t fsize(const char *path);
};
} // namespace ctoy
