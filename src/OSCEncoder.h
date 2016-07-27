/*
 
 ArdOSC 2.1 - OSC Library for Arduino.
 
 -------- licence -----------------------------------------------------------
 
 ArdOSC
 
 The MIT License
 
 Copyright (c) 2009 - 2011 recotana( http://recotana.com )　All right reserved
 
 */


#ifndef OSCEncoder_h
#define OSCEncoder_h



#include "OSCMessage.h"

class OSCEncoder{
	
private:
	
    int16_t encode( OSCMessage *mes ,uint8_t *_binData );
	
    
public:
	

    friend class Gemini;
};

#endif
