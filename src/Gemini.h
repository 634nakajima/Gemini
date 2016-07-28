#ifndef Gemini_h
#define Gemini_h

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
	int addInput(const char *inAddr, int inputPin);
	int addInput(const char *inAddr, void (*inputCallback)(int));
	int addOutput(const char *outAddr);
	void send(int outputID, int v);
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
	void addCallback(char *_adr, Pattern::AdrFunc _func );
	static void infoReqReceved(OSCMessage *_mes);
	static void initReqReceved(OSCMessage *_mes);
	static void delReqReceved(OSCMessage *_mes);
	static void dataReceived(OSCMessage *_mes);
};
#endif
