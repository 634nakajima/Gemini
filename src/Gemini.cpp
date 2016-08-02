#include "Gemini.h"

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
    while(client.available()<5){
      delay(1);
    }

    if(client.available() > 4) {
      client.read(packet, sizeof(uint8_t)*512);
      tmes.remoteIP = udp.remoteIP();
      decoder.decode(&tmes, &packet[4]);
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
  for(int i=0; i<MAX_IO;i++){
	if(module[i].live){
	  for(int j=0;j<module[i].outAddrNum[outputID]; j++){
		OSCMessage mes;
		mes.remoteIP = module[i].outputIP[outputID][j];
		mes.beginMessage(module[i].outputAddr[outputID][j]);
		mes.addArgInt32((int32_t)v);
		mes.addArgInt32(0);
		sendMessage(&mes);
	  }
	}
  }
}

void Gemini::infoReqReceived(OSCMessage *_mes, void *ud){
  //send Module List
  Gemini *g = (Gemini *)ud;

  //make a OSCMessage
  for(int i=0; i<4;i++)
	g->coIP[i] = _mes->remoteIP[i];

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
  response.remoteIP = g->coIP;
  response.beginMessage("/ModuleList/setMList");
  response.addArgString(g->geminame);
  response.addArgString(inputAddrs);
  response.addArgString(outputAddrs);
  byte blob[] = {10,0,1,30};
  response.addArgBlob((char *)blob, 4);
  
  g->sendMessageTCP(&response);
}

void Gemini::moduleReqReceived(OSCMessage *_mes, void *ud){
  Gemini *g = (Gemini *)ud;
  char p[128], id[4];
  int module_new = _mes->getArgInt32(0);
  _mes->getArgString(1,id);
  int intID = atoi(id);

  //create a new module's address
  p[0] = '\0';
  strcat(p, g->geminame);
  strcat(p, "/Node");
  strcat(p,id);

  if(module_new) {//generate a new module
    g->setupModule(p, intID);

    //send request to make a module token
    OSCMessage response;
	response.remoteIP = g->coIP;
    response.beginMessage("/Coordinator/SetMdtkn");
    response.addArgString(p);
    response.addArgInt32(intID);
    g->sendMessageTCP(&response);

  }else {// delete the module
    g->flushModule(intID);

	//send request to make a module token
    OSCMessage response;
	response.remoteIP = g->coIP;
    response.beginMessage("/Coordinator/DeleteMdtkn");
    response.addArgString(p);
    response.addArgInt32(intID);
    g->sendMessageTCP(&response);
  }
}

void Gemini::addRoute(OSCMessage *_mes, void *ud){
  //コールバック呼び出し
  Gemini *g = (Gemini *)ud;
  char ip[32], osc[64];
  int32_t outID = _mes->getArgInt32(2);

  _mes->getArgString(0,ip);
  _mes->getArgString(1,osc);
  for (uint8_t i=0 ; i<MAX_MODULE ; i++) {
	if(!g->module[i].live) continue;
	if (strcmp(g->module[i].addRoute, _mes->_oscAddress)==0) {
	  g->module[i].addOutputAddr(ip, osc, outID);
	  break;
	}
  }
}

void Gemini::delRoute(OSCMessage *_mes, void *ud){
  //コールバック呼び出し
  Gemini *g = (Gemini *)ud;
  char osc[64];
  int32_t outID = _mes->getArgInt32(2);
  _mes->getArgString(1, osc);
  for (uint8_t i=0 ; i<MAX_MODULE ; i++) {
	if (strcmp(g->module[i].delRoute, _mes->_oscAddress)==0) {
	  g->module[i].delOutputAddr(osc, outID);
	  break;
	}
  }
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
  if (!client.connect(m->remoteIP, 6341)) {
    Serial.println("connection failed");
    return;
  }
  int32_t size32 = (int32_t)size;
  int32_t sizenl = htonl(size32);
  client.write((uint8_t *)&sizenl, sizeof(int32_t));
  client.write((uint8_t *)sendData, sizeof(uint8_t)*size);
}

void Gemini::sendMessage(OSCMessage *m){
  if(localCheck(m->remoteIP)){
	parser.patternComp(m);
	return;
  }
  //make binary packet
  uint8_t size = m->getMessageSize(); 
  uint8_t *sendData = (uint8_t*)calloc(size, 1);
  encoder.encode(m, sendData);

  //send packe
  udp.beginPacket(m->remoteIP, 6340);
  udp.write((uint8_t *)sendData, sizeof(uint8_t)*size);
  udp.endPacket();
  free(sendData);
}

void Gemini::setupModule(char *addr, int id){
  int m;
  for(m=0;m<MAX_MODULE;m++){
    if(!module[m].live) break;
  }
  if(m == MAX_MODULE) return;
  
  module[m].setup(addr, int(id));
  module[m].setInputAddr(inputAddr, inputNum);
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

  parser.delOscAddress(module[m].addRoute);
  parser.delOscAddress(module[m].delRoute);
  for(int i=0;i<inputNum;i++){
    parser.delOscAddress(module[m].inputAddr[i]);
  }
  module[m].flush();
}

bool Gemini::localCheck(IPAddress ip) {
  IPAddress local = WiFi.localIP();
  if(ip[0] == local[0] &&
	 ip[1] == local[1] &&
	 ip[2] == local[2] &&
	 ip[3] == local[3])
	return true;
  else
	return false;
}
































