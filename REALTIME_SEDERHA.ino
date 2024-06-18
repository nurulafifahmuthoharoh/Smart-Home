#include <Time.h>
#include <FirebaseESP32.h>
#include <WiFi.h>


#define FIREBASE_HOST "home-iot-7aded-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "IMj0hntrfulYTMBg46MuAJ9TFctSGEOeX9Wlveh6"
#define WIFI_SSID "SKRIPSI"
#define WIFI_PASSWORD "raspberry"

FirebaseData firebaseData;
WiFiClient wifiClient;

int lampu5 = 25; // Pin untuk relay lampu5
int pompa = 26;  // Pin untuk relay pompa

void setup() {
  Serial.begin(115200);
  pinMode(lampu5, OUTPUT);
  pinMode(pompa, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, wifiClient);
  Firebase.reconnectWiFi(true);

  // Mulai dengan relay mati
  digitalWrite(lampu5, LOW);
  digitalWrite(pompa, LOW);
}

void loop() {
  if (Firebase.getString(firebaseData, "DATA/WAKTU/LAMPU5_ON") &&
      Firebase.getString(firebaseData, "DATA/WAKTU/LAMPU5_OFF")) {
    String waktuON = firebaseData.stringData();
    Firebase.getString(firebaseData, "DATA/WAKTU/LAMPU5_OFF");
    String waktuOFF = firebaseData.stringData();
    if (isTimeToTurnOn(waktuON, waktuOFF)) {
      digitalWrite(lampu5, HIGH); // Aktifkan relay lampu5
    } else {
      digitalWrite(lampu5, LOW); // Matikan relay lampu5
    }
  }

  if (Firebase.getString(firebaseData, "DATA/WAKTU/POMPA_ON") &&
      Firebase.getString(firebaseData, "DATA/WAKTU/POMPA_OFF")) {
    String waktuON = firebaseData.stringData();
    Firebase.getString(firebaseData, "DATA/WAKTU/POMPA_OFF");
    String waktuOFF = firebaseData.stringData();
    if (isTimeToTurnOn(waktuON, waktuOFF)) {
      digitalWrite(pompa, HIGH); // Aktifkan relay pompa
    } else {
      digitalWrite(pompa, LOW); // Matikan relay pompa
    }
  }

  delay(1000); // Tunda pembacaan data agar tidak terlalu sering
}

bool isTimeToTurnOn(String waktuON, String waktuOFF) {
  // Mendapatkan waktu saat ini
  int jamSekarang = hour();
  int menitSekarang = minute();

  // Mendapatkan jam dan menit dari waktu ON dan OFF
  int jamON = waktuON.substring(0, 2).toInt();
  int menitON = waktuON.substring(3, 5).toInt();
  int jamOFF = waktuOFF.substring(0, 2).toInt();
  int menitOFF = waktuOFF.substring(3, 5).toInt();

  // Memeriksa apakah saat ini berada dalam rentang waktu ON atau OFF
  if ((jamSekarang > jamON || (jamSekarang == jamON && menitSekarang >= menitON)) &&
      (jamSekarang < jamOFF || (jamSekarang == jamOFF && menitSekarang < menitOFF))) {
    return true;
  } else {
    return false;
  }
}
