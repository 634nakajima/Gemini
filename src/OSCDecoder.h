#ifndef OSCDecoder_h
#define OSCDecoder_h

#include "OSCMessage.h"

class OSCDecoder{
private:
	int16_t decode( OSCMessage *_mes ,const uint8_t *_binData );
    
public:
    friend class Gemini;
};

#endif
