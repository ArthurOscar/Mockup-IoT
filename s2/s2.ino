#include <Wifi.h>
#include <PubSubClient.h>

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
String BoardID = "S2";
BoardID += String(random(0xffff),HEX);
mqtt.connect(BoardID.c_str(), BrokerUser.c_str() , BrokerPass.c_str());

While(!mqtt.connected()){
  Serial.print(".");
  delay(200);
 }
 mqtt.subscribe("Topico-DSM14")
 Serial,println("\nConectado ao Broker")
}

void loop() {
  String mensagem = "Lucas: taoo seu zika";
  mensagem =="taoo seu zika"
  mqtt.publish("Topico-DSM14", mensagem.c_str())
  mqtt.loop();
  delay(1000);
  
}

void callback(char* topic, type*paylod,unsigmed0)