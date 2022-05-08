#include "ctoy/net.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>

namespace ctoy {

static void sockaddr_to_addr(const struct sockaddr_in *saddr, NetAddr *addr) {
  addr->ip.u32 = saddr->sin_addr.s_addr;
  addr->port = ntohs(saddr->sin_port);
}

static void addr_to_sockaddr(const NetAddr *addr, struct sockaddr_in *saddr) {
  saddr->sin_addr.s_addr = addr->ip.u32;
  saddr->sin_port = htons(addr->port);
  saddr->sin_family = AF_INET;
}

bool UdpSocket::init(NetAddr &self_addr) {
  struct sockaddr_in addr;
  addr_to_sockaddr(&self_addr, &addr);
  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s < 0) {
    return false;
  }

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    close(s);
    return false;
  }
  return true;
}

int UdpSocket::recvDatagram(void *buf, int len, NetAddr &from) {
  if (s < 0) {
    return -1;
  }

  struct sockaddr_in addr;
  socklen_t fromlen = sizeof(addr);
  int res = recvfrom(s, buf, len, 0, (struct sockaddr *)&addr, &fromlen);
  sockaddr_to_addr(&addr, &from);

  return res;
}
int UdpSocket::sendDatagram(const void *data, int len, const NetAddr &to) {
  if (s < 0) {
    return -1;
  }
  struct sockaddr_in addr;
  addr_to_sockaddr(&to, &addr);
  return sendto(s, data, len, 0, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
}
void UdpSocket::deinit() {
  close(s);
  s = -1;
}

void UdpSocket::enableBroadcast() {
  if (s < 0) {
    return;
  }
  int bOpt = 1;
  setsockopt(s, SOL_SOCKET, SO_BROADCAST, &bOpt, sizeof(bOpt));
}
} // namespace ctoy
