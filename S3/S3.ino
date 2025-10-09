#include <WiFi.h>

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

void setup() {
  Serial.begin(115200);
  Serial.println("Conectanto ao WiFi"); 
  WiFi.begin(SSID,PASS);
  while(WiFi.status() !=WL_CONNECTED){
    Serial.print(".");
    delay(200);
}
Serial.println("\nConectando com Sucesso!");
}

void loop() {
  
}
