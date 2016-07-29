#ifndef Gemini_h
#define Gemini_h

#define htonl(x)    ( ((x)<<24 & 0xFF000000UL) | \
                    ((x)<< 8 & 0x00FF0000UL) | \
                    ((x)>> 8 & 0x0000FF00UL) | \
                    ((x)>>24 & 0x000000FFUL) )

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include "OSCDecoder.h"
#include "OSCEncoder.h"
#include "OSCMessage.h"
#include "Pattern.h"
#include "OSCcommon.h"
//tokenのosc	アドレスは，
//（コーディネータIP）/（Geminiネーム）/（番号）

class Gemini {
public:
	Gemini();
	~Gemini();
	
	void begin(const char *ssid, const char *password);
	void monitor();
	int addInput(char *inAddr, int inputPin);
	int addInput(char *inAddr, void (*inputCallback)(int));
	int addOutput(const char *outAddr);
	void send(int outputID, int v);
	int getInput();
	void sendInfo();
	void sendInitTokenReq();
	void sendDelTokenReq();

private:
	WiFiUDP udp;
	WiFiClient client;
    uint8_t packet[512];
    
    char *addr[kMaxPatternMatch];
    uint8_t patternNum;
    
	OSCDecoder decoder;
	OSCEncoder encoder;
	Pattern parser;
	int input, output;
	void addCallback(char *_adr, Pattern::AdrFunc _func );
	static void infoReqReceved(OSCMessage *_mes, void *ud);
	static void initReqReceved(OSCMessage *_mes, void *ud);
	static void delReqReceved(OSCMessage *_mes, void *ud);
	static void dataReceived(OSCMessage *_mes, void *ud);
};
#endif
