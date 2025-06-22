  #include <Servo.h>
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>

  #define TRIG_PIN D5
  #define ECHO_PIN D6
  #define SERVO_PIN D7
  #define BUZZER_PIN D8  // Tambahan: pin buzzer

  const char* ssid = "ujang";
  const char* password = "nanananang";
  const char* serverUrl = "http://192.168.134.74:5000/post_data";

  Servo radarServo;

  long readDistanceCM() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
    long distance = duration * 0.034 / 2;
    return distance;
  }

  void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT); // Tambahan: set pin buzzer sebagai output
    digitalWrite(BUZZER_PIN, LOW); // Tambahan: matikan buzzer di awal
    radarServo.attach(SERVO_PIN);

    WiFi.begin(ssid, password);
    Serial.print("Menghubungkan WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Terhubung. IP ESP: ");
    Serial.println(WiFi.localIP());
    Serial.println("Radar dimulai...");
  }

  void sendToServer(int angle, int distance) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      http.begin(client, serverUrl);
      http.addHeader("Content-Type", "application/json");

      String payload = "{\"angle\":" + String(angle) + ",\"distance\":" + String(distance) + "}";
      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0) {
        Serial.print("Terkirim: ");
        Serial.println(httpResponseCode);
      } else {
        Serial.print("Gagal kirim: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }
  }

  void loop() {
    if (!getRadarStatusFromServer()) {
      digitalWrite(BUZZER_PIN, LOW); // Pastikan buzzer mati saat radar off
      delay(500);
      return;
    }

    for (int angle = 0; angle <= 180; angle += 10) {
      if (!getRadarStatusFromServer()) return;

      radarServo.write(angle);
      delay(200);
      long distance = readDistanceCM();

      if (distance > 0 && distance < 10) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }

      Serial.print("Sudut: ");
      Serial.print(angle);
      Serial.print("°, Jarak: ");
      Serial.print(distance);
      Serial.println(" cm");

      sendToServer(angle, distance);
    }

    for (int angle = 180; angle >= 0; angle -= 10) {
      if (!getRadarStatusFromServer()) return;

      radarServo.write(angle);
      delay(200);
      long distance = readDistanceCM();

      if (distance > 0 && distance < 10) {
        digitalWrite(BUZZER_PIN, HIGH);
      } else {
        digitalWrite(BUZZER_PIN, LOW);
      }

      Serial.print("Sudut: ");
      Serial.print(angle);
      Serial.print("°, Jarak: ");
      Serial.print(distance);
      Serial.println(" cm");

      sendToServer(angle, distance);
    }
  }

  bool getRadarStatusFromServer() {
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      http.begin(client, "http://192.168.134.74:5000/get_radar");
      int httpCode = http.GET();

      if (httpCode > 0) {
        String payload = http.getString();
        http.end();
        return payload.indexOf("\"on\":true") > 0;
      }

      http.end();
    }
    return false; // default jika gagal, radar dianggap mati
  }
