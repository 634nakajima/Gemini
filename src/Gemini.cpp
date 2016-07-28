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

int Gemini::addInput(const char *inAddr, int inputPin){
  return parser.patternNum;
}
int Gemini::addInput(const char *inAddr, void (*inputCallback)(int)){
  //perserにinAddrでコールバック登録
  parser.addOscAddress((char *)inAddr, inputCallback);
  return parser.patternNum;
}

int Gemini::addOutput(const char *outAddr) {
  return 0;
}
void Gemini::send(int outputID, int v){
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

//addCallback関数ではコーディネータからのリクエスト受信時，及び他モジュールからのデータ受信時に呼び出されるコールバック関数を登録
void Gemini::addCallback(char *_adr , Pattern::AdrFunc _func){
    parser.addOscAddress("/ModuleManager/RequestML", Gemini::infoReqReceved);
}


void Gemini::infoReqReceved(OSCMessage *_mes){
  //モジュールリストの送信
  sendInfo();
}

void Gemini::initReqReceved(OSCMessage *_mes){
  //モジュールの生成
}

void Gemini::delReqReceved(OSCMessage *_mes){
  //モジュールの削除
}

void Gemini::dataReceived(OSCMessage *_mes){
  //コールバック呼び出し
}

