#include "Module.h"

void Module::setup(char *addr, int i){
  live = true;
  id = i;
  strcpy(mAddr, addr);
  strcpy(addRoute, addr);
  strcpy(delRoute, addr);
  strcat(addRoute, "/AddRoute");
  strcat(delRoute, "/DeleteRoute");
  
  for(int i=0;i<MAX_IO;i++){
    outAddrNum[i] = 0;
  }
}

void Module::setInputAddr(char **inAddr, int inNum){
  for(int i=0;i<inNum;i++){
    strcpy(inputAddr[i], mAddr);
    strcat(inputAddr[i], inAddr[i]);
  }
}

void Module::addOutputAddr(char *ip, char *osc, int outID){
  char *tmp[4], tmpip[32];
  strcpy(tmpip, ip);
  for(int i=0;i<4;i++){
    tmp[i] = strtok(tmpip, ".");
    outputIP[outID][outAddrNum[outID]][i] = (uint8_t)int(tmp[i]);
  }
  strcpy(outputAddr[outID][outAddrNum[outID]], osc);
  outAddrNum[outID]++;
}

void Module::delOutputAddr(char *ip, char *osc, int outID){
  int i;
  for(i=0;i<MAX_IO;i++){
    if(strcmp(outputAddr[outID][i], osc) == 0){
      memset(outputIP[outID][i], '\0', 4);
      memset(outputAddr[outID][i], '\0', 64);
    }
  }
  for(int j=i;j<MAX_IO-1;j++){
    memcpy(outputIP[outID][j], outputIP[outID][j], 4);
    strcpy(outputAddr[outID][j], outputAddr[outID][j+1]);
  }
  memset(outputAddr[outID][MAX_IO], '\0', 64);
  memset(outputIP[outID][MAX_IO], '\0', 4);
  outAddrNum[outID]--;
}

void Module::flush() {
  live = false;
  id = -1;
  memset(addRoute,'\0', 64);
  memset(delRoute,'\0', 64);
  for(int i=0;i<MAX_IO;i++){
    memset(inputAddr[i],'\0', 64);
    outAddrNum[i] = 0;
    for(int j=0;j<MAX_IO;j++){
      memset(outputAddr[i][j],'\0', 64);
      memset(outputIP[i][j],'\0', 4);
    }
  }
}
