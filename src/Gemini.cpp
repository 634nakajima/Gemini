#include <Gemini.h>

WiFiServer server(6341);

Gemini::Gemini(){
  inputNum = 0;
  outputNum = 0;
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

  return inputNum++;
}
int Gemini::addInput(char *inAddr, void (*inputCallback)(int)){
  //perserにinAddrでコールバック登録
  inputAddr[inputNum] = inAddr;
  inputCb[inputNum] = inputCallback;
    
  return inputNum++;
}

int Gemini::addOutput(const char *outAddr) {
  outputAddr[outputNum] = (char *)outAddr;
  
  return outputNum++;
}

void Gemini::send(int outputID, int v){
}

void Gemini::infoReqReceived(OSCMessage *_mes, void *ud){
  //send Module List
  Gemini *g = (Gemini *)ud;

  //make a OSCMessage
  byte destIP[]= {_mes->remoteIP[0],
		  _mes->remoteIP[1],
		  _mes->remoteIP[2],
		  _mes->remoteIP[3]};

  char inputAddrs[128], outputAddrs[128];
    memset(inputAddrs,'\0', 128);
    memset(outputAddrs,'\0', 128);
  for(int i=0;i<g->inputNum;i++){
    if(strlen(inputAddrs)+strlen(g->inputAddr[i])<128){
      strcat(inputAddrs, g->inputAddr[i]);
        if(i != g->inputNum)strcat(inputAddrs, ",");
    }
  }
  for(int i=0;i<g->outputNum;i++){
    if(strlen(outputAddrs)+strlen(g->outputAddr[i])<128){
      strcat(outputAddrs, g->outputAddr[i]);
        if(i != g->outputNum)strcat(outputAddrs, ",");

    }
  }
  OSCMessage response;
  response.setAddress(destIP, 6341);
  response.beginMessage("/ModuleList/setMList");
  response.addArgString(g->geminame);
  response.addArgString(inputAddrs);
  response.addArgString(outputAddrs);
  response.addArgBlob((const char *)destIP, 4);
  
  g->sendMessageTCP(&response);
}

void Gemini::moduleReqReceived(OSCMessage *_mes, void *ud){
  Gemini *g = (Gemini *)ud;
  char p[128], id[4];
  int module_new = _mes->getArgInt32(0);
  _mes->getArgString(1,id);
  byte destIP[]= {_mes->remoteIP[0],
		  _mes->remoteIP[1],
		  _mes->remoteIP[2],
		  _mes->remoteIP[3]};

  //create a new module's address
  for(int i=0;i<4;i++)
    p[i] = _mes->remoteIP[i];
  p[4] = '\0';  
  strcat(p, g->geminame);
  strcat(p, "/Node");
  strcat(p,id);

  if(module_new) {//generate a new module
    g->setupModule(p, int(id));

    //send request to make a module token
    OSCMessage response;
    response.remoteIP = _mes->remoteIP;
    response.setAddress(destIP, 6341);
    response.beginMessage("/Coordinator/SetMdtkn");
    response.addArgString(p);
    response.addArgInt32(int(id));
    g->sendMessageTCP(&response);

  }else// delete the module
    g->flushModule(int(id));
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
      if (strcmp(g->module[i].inputAddr[j], _mes->_oscAddress)==0) 
	g->inputCb[j](_mes->getArgInt32(0));
    }
  }
}

void Gemini::sendMessageTCP(OSCMessage *m){
  //make binary packet
  uint8_t size = m->getMessageSize(); 
  uint8_t *sendData = (uint8_t*)calloc(size, 1);
  encoder.encode(m, sendData);

  //send packet
  WiFiClient client;
  if (!client.connect(m->getIpAddress(), 6341)) {
    Serial.println("connection failed");
    return;
  }
  int32_t size32 = (int32_t)size;
  int32_t sizenl = htonl(size32);
  client.write((uint8_t *)&sizenl, sizeof(int32_t));
  client.write((uint8_t *)sendData, sizeof(uint8_t)*size);
}

void Gemini::setupModule(char *addr, int id){
  int m;
  for(m=0;m<MAX_MODULE;m++){
    if(!module[m].live) break;
  }
  if(m == MAX_MODULE) return;
  
  module[m].setup(addr, int(id));
  parser.addOscAddress((char *)module[m].addRoute,
		       Gemini::addRoute);
  parser.addOscAddress((char *)module[m].delRoute,
		       Gemini::delRoute);
  for(int i=0;i<inputNum;i++){
    parser.addOscAddress(module[m].inputAddr[i],
			 Gemini::dataReceived);
  }
}

void Gemini::flushModule(int id){
  //search for the same module 
  int m;
  for(m=0;m<MAX_MODULE;m++){
    if(module[m].id == id)
      break;
  }
  if(m == MAX_MODULE) return;

  module[m].flush();
  parser.delOscAddress(module[m].addRoute);
  parser.delOscAddress(module[m].delRoute);
  for(int i=0;i<inputNum;i++){
    parser.delOscAddress(module[m].inputAddr[i]);
  }
}
