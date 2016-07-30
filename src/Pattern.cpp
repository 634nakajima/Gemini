/*
 
 ArdOSC 2.1 - OSC Library for Arduino.
 
 -------- licence -----------------------------------------------------------
 
 ArdOSC
 
 The MIT License
 
 Copyright (c) 2009 - 2011 recotana( http://recotana.com )　All right reserved
 
 */

#include <stdlib.h>
#include <string.h>
#include "OSCcommon.h"
#include "OSCMessage.h"

#include "Pattern.h"



Pattern::Pattern(){
    patternNum=0;

}
Pattern::~Pattern(){    
}

void Pattern::addOscAddress(char *_adr, AdrFunc _func){
    adrFunc[patternNum] = _func;
    addr[patternNum] = _adr;
    patternNum++;
}

void Pattern::delOscAddress(char *adr){
  uint8_t i;
  for (i=0 ; i<kMaxPatternMatch ; i++) {
    if ( strcmp(addr[i] , adr ) == 0 ) {
      adrFunc[i] = NULL;
      break;
    }
  }
  for(uint8_t j=i; j<kMaxPatternMatch-1;j++){
    adrFunc[j] = adrFunc[j+1];
  }
  patternNum--;
}

void Pattern::execFunc(uint8_t _index, OSCMessage *_mes){
        adrFunc[_index](_mes, user_data);
}

void Pattern::patternComp(OSCMessage *_mes){    
    for (uint8_t i=0 ; i<kMaxPatternMatch ; i++) {
        if ( strcmp( addr[i] , _mes->_oscAddress ) == 0 && user_data != NULL) execFunc( i , _mes);
    }
}

void Pattern::setUserData(void *ud){
    user_data = ud;
}

