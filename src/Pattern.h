/*
 
 ArdOSC 2.1 - OSC Library for Arduino.
 
 -------- licence -----------------------------------------------------------
 
 ArdOSC
 
 The MIT License
 
 Copyright (c) 2009 - 2011 recotana( http://recotana.com )　All right reserved
 
 */

#ifndef ArdOSC_patternmatch_h
#define ArdOSC_patternmatch_h

#define kMaxPatternMatch    50

#include "OSCMessage.h"

class Pattern{
	
private:
    
    typedef void (*AdrFunc)(OSCMessage*, void *);
    typedef void (*inputCallback)(int);
    
    AdrFunc adrFunc[kMaxPatternMatch];
    inputCallback inputCb[kMaxPatternMatch];
    char *addr[kMaxPatternMatch];

    uint8_t patternNum;
    void *user_data;
public:
    
    Pattern();
    ~Pattern();
    
    void setUserData(void *ud);
	void addOscAddress(char *_adr , AdrFunc _func);
    void addOscAddress(char *_adr , inputCallback _func );
    void execFunc(uint8_t _index,OSCMessage *_mes);
    void execFunc(uint8_t _index, int v);
    void patternComp(OSCMessage *_mes);
   
    friend class Gemini;
};


#endif
