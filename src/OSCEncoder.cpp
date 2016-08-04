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
#include "OSCEncoder.h"
#include "OSCArg.h"


int16_t OSCEncoder::encode( OSCMessage *_newMes ,uint8_t *_binData ){
	
	uint8_t *packStartPtr = _binData;
    	
//=========== OSC Address(String) -> BIN Encode   ===========

	memcpy( _binData , _newMes->_oscAddress , _newMes->_oscAdrSize );
	
	packStartPtr += _newMes->_oscAdrAlignmentSize;

	
//=========== TypeTag(String) -> BIN Encode   ===========
	
	*packStartPtr = ',';
    
    for ( uint8_t i=0 ; i<_newMes->_argsNum ; i++ ) packStartPtr[i+1] = _newMes->getArgTypeTag(i);

	packStartPtr += _newMes->_typeTagAlignmentSize;
	
	
//=========== Arguments -> BIN Encode   ==================
	

	for ( uint8_t i=0 ; i < _newMes->_argsNum ; i++ ) {
		switch ( _newMes->getArgTypeTag(i) ) {
				
			case kTagInt32:
            case kTagFloat:
            case kTagString:
            case kTagBlob:
            {                
                if(_newMes->_args[i]->_dataSize != 0)
                    memcpy( packStartPtr, _newMes->_args[i]->_argData, _newMes->getArgAlignmentSize(i) );
                else {
                    uint8_t *arg = (uint8_t *)calloc( 1, 4 );
                    memcpy( packStartPtr, arg, _newMes->getArgAlignmentSize(i));
                    free(arg);
                }
                
            }
                break;
   
            default:

                break;
	
		}
        packStartPtr += _newMes->getArgAlignmentSize(i);
	}
    
    
	return 1;
    
}

