#ifndef Gemini_h
#define Gemini_h
#define MAX_MODULE 4
#define MAX_IO 4
#define htonl(x)    ( ((x)<<24 & 0xFF000000UL) |	\
					  ((x)<< 8 & 0x00FF0000UL) |	\
					  ((x)>> 8 & 0x0000FF00UL) |	\
					  ((x)>>24 & 0x000000FFUL) )

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Arduino.h>
#include "OSCDecoder.h"
#include "OSCEncoder.h"
#include "OSCMessage.h"
#include "Pattern.h"
#include "OSCcommon.h"
#include "Module.h"

class Gemini {
 public:
  Gemini();
  ~Gemini();
  WiFiClient client;
  typedef void (*inputCallback)(int);
  void begin(char *gname, const char *ssid, const char *password);
  void monitor();
  int addInput(char *inAddr, void (*inputCallback)(int));
  int addOutput(const char *outAddr);
  void send(int outputID, int v);
  void sendMessageTCP(OSCMessage *m);
  void sendMessage(OSCMessage *m);
  void setupModule(char *addr, int id);
  void flushModule(int m);

 private:
  WiFiUDP udp;
  uint8_t packet[512];
  IPAddress coIP;
  char *geminame;
  char *inputAddr[MAX_IO];
  char *outputAddr[MAX_IO];
  inputCallback inputCb[MAX_IO];
  Module module[MAX_MODULE];
  uint8_t inputNum, outputNum;
    
  OSCDecoder decoder;
  OSCEncoder encoder;
  Pattern parser;
    bool localCheck(IPAddress ip);
  static void infoReqReceived(OSCMessage *_mes, void *ud);
  static void moduleReqReceived(OSCMessage *_mes, void *ud);
  static void addRoute(OSCMessage *_mes, void *ud);
  static void delRoute(OSCMessage *_mes, void *ud);
  static void dataReceived(OSCMessage *_mes, void *ud);
};
#endif
