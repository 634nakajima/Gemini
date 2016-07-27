#include <Gemini.h>

Gemini gemini;
const char* ssid = "intermediakanno";
const char* password = "kannolab";

void setup() {
  // put your setup code here, to run once:
    gemini.begin(ssid, password);
}

void loop() {
  // put your main code here, to run repeatedly:

}
