#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Replace with your network credentials
const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// MQTT Topics
const char* controlTopic1 = "coba/lampu1";  // Control topic for Lampu 1
const char* statusTopic1 = "status/lampu1";  // Status topic for Lampu 1

const char* controlTopic2 = "coba/lampu2";  // Control topic for Lampu 2
const char* statusTopic2 = "status/lampu2";  // Status topic for Lampu 2

// Pin definitions for relays or lights
const int lampu1Pin = D1;  // Pin for Lampu 1 (D1 pin on ESP8266)
const int lampu2Pin = D2;  // Pin for Lampu 2 (D2 pin on ESP8266)

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  setupWiFi();

  // Initialize relay pins
  pinMode(lampu1Pin, OUTPUT);
  pinMode(lampu2Pin, OUTPUT);
  digitalWrite(lampu1Pin, LOW);  // Make sure the lamp is OFF initially
  digitalWrite(lampu2Pin, LOW);  // Make sure the lamp is OFF initially

  // Set up MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void setupWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("Connected to MQTT broker");

      // Subscribe to control topics
      client.subscribe(controlTopic1);
      client.subscribe(controlTopic2);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String receivedMessage = "";

  // Convert the payload into a string
  for (int i = 0; i < length; i++) {
    receivedMessage += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(receivedMessage);

  // Control the light based on the message received
  if (String(topic) == controlTopic1) {
    if (receivedMessage == "1") {
      digitalWrite(lampu1Pin, HIGH);  // Turn Lampu 1 ON
      client.publish(statusTopic1, "1");  // Send the status to the status topic
    } else {
      digitalWrite(lampu1Pin, LOW);  // Turn Lampu 1 OFF
      client.publish(statusTopic1, "0");  // Send the status to the status topic
    }
  }

  if (String(topic) == controlTopic2) {
    if (receivedMessage == "1") {
      digitalWrite(lampu2Pin, HIGH);  // Turn Lampu 2 ON
      client.publish(statusTopic2, "1");  // Send the status to the status topic
    } else {
      digitalWrite(lampu2Pin, LOW);  // Turn Lampu 2 OFF
      client.publish(statusTopic2, "0");  // Send the status to the status topic
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
