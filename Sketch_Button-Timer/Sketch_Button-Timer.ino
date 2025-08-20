#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Ganti sesuai jaringanmu
const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";

// MQTT Broker
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Pin relay (untuk kontrol lampu)
const int relay1 = D1;  // GPIO5
const int relay2 = D2;  // GPIO4
const int relay3 = D3;  // GPIO0
const int relay4 = D6;  // GPIO12

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi menerima pesan dari topik MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  String pesan;
  for (int i = 0; i < length; i++) {
    pesan += (char)payload[i];
  }

  Serial.print("Pesan diterima: ");
  Serial.print(topic);
  Serial.print(" = ");
  Serial.println(pesan);

  // Kontrol relay berdasarkan topik yang diterima
  if (String(topic) == "coba/lampu1") {
    digitalWrite(relay1, pesan == "1" ? LOW : HIGH);  // ON jika "1", OFF jika "0"
  } else if (String(topic) == "coba/lampu2") {
    digitalWrite(relay2, pesan == "1" ? LOW : HIGH);
  } else if (String(topic) == "coba/lampu3") {
    digitalWrite(relay3, pesan == "1" ? LOW : HIGH);
  } else if (String(topic) == "coba/lampu4") {
    digitalWrite(relay4, pesan == "1" ? LOW : HIGH);
  }
}

// Fungsi untuk memastikan koneksi dengan broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke MQTT...");
    String clientId = "ESP_" + String(random(0xffff), HEX);  // Client ID unik
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung ke MQTT!");

      // Subscribe ke semua topik untuk mengontrol lampu
      client.subscribe("coba/lampu1");
      client.subscribe("coba/lampu2");
      client.subscribe("coba/lampu3");
      client.subscribe("coba/lampu4");
    } else {
      Serial.print("Gagal terhubung, rc=");
      Serial.print(client.state());
      Serial.println(" mencoba lagi dalam 5 detik.");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Atur pin relay sebagai output
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);

  // Matikan semua relay (aktif dengan HIGH)
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);

  // Koneksi ke Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi terhubung");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT client
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();  // Cek koneksi ke MQTT, coba lagi jika gagal
  }
  client.loop();  // Proses pesan masuk
}
