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
    
	if (client) {
        while(!client.available()){
            delay(1);
        }
        if(client.available() >0) {
            client.read(packet, sizeof(uint8_t)*512);
            decoder.decode(&tmes, packet);
            parser.patternComp(&tmes);
        }
        client.flush();
	}

	int packetSize = udp.parsePacket();
	if (packetSize){
        umes.remoteIP = udp.remoteIP();
		udp.read((uint8_t *)packet, sizeof(uint8_t)*512);
		decoder.decode(&umes, packet);
		parser.patternComp(&umes);
	}
}

int Gemini::addInput(char *inAddr, int inputPin){
    addr[patternNum] = inAddr;
    patternNum++;
    
    return patternNum;
}
int Gemini::addInput(char *inAddr, void (*inputCallback)(int)){
    //perserにinAddrでコールバック登録
    addr[patternNum] = inAddr;
    patternNum++;
    parser.addOscAddress((char *)inAddr, inputCallback);
    
    return patternNum;
}

int Gemini::addOutput(const char *outAddr) {
  return 0;
}
void Gemini::send(int outputID, int v){
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

//addCallback関数ではコーディネータからのリクエスト受信時，及び他モジュールからのデータ受信時に呼び出されるコールバック関数を登録
void Gemini::addCallback(char *_adr , Pattern::AdrFunc _func){
    parser.addOscAddress("/ModuleManager/RequestML", Gemini::infoReqReceved);
}

void Gemini::infoReqReceved(OSCMessage *_mes, void *ud){
    //send Module List
    Gemini *g = (Gemini *)ud;
    Serial.print("Received from ");
    Serial.println(_mes->remoteIP);
    
    //make a OSCMessage
    OSCMessage response;
    byte destIP[]= {_mes->remoteIP[0],_mes->remoteIP[1],_mes->remoteIP[2],_mes->remoteIP[3]};
    response.setAddress(destIP, 6341);
    response.beginMessage("/ModuleList/setMList");
    response.addArgString("/SimpleIO");
    response.addArgString("/LED");
    response.addArgString("/Sensorvalue");
    response.addArgBlob((const char *)destIP, sizeof(destIP));
    
    //make binary packet
    uint8_t size = response.getMessageSize();
    uint8_t *sendData = (uint8_t*)calloc(size, 1);
    g->encoder.encode(&response, sendData);
    
    //send packet
    WiFiClient client;
    if (!client.connect(_mes->remoteIP, 6341)) {
        Serial.println("connection failed");
        return;
    }
    int32_t size32 = (int32_t)size;
    int32_t sizenl = htonl(size32);
    delay(100);
    client.println("1");
    //client.write((uint8_t *)&size32, sizeof(int32_t));
    //int s = client.write((uint8_t *)sendData, sizeof(uint8_t)*size);
    //Serial.println(s);
}

void Gemini::initReqReceved(OSCMessage *_mes, void *ud){
    //モジュールの生成
    Gemini *g = (Gemini *)ud;
    WiFiClient client;
    //client.connect("192.168.1.1", "1234");
    uint8_t *sd = (uint8_t *)calloc(10, 1);
    client.write((uint8_t *)sd, sizeof(uint8_t)*10);
}

void Gemini::delReqReceved(OSCMessage *_mes, void *ud){
    //モジュールの削除
    Gemini *g = (Gemini *)ud;
}

void Gemini::dataReceived(OSCMessage *_mes, void *ud){
    //コールバック呼び出し
    Gemini *g = (Gemini *)ud;
}