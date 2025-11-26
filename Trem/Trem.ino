#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "env.h"

#define PIN_FRENTE 32
#define PIN_TRAS   33

#define LED_R 26
#define LED_G 27
#define LED_B 14

WiFiClientSecure client;
PubSubClient mqtt(client);

void setColor(int r, int g, int b) {
  digitalWrite(LED_R, r);
  digitalWrite(LED_G, g);
  digitalWrite(LED_B, b);
}

void piscarCor(int r, int g, int b, int tempo, int repeticoes) {
  for (int i = 0; i < repeticoes; i++) {
    setColor(r, g, b);
    delay(tempo);
    setColor(0, 0, 0);
    delay(tempo);
  }
}

void tremAdiante() {
  digitalWrite(PIN_FRENTE, HIGH);
  digitalWrite(PIN_TRAS, LOW);
}

void tremAtras() {
  digitalWrite(PIN_FRENTE, LOW);
  digitalWrite(PIN_TRAS, HIGH);
}

void pararTrem() {
  digitalWrite(PIN_FRENTE, LOW);
  digitalWrite(PIN_TRAS, LOW);
}

void publicarStatus(const char* status) {
  mqtt.publish(TOPIC_STATUS, status);
  Serial.println(String("Status enviado: ") + status);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.println("Mensagem recebida: " + msg);

  if (msg == "Trem_Adiante") {
    tremAdiante();
    publicarStatus("ANDANDO");
  } 
  else if (msg == "Trem_Atras") {
    tremAtras();
    publicarStatus("ANDANDO");
  }
  else if (msg == "Trem_Parar") {
    pararTrem();
    publicarStatus("PARADO");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(PIN_FRENTE, OUTPUT);
  pinMode(PIN_TRAS, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  client.setInsecure();
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  Serial.println("WiFi Conectado!");

  mqtt.setServer(BROKER_URL, BROKER_PORT);
  mqtt.setCallback(callback);

  Serial.println("Conectando ao Broker...");

  while (!mqtt.connected()) {
    mqtt.connect("Placa_Trem", BROKER_USER, BROKER_PASS);
    delay(300);
  }

  Serial.println("Broker Conectado!");

  piscarCor(0, 1, 0, 200, 3);
  mqtt.subscribe(TOPIC_TREM);
  setColor(0,1,0);
  publicarStatus("PARADO");
}

void loop() {
  mqtt.loop();

  if (digitalRead(PIN_FRENTE) || digitalRead(PIN_TRAS)) {
    piscarCor(1, 0, 0, 300, 1);
  } else {
    setColor(0, 1, 0);
  }

  delay(100);
}