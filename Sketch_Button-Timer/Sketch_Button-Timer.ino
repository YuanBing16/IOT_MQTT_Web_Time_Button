#include <ESP8266WiFi.h>               // Ganti dengan <ESP8266WiFi.h> jika pakai ESP8266
#include <PubSubClient.h>

// Ganti dengan WiFi Anda
const char* ssid = "SMUHERO.LAB";
const char* password = "Smuhero@LAB";

// MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Topik MQTT
const char* topics[] = {
  "coba/lampu1",
  "coba/lampu2",
  "coba/lampu3",
  "coba/lampu4"
};

const int NUM_LAMPU = 4;
const int pinRelay[NUM_LAMPU] = { D1, D2, D3, D6 }; // Ubah sesuai pin board Anda

WiFiClient espClient;
PubSubClient client(espClient);

// Fungsi untuk mengatur status relay
void kontrolLampu(int index, const String& payload) {
  if (index >= 0 && index < NUM_LAMPU) {
    digitalWrite(pinRelay[index], (payload == "1") ? LOW : HIGH);
    Serial.printf("Lampu %d diatur ke: %s\n", index + 1, (payload == "1") ? "NYALA" : "MATI");

    // Kirim status balik (opsional)
    String statusTopic = String(topics[index]) + "/status";
    client.publish(statusTopic.c_str(), payload.c_str(), true);  // QoS 0, retained
  }
}


// Callback saat pesan diterima
void callback(char* topic, byte* message, unsigned int length) {
  String payload;
  for (unsigned int i = 0; i < length; i++) {
    payload += (char)message[i];
  }

  for (int i = 0; i < NUM_LAMPU; i++) {
    if (String(topic) == topics[i]) {
      kontrolLampu(i, payload);
      break;
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan MQTT...");
    String clientId = "ESPClient-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Terhubung!");
      for (int i = 0; i < NUM_LAMPU; i++) {
        client.subscribe(topics[i]);
        Serial.println("Berlangganan topik: " + String(topics[i]));
      }
    } else {
      Serial.print("Gagal, rc=");
      Serial.print(client.state());
      Serial.println(" coba lagi dalam 5 detik...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Inisialisasi pin relay
  for (int i = 0; i < NUM_LAMPU; i++) {
    pinMode(pinRelay[i], OUTPUT);
    digitalWrite(pinRelay[i], LOW); // Awal kondisi MATI
  }

  // Koneksi WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi!");

  // Setup MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
