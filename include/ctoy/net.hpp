#pragma once
#include <stdint.h>

namespace ctoy{
  int net_init(void);
  struct NetAddr{
    union IP{
      uint32_t u32;
      uint8_t u8[4];
      IP(uint32_t v):u32(v){}
      IP(uint8_t u8[4]){this->u8[0]=u8[0];this->u8[1]=u8[1];this->u8[2]=u8[2];this->u8[3]=u8[3];}
    };

     IP ip;
     uint16_t port;
     NetAddr(uint8_t ip[4],uint16_t port):ip(ip),port(port){};
     NetAddr(uint16_t port):ip((uint32_t)0),port(port){}
     NetAddr():NetAddr(0){}
  };

  class UdpSocket{
    private:
    int s;
    public:
     bool init(NetAddr&self);
     int recvDatagram(void* buf,int len,NetAddr &from);
     int 	sendDatagram(const void* data,int len, const NetAddr &to);
     void deinit();
     void enableBroadcast();
  };
}
