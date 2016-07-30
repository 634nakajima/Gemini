/*
 
 ArdOSC 2.1 - OSC Library for Arduino.
 
 -------- licence -----------------------------------------------------------
 
 ArdOSC
 
 The MIT License
 
 Copyright (c) 2009 - 2011 recotana( http://recotana.com )　All right reserved
 
 */

#include "OSCArg.h"
#include "OSCMessage.h"
#include <stdlib.h>
#include <string.h>

#define htonl(x)    ( ((x)<<24 & 0xFF000000UL) | \
((x)<< 8 & 0x00FF0000UL) | \
((x)>> 8 & 0x0000FF00UL) | \
((x)>>24 & 0x000000FFUL) )

OSCArg::OSCArg(void){
    _typeTag = 0;
    _argData = 0;
    _dataSize = 0;
    _alignmentSize = 0;
}

OSCArg::OSCArg(char _tag){
    _typeTag = _tag;
    _argData = 0;
    _dataSize = 0;
    _alignmentSize = 0;
}


OSCArg::OSCArg( char _tag , void *_data , uint16_t _size , bool _packSizeCulc ) {
    
    _typeTag = _tag;
    _dataSize = _size;
    
    if( _packSizeCulc )   _alignmentSize = CULC_ALIGNMENT(_size);
    else                _alignmentSize = _size;
    
    if( _size == 0 ) return;

    if(_typeTag != kTagBlob) {
        _argData = calloc( 1, _alignmentSize );
        memcpy( (uint8_t*)_argData , _data , _size );
    }
    else {
        _alignmentSize = _size + sizeof(uint32_t);
        _argData = calloc( 1, _alignmentSize);
        
        uint32_t size32 = (uint32_t)_size;
        uint32_t sizenl = htonl(size32);
        memcpy((uint8_t*)_argData, (uint32_t *)&sizenl, sizeof(uint32_t));
        memcpy((uint8_t*)_argData+sizeof(uint32_t) , _data , _size );
    }
}

OSCArg::~OSCArg(void){
    
    if( _typeTag != 0 ) flush();

}

void OSCArg::flush(void){
    
    if( _dataSize > 0 ) free(_argData);

    _dataSize = 0;
    _alignmentSize = 0;
    _typeTag = 0;
}



