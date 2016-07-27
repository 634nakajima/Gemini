#include <Gemini.h>

WiFiServer server(6341);

Gemini::Gemini(){
    input = 0;
}

Gemini::~Gemini(){
}

void Gemini::begin(const char *ssid, const char *password){
	WiFi.mode(WIFI_STA);
  	WiFi.begin(ssid, password);
    Serial.begin(115200);
  	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    	Serial.println("Connection Failed! Rebooting...");
    	delay(5000);
    	ESP.restart();
  	}
	udp.begin(6340);
	server.begin();
}

void Gemini::monitor(){
	WiFiClient client = server.available();
    OSCMessage mes;
    uint8_t packet[512];
    
	if (client) {
		while(!client.available()){
			delay(1);
		}
		//client.read((char *)packet, sizeof(long)*100);
		decoder.decode(&mes, packet);
		parser.patternComp(&mes);
  		client.flush();
	}

	int packetSize = udp.parsePacket();
	if (packetSize){
		//udp.read((char *)packet, sizeof(long)*100);
		decoder.decode(&mes, packet);
		parser.patternComp(&mes);
	}
}

void Gemini::sendOutput(int v){
}

int Gemini::getInput(){
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

