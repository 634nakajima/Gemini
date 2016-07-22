#include "Gemini.h"

Gemini::Gemini(){
}

Gemini::~Gemini(){
}

void Gemini::begin(){
	/*WiFi.mode(WIFI_STA);
  	WiFi.begin(ssid, password);
  	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    	Serial.println("Connection Failed! Rebooting...");
    	delay(5000);
    	ESP.restart();
  	}*/
	udp.begin(6340);
	server.begin();
}

void Gemini::monitor(){
	WiFiClient client = server.available();
	if (client) {
		while(!client.available()){
			delay(1);
		}
		OSCMessage mes;
		char packet[512];
		client.read((char *)packet, sizeof(long)*100);
		decoder.decode(&mes, packet);
		parser.patternComp(&mes);
  		client.flush();
	}

	packetSize = udp.parsePacket();
	if (packetSize){
		udp.read((char *)packet, sizeof(long)*100);
		decoder.decode(&mes, packet);
		parser.patternComp(&mes);
	}
}

void Gemini::output(int_16 v){
}

int_16 Gemini::input(){
}

void Gemini::sendInfo(){
}

void Gemini::sendInitTokenReq(){
}

void Gemini::sendDelTokenReq(){
}

void Gemini::addFunc(){
}

void Gemini::infoReqReceved(OSCMessage *_mes){
}

void Gemini::initReqReceved(OSCMessage *_mes){
}

void Gemini::delReqReceved(OSCMessage *_mes){
}

void Gemini::dataReceived(OSCMessage *_mes){
}

