#include "Module.h"
#include "misc.h"

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
  tmp[0] = strtok(tmpip, ".");
  outputIP[outID][outAddrNum[outID]][0] = (uint8_t)atoi(tmp[0]);
  for(int i=1;i<4;i++){
	tmp[i] = strtok(NULL, ".");
    outputIP[outID][outAddrNum[outID]][i] = (uint8_t)atoi(tmp[i]);
  }
  strcpy(outputAddr[outID][outAddrNum[outID]], osc);

  outAddrNum[outID]++;
}

void Module::delOutputAddr(char *osc, int outID){
  int i;
  for(i=0;i<MAX_IO;i++){
    if(strcmp(outputAddr[outID][i], osc) == 0){
	  outputIP[outID][i] = {0,0,0,0};
      memset(outputAddr[outID][i], '\0', 64);
	  break;
    }
  }
  for(int j=i;j<MAX_IO-1;j++){
    outputIP[outID][j] = outputIP[outID][j+1];
    strcpy(outputAddr[outID][j], outputAddr[outID][j+1]);
  }
  memset(outputAddr[outID][MAX_IO-1], '\0', 64);
  outputIP[outID][MAX_IO-1] = {0,0,0,0};
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
      outputIP[i][j] = {0,0,0,0};
    }
  }
}

