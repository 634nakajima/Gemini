#ifndef Gemini_h
#define Gemini_h

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "OSCDecoder.h"
#include "OSCEncoder.h"
#include "OSCMessage.h"
#include "Pattern.h"
#include "OSCCommon.h"

//tokenのosc	アドレスは，
//（コーディネータIP）/（Geminiネーム）/（番号）

class Gemini {
public:
	Gemini();
	~Gemini();
	
	void begin(const char *ssid, const char *password);
	void monitor();
	void sendOutput(int v);
	int getInput();
	void sendInfo();
	void sendInitTokenReq();
	void sendDelTokenReq();

private:
	WiFiUDP udp;
	WiFiClient client;

	OSCDecoder decoder;
	OSCEncoder encoder;
	Pattern parser;
	int input, output;
	void addFunc();
	void infoReqReceved(OSCMessage *_mes);
	void initReqReceved(OSCMessage *_mes);
	void delReqReceved(OSCMessage *_mes);
	void dataReceived(OSCMessage *_mes);
};
#endif
