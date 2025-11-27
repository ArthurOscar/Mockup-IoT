#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "env.h"

WiFiClientSecure client;
PubSubClient mqtt(client);

// Pinos dos sensores ultrassônicos
const byte TRIGGER_PIN_1 = 19;
const byte ECHO_PIN_1    = 13;

const byte TRIGGER_PIN_2 = 27;
const byte ECHO_PIN_2    = 18;

const byte LED_PIN = 12;

unsigned long lastMsg = 0;

// Estado anterior de detecção de cada sensor
bool objSensor1 = false;
bool objSensor2 = false;

// ------------------------ WiFi ------------------------
void conectaWiFi() {
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ------------------------ MQTT ------------------------
void conectaMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Conectando ao broker MQTT...");

    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str(), BROKER_USER, BROKER_PASS)) {
      Serial.println("Conectado!");
      mqtt.subscribe(TOPIC_ILUM);
    } else {
      Serial.print("Falha (rc=");
      Serial.print(mqtt.state());
      Serial.println("), tentando em 5s...");
      delay(5000);
    }
  }
}

// ------------------------ SENSOR ------------------------
long lerDistancia(byte triggerPin, byte echoPin) {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(3);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(12);
  digitalWrite(triggerPin, LOW);

  long duracao = pulseIn(echoPin, HIGH, 20000);

  if (duracao == 0) return -1; // leitura inválida

  return (duracao * 0.034) / 2;  // distância em cm
}

// ------------------------ CALLBACK ------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;

  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Msg em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);

  if (String(topic) == TOPIC_ILUM) {
    if (msg == "Acender") {
      digitalWrite(LED_PIN, HIGH);
    } else if (msg == "Apagar") {
      digitalWrite(LED_PIN, LOW);
    }
  }
}

// ------------------------ SETUP ------------------------
void setup() {
  Serial.begin(115200);

  pinMode(TRIGGER_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  
  pinMode(TRIGGER_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  pinMode(LED_PIN, OUTPUT);

  client.setInsecure();

  conectaWiFi();
  mqtt.setServer(BROKER_URL, BROKER_PORT);
  mqtt.setCallback(callback);

  conectaMQTT();
}

// ------------------------ LOOP ------------------------
void loop() {
  if (!mqtt.connected()) conectaMQTT();
  mqtt.loop();

  unsigned long agora = millis();

  if (agora - lastMsg > 2000) {
    lastMsg = agora;

    long d1 = lerDistancia(TRIGGER_PIN_1, ECHO_PIN_1);
    long d2 = lerDistancia(TRIGGER_PIN_2, ECHO_PIN_2);

    Serial.print("Sensor 1: "); Serial.print(d1); Serial.println(" cm");
    Serial.print("Sensor 2: "); Serial.print(d2); Serial.println(" cm");

    // ---------- Sensor 1 ----------
    if (d1 > 0 && d1 < 10) {
      if (!objSensor1) {
        mqtt.publish(TOPIC_ILUM, "Objeto detectado no sensor 1");
        objSensor1 = true;
      }
    } else {
      objSensor1 = false;
    }

    // ---------- Sensor 2 ----------
    if (d2 > 0 && d2 < 10) {
      if (!objSensor2) {
        mqtt.publish(TOPIC_ILUM, "Objeto detectado no sensor 2");
        objSensor2 = true;
      }
    } else {
      objSensor2 = false;
    }
  }
}