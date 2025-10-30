#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;
PubSubClient mqtt(client);

const String BrokerURL = "test.mosquitto.org";
const int  BrokerPort =1883;
const String BrokerUser ="";
const String BrokerPass ="";

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

Serial.println("Conectando ao Broker...");
mqtt.setServer(BrokerURL.c_str(),BrokerPort);
String BoardID = "S3";
BoardID += String(random(0xffff),HEX);
mqtt.connect(BoardID.c_str(), BrokerUser.c_str() , BrokerPass.c_str());

while(!mqtt.connected()){
  Serial.print(".");
  delay(200);
 }
 Serial.println("\nConectado ao Broker");
}

void loop() {
 String mensagem = "Leonardo Henrique: ";
 mensagem += "oi gato de botas";
 mqtt.publish("Topico-DSM14" , mensagem.c_str());
 mqtt.loop();
 delay(1000);
 
}
