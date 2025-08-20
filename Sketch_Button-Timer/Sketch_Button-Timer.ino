#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Ganti dengan kredensial WiFi Anda
const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";

// Ganti dengan alamat broker MQTT
const char* mqttServer = "broker.hivemq.com";  // Misalnya broker publik
const int mqttPort = 1883;
const char* mqttUser = "";  // Jika ada, isi username
const char* mqttPassword = "";  // Jika ada, isi password

WiFiClient espClient;
PubSubClient client(espClient);

// Topik untuk kontrol lampu
const char* lampu1Topic = "coba/lampu1";
const char* lampu2Topic = "coba/lampu2";
const char* lampu3Topic = "coba/lampu3";
const char* lampu4Topic = "coba/lampu4";

// Pin untuk lampu (ubah sesuai dengan pengaturan Anda)
const int lampu1Pin = D1;  // Ganti dengan pin yang sesuai
const int lampu2Pin = D2;
const int lampu3Pin = D3;
const int lampu4Pin = D4;

void setup() {
  // Mulai komunikasi serial untuk debug
  Serial.begin(115200);
  
  // Set pin lampu sebagai output
  pinMode(lampu1Pin, OUTPUT);
  pinMode(lampu2Pin, OUTPUT);
  pinMode(lampu3Pin, OUTPUT);
  pinMode(lampu4Pin, OUTPUT);

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menyambung ke WiFi...");
  }
  Serial.println("Terhubung ke WiFi");

  // Mengatur broker MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);

  // Menyambung ke MQTT
  reconnectMQTT();
}

void loop() {
  // Menjaga koneksi dengan broker MQTT
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
}

void reconnectMQTT() {
  // Loop sampai terhubung ke broker
  while (!client.connected()) {
    Serial.println("Mencoba untuk terhubung ke broker MQTT...");
    
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("Terhubung ke broker MQTT");
      
      // Berlangganan ke topik untuk kontrol lampu
      client.subscribe(lampu1Topic);
      client.subscribe(lampu2Topic);
      client.subscribe(lampu3Topic);
      client.subscribe(lampu4Topic);
    } else {
      Serial.print("Gagal, mencoba lagi dalam 5 detik...");
      delay(5000);
    }
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Mengonversi payload menjadi string
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  // Periksa topik dan kontrol lampu sesuai perintah
  if (String(topic) == lampu1Topic) {
    if (message == "1") {
      digitalWrite(lampu1Pin, HIGH);  // Nyalakan lampu 1
    } else if (message == "0") {
      digitalWrite(lampu1Pin, LOW);   // Matikan lampu 1
    }
  }

  if (String(topic) == lampu2Topic) {
    if (message == "1") {
      digitalWrite(lampu2Pin, HIGH);  // Nyalakan lampu 2
    } else if (message == "0") {
      digitalWrite(lampu2Pin, LOW);   // Matikan lampu 2
    }
  }

  if (String(topic) == lampu3Topic) {
    if (message == "1") {
      digitalWrite(lampu3Pin, HIGH);  // Nyalakan lampu 3
    } else if (message == "0") {
      digitalWrite(lampu3Pin, LOW);   // Matikan lampu 3
    }
  }

  if (String(topic) == lampu4Topic) {
    if (message == "1") {
      digitalWrite(lampu4Pin, HIGH);  // Nyalakan lampu 4
    } else if (message == "0") {
      digitalWrite(lampu4Pin, LOW);   // Matikan lampu 4
    }
  }
}
