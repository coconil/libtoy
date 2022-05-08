#include "ctoy/fs.hpp"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdio.h>

namespace ctoy {

File::File() : priv(-1) {
}

bool File::open(const char *path, Mode mode) {
  // todo:convert mode

  int mode_ = 0;
  int fd = ::open(path, mode_);
  if (fd < 0) {
    return false;
  }
  return true;
}

int File::write(const void *buf, int len) {
  return ::write(priv, buf, len);
}
int File::read(void *buf, int len) {
  return ::read(priv, buf, len);
};

int File::seek(int offset, Seek origin) {
  int origin_;
  if (origin == Seek::CUR) {
    origin_ = SEEK_CUR;
  } else if (origin == Seek::BEG) {
    origin_ = SEEK_SET;
  } else if (origin == Seek::END) {
    origin_ = SEEK_END;
  } else {
    return -1;
  }
  return lseek(priv, offset, origin_);
}

void File::close() {
  ::close(priv);
}

File::~File(){
  if(priv>0){
    ::close(priv);
  }
}

} // namespace ctoy
