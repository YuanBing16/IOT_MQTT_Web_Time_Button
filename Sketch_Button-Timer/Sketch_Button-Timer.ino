#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";
const char* mqtt_server = "broker.hivemq.com";  // Gunakan broker MQTT yang diinginkan

WiFiClient espClient;
PubSubClient client(espClient);

const int relayPin = D1;  // Pin untuk relay (misalnya mengendalikan lampu)

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);  // Pastikan relay mati di awal

  // Menghubungkan ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Menghubungkan ke Broker MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT Broker");
      client.subscribe("device/control");  // Subscribes to topic 'device/control'
    } else {
      delay(1000);
      Serial.println("Failed to connect to MQTT Broker. Retrying...");
    }
  }
}

void callback(char* topic, byte* message, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  if (msg == "ON") {
    digitalWrite(relayPin, HIGH);
  } else if (msg == "OFF") {
    digitalWrite(relayPin, LOW);
  }
}

void loop() {
  client.loop();
}
