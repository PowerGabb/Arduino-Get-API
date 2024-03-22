#include <Arduino.h>
#include <ArduinoJson.h>
#include <ArduinoHttpClient.h> // Untuk ESP8266 atau ESP32
//#include <Ethernet.h>         // Untuk Ethernet Shield
#include <ESP8266WiFi.h>       // Untuk ESP8266
//#include <WiFi.h>            // Untuk ESP32

// Define server details
const char serverAddress[] = "192.168.1.1"; // Ganti dengan alamat IP atau URL server Anda
const int serverPort = 8000; // Port 8000

// Create HttpClient instance
WiFiClient wifiClient; // Untuk ESP8266 atau ESP32
//EthernetClient ethernetClient; // Untuk Ethernet Shield
HttpClient httpClient = HttpClient(wifiClient, serverAddress, serverPort);

// Pin untuk mengontrol pompa penyiram otomatis
const int pumpPin = D8; // Ganti dengan pin yang sesuai

void setup() {
  Serial.begin(9600);
  
  // Mengatur pin pompa sebagai OUTPUT
  pinMode(pumpPin, OUTPUT);

  // Inisialisasi koneksi jaringan
  // Untuk ESP8266 atau ESP32
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to WiFi");

  // Tunggu beberapa saat untuk memastikan koneksi stabil
  delay(2000);
}

void runAutomaticWateringSystem() {
  // Logika program penyiram otomatis
  digitalWrite(pumpPin, HIGH); // Hidupkan pompa
  delay(5000); // Jalankan pompa selama 5 detik (contoh)
  digitalWrite(pumpPin, LOW); // Matikan pompa
}

void loop() {
  // Kirim permintaan GET ke server
  httpClient.beginRequest();
  httpClient.get("/arduino-set");
  httpClient.sendHeader("Connection", "close");
  httpClient.endRequest();

  // Baca respons dari server
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  if (statusCode == 200) { // Jika permintaan berhasil
    Serial.print("Response: ");
    Serial.println(response);

    // Buat objek DynamicJsonDocument untuk menampung respons JSON
    DynamicJsonDocument doc(1024); // Sesuaikan ukurannya dengan kebutuhan Anda

    // Parse respons JSON
    DeserializationError error = deserializeJson(doc, response);

    // Periksa jika parsing berhasil
    if (error) {
      Serial.print("Parsing failed: ");
      Serial.println(error.c_str());
      return;
    }

    // Baca nilai "value" dari respons JSON
    bool value = doc["value"];

    if (value) {
      // Jalankan program penyiram otomatis jika nilai true
      Serial.println("Value is true. Running automatic watering system...");
      runAutomaticWateringSystem();
    } else {
      // Tunggu sebelum memeriksa lagi jika nilai false
      Serial.println("Value is false. Waiting...");
      delay(60000); // Tunggu 1 menit sebelum memeriksa lagi
    }
  } else {
    Serial.print("Error: ");
    Serial.println(statusCode);
  }

  // Tunggu sebelum memeriksa lagi
  delay(1000); // Tunggu 1 detik sebelum memeriksa lagi
}
