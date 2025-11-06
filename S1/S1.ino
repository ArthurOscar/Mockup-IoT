#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "env.h"

WiFiClientSecure client;
PubSubClient mqtt(client);


const byte ldr = 34;
int threshold = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ldr, INPUT);
  pinMode(19, OUTPUT);
  client.setInsecure();
  Serial.println("Conectando no WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASS);  //Tenta conectar na rede
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(200);
  }
  Serial.println("\nConectado com Sucesso no WiFi!");

  Serial.println("Conectando ao Broker...");
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  String BoardID = "S1";
  BoardID += String(random(0xffff), HEX);
  mqtt.connect(BoardID.c_str(), BROKER_USER, BROKER_PASS);
  while (!mqtt.connected()) {
    Serial.print(".");
    delay(200);
  }
  mqtt.subscribe(TOPIC_ILUM);
  mqtt.setCallback(callback);  // Recebe a mensagem
  Serial.println("\nConectado ao Broker!");
}

void loop() {
  int valor_ldr = analogRead(ldr);
  // Serial.println(valor_ldr);
  if (valor_ldr > 2500) {
    mqtt.publish(TOPIC_ILUM, "Acender");  // Envia a mensagem
    mqtt.loop();
    delay(1000);
  } else {
    mqtt.publish(TOPIC_ILUM, "Apagar");  // Envia a mensagem
    mqtt.loop();
    delay(1000);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg = msg.c_str();
  if (strcmp(topic,TOPIC_ILUM) == 0 && msg == "Acender") {  // Verifica mensagem do tópico de luz do S1 e fala para acender led
    Serial.println(msg);
    digitalWrite(19, HIGH);
  } else if (strcmp(topic,TOPIC_ILUM)==0 && msg == "Apagar") {  // Verifica mensagem do tópico de luz do S1 e fala para apagar led
    digitalWrite(19, LOW);
    Serial.println(msg);
  }
}