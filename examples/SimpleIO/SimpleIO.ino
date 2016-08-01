#include <Gemini.h>

Gemini gemini;
const char* ssid = "intermediakanno";
const char* password = "kannolab";
const int LED = 16;
int cnt =0;
int sensorID, outputID;

void setup() {
  pinMode(LED, OUTPUT);
  
  //name input port with pin number or callback function. 
  gemini.addInput("/LED", LED);
  gemini.addInput("/inputFunc", &inputFunc);
  
  //name output port.
  //This function sets outputID in the second argument with which you will send a value.
  sensorID = gemini.addOutput("/SensorValue");
  outputID = gemini.addOutput("/outputFunc");
  
  gemini.begin("/SimpleIO", ssid, password);
}

void loop() {
  gemini.monitor();
  delay(1);
  if(cnt++%1000 == 0){
    gemini.send(outputID, cnt);
  }
}

void inputFunc(int val) {
  //do something
}

