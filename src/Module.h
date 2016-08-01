#ifndef Module_h
#define Module_h
#define MAX_IO 4

#include <Arduino.h>

class Module {
public:
  Module(){
    live = false;
    id = -1;
  }
  ~Module(){};

  bool live;
  int id;
  char mAddr[64];
  char addRoute[64];
  char delRoute[64];
  char inputAddr[MAX_IO][64];
  char outputAddr[MAX_IO][MAX_IO][64];
  uint8_t outputIP[MAX_IO][MAX_IO][4];
  int outAddrNum[MAX_IO];
  void setup(char *addr, int i);
  void setInputAddr(char **inAddr, int inNum);
  void addOutputAddr(char *ip, char *osc, int outID);
  void delOutputAddr(char *osc, int outID);
  void flush();
};
#endif
