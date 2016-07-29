#include <Gemini.h>

WiFiServer server(6341);

Gemini::Gemini(){
    input = 0;
    parser.setUserData(this);
    parser.addOscAddress("/ModuleManager/RequestML", Gemini::infoReqReceved);
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
    Serial.println("Gemini started!");
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
        umes.remoteIP = udp.remoteIP();
		udp.read((char *)packet, sizeof(long)*512);
		decoder.decode(&umes, packet);
		parser.patternComp(&umes);
	}
}

void Gemini::sendOutput(int v){
}

int Gemini::getInput(){
    input = 0;
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

}

void Gemini::infoReqReceved(OSCMessage *_mes, void *ud){
    Gemini *g = (Gemini *)ud;
    Serial.print("Received from ");
    Serial.println(_mes->remoteIP);
    OSCMessage response;
    byte destIP[]= {_mes->remoteIP[0],_mes->remoteIP[1],_mes->remoteIP[2],_mes->remoteIP[3]};
    response.setAddress(destIP, 6341);
    response.beginMessage("/ModuleList/setMList");
    response.addArgString("/SimpleIO");
    response.addArgString("/LED");
    response.addArgString("/Sensorvalue");
    response.addArgBlob((const char *)destIP, sizeof(destIP));
    uint8_t size = response.getMessageSize();
    uint8_t *sendData = ( uint8_t*)calloc(size, 1);
    g->encoder.encode(&response, sendData);
    
    WiFiClient client;
    client.connect(_mes->remoteIP, 6341);
    for(int i=0;i<size;i++){
        client.write(sendData[i]);
    }
    Serial.println(size);
}

void Gemini::initReqReceved(OSCMessage *_mes, void *ud){
    Gemini *g = (Gemini *)ud;
}

void Gemini::delReqReceved(OSCMessage *_mes, void *ud){
    Gemini *g = (Gemini *)ud;
}

void Gemini::dataReceived(OSCMessage *_mes, void *ud){
    Gemini *g = (Gemini *)ud;

}

