#include <Gemini.h>

WiFiServer server(6341);

Gemini::Gemini(){
    input = 0;
    inputNum = 0;
    outputNum = 0;
    fullInNum = 0;
    parser.setUserData(this);
    parser.addOscAddress("/ModuleManager/RequestML", Gemini::infoReqReceived);
}

Gemini::~Gemini(){
}

void Gemini::begin(char *gname, const char *ssid, const char *password){
  geminame = gname;
  parser.addOscAddress(geminame, Gemini::moduleReqReceived);
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
    inputAddr[inputNum] = inAddr;
    inputNum++;
    
    return inputNum;
}
int Gemini::addInput(char *inAddr, void (*inputCallback)(int)){
    //perserにinAddrでコールバック登録
    inputAddr[inputNum] = inAddr;
    inputCb[inputNum] = inputCallback;
    inputNum++;
    
    return inputNum;
}

int Gemini::addOutput(const char *outAddr) {
  outputAddr[outputNum] = (char *)outAddr;
  outputNum++;
  
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
    parser.addOscAddress("/ModuleManager/RequestML", Gemini::infoReqReceived);
}

void Gemini::infoReqReceived(OSCMessage *_mes, void *ud){
    //send Module List
    Gemini *g = (Gemini *)ud;
    Serial.print("Received from ");
    Serial.println(_mes->remoteIP);
    
    //make a OSCMessage
    OSCMessage response;
    byte destIP[]= {_mes->remoteIP[0],_mes->remoteIP[1],_mes->remoteIP[2],_mes->remoteIP[3]};
    response.setAddress(destIP, 6341);
    response.beginMessage("/ModuleList/setMList");
    response.addArgString(g->geminame);

    char inputAddrs[256], outputAddrs[256];
    for(int i=0;i<g->inputNum;i++){
      if(strlen(inputAddrs)+strlen(g->inputAddr[i])<128)
	strcat(inputAddrs, g->inputAddr[i]);
    }
    for(int i=0;i<g->outputNum;i++){
      if(strlen(outputAddrs)+strlen(g->outputAddr[i])<128)
	strcat(outputAddrs, g->outputAddr[i]);
    }
    response.addArgString(inputAddrs);
    response.addArgString(outputAddrs);
    response.addArgBlob((const char *)destIP, 4);
    
    //make binary packet
    uint8_t size = response.getMessageSize(); 
    uint8_t *sendData = (uint8_t*)calloc(size, 1);
    g->encoder.encode(&response, sendData);
    
    //send packet
    WiFiClient client;
    if (!client.connect(destIP, 6341)) {
        Serial.println("connection failed");
        return;
    }
    int32_t size32 = (int32_t)size;
    int32_t sizenl = htonl(size32);

    client.write((uint8_t *)&sizenl, sizeof(int32_t));
    client.write((uint8_t *)sendData, sizeof(uint8_t)*size);
}

void Gemini::moduleReqReceived(OSCMessage *_mes, void *ud){
    //モジュールの生成
    Gemini *g = (Gemini *)ud;
    char p[128];
    for(int i=0;i<4;i++)
      p[i] = _mes->remoteIP[3];

    byte destIP[]= {_mes->remoteIP[0],
		    _mes->remoteIP[1],
		    _mes->remoteIP[2],
		    _mes->remoteIP[3]};
    p[4] = '\0';
    strcat(p, g->geminame);
    strcat(p, "/Node");
    char arg[5];
    _mes->getArgString(1,arg);
    strcat(p,arg);
    char id[4];
    _mes->getArgString(1,id);
    
    if (_mes->getArgInt32(0)) {
      //search for free module
      int m;
      for(m=0;m<MAX_MODULE;m++){
	if(!g->module[m].live) break;
      }
      //generate a new module
      g->module[m].live = true;
      g->module[m].id = int(id);

      strcpy(g->module[m].addRoute, p);
      strcat(g->module[m].addRoute, "/AddRoute");
      g->parser.addOscAddress((char *)g->module[m].addRoute, Gemini::addRoute);

      strcpy(g->module[m].delRoute, p);
      strcat(g->module[m].delRoute, "/DeleteRoute");
      g->parser.addOscAddress((char *)g->module[m].delRoute, Gemini::delRoute);

      for(int i=0;i<g->inputNum;i++){
	strcpy(g->module[m].inputAddr[i], p);
	strcat(g->module[m].inputAddr[i], g->inputAddr[i]);
	g->parser.addOscAddress(g->module[m].inputAddr[i], Gemini::dataReceived);
      }

      OSCMessage response;
      response.setAddress(destIP, 6341);
      response.beginMessage("/Coordinator/SetMdtkn");
      response.addArgString(p);
      response.addArgInt32(int(id));

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

      client.write((uint8_t *)&sizenl, sizeof(int32_t));
      client.write((uint8_t *)sendData, sizeof(uint8_t)*size);

    }else {
      //search for the same module 
      int m;
      for(m=0;m<MAX_MODULE;m++){
	if(g->module[m].id == int(id))
	  break;
      }
      //delete it
      g->module[m].live = false;
      g->module[m].id = -1;
      g->parser.delOscAddress(g->module[m].addRoute);
      g->parser.delOscAddress(g->module[m].delRoute);
      for(int i=0;i<g->inputNum;i++){
	g->parser.delOscAddress(g->module[m].inputAddr[i]);
      }
    }
}

void Gemini::addRoute(OSCMessage *_mes, void *ud){

}

void Gemini::delRoute(OSCMessage *_mes, void *ud){

}

void Gemini::dataReceived(OSCMessage *_mes, void *ud){
    //コールバック呼び出し
    Gemini *g = (Gemini *)ud;
    for (uint8_t i=0 ; i<MAX_MODULE ; i++) {
      for (uint8_t j=0 ; j<MAX_IO ; j++) {
	if(!g->module[i].live) continue;
	if (strcmp(g->module[i].inputAddr[j] , _mes->_oscAddress) == 0) 
	  g->inputCb[j](_mes->getArgInt32(0));
      }
    }
}
