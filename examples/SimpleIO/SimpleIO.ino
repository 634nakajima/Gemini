#include <Gemini.h>

Gemini gemini;
const char* ssid = "intermediakanno";
const char* password = "kannolab";
const int LED = 16;
int cnt =0;
int sensorID, outputID;

void setup() {
  pinMode(LED, OUTPUT);
  
  //name input port with callback function. 
  gemini.addInput("/LED", &led);
  gemini.addInput("/InputFunc", &inputFunc);
  
  //name output port.
  //This function returns outputID with which you will send a value.
  sensorID = gemini.addOutput("/SensorValue");
  outputID = gemini.addOutput("/OutputFunc");
  
  gemini.begin("/SimpleIO", ssid, password);
}

void loop() {
  gemini.monitor();
  delay(1);
  if(cnt++%100 == 0){
    gemini.send(outputID, cnt%1000);
  }
}

void led(int val) {
  analogWrite(LED, val);
}

void inputFunc(int val) {
  Serial.print("Input: ");
  Serial.println(val);
}
