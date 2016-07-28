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
    OSCMessage tmes, umes;
    uint8_t packet[512];
    
	if (client) {
		while(!client.available()){
			delay(1);
		}
        int size = client.available();
        if(size >0) {
            for(int i=0; i<size ;i++) {
                packet[i]  = client.read();
            }
        }
		decoder.decode(&tmes, packet);
		parser.patternComp(&tmes);
  		client.flush();
	}

	int packetSize = udp.parsePacket();
	if (packetSize){
		udp.read((char *)packet, sizeof(long)*512);
		decoder.decode(&umes, packet);
		parser.patternComp(&umes);
	}
}

void Gemini::sendOutput(int v){
}

int Gemini::getInput(){
    input =
    return input;
}

void Gemini::sendInfo(){
}

void Gemini::sendInitTokenReq(){
}

void Gemini::sendDelTokenReq(){
}

void Gemini::addCallback(char *_adr , Pattern::AdrFunc _func){
    parser.addOscAddress("/ModuleManager/RequestML", Gemini::infoReqReceved);
    parser.addOscAddress(_adr, _func);
    parser.addOscAddress(_adr, _func);

}

void Gemini::infoReqReceved(OSCMessage *_mes){
}

void Gemini::initReqReceved(OSCMessage *_mes){
}

void Gemini::delReqReceved(OSCMessage *_mes){
}

void Gemini::dataReceived(OSCMessage *_mes){
}

