#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

// Used FIRST resistor 100KOhm connected from Bat+,
// them Pin A0 on D1 mini connected to that resistor,
// and SECOND resistor 100KOhm from that previous resistor to GND,
// and GND to d1 mini GND also with Bat-

const char* ssid = "ESP8266_Battery_Monitor";
const char* password = "12345678";

// === KALIBRÁCIA ===
// Vypočítané z nameraného raw a multimetra:
// raw = 508, V_meter = 3.89 -> CAL = 3.89 / 508 = 0.00765748
const float CALIB_RAW_TO_V = 0.00765748; // <--- vložené z tvojich meraní

// Napäťové hranice LiPo
const float BATTERY_MAX = 4.20;
const float BATTERY_MIN = 3.30;

unsigned long startMillis;

// Prevedenie priemerovania ADC pre stabilitu
int readAverageRaw(int samples = 10) {
  long sum = 0;
  for (int i = 0; i < samples; ++i) {
    sum += analogRead(A0);
    delay(5); // krátke zdržanie medzi čítaniami
  }
  return (int)(sum / samples);
}

float readBatteryVoltage() {
  int raw = readAverageRaw(10); // priemer z 10 meraní
  float batteryVoltage = raw * CALIB_RAW_TO_V;
  // zamedzíme absurdným hodnotám
  if (batteryVoltage < 0.0) batteryVoltage = 0.0;
  if (batteryVoltage > 6.0) batteryVoltage = 6.0;
  return batteryVoltage;
}

int voltageToPercent(float v) {
  if (v >= BATTERY_MAX) return 100;
  if (v <= BATTERY_MIN) return 0;
  return (int)((v - BATTERY_MIN) * 100.0 / (BATTERY_MAX - BATTERY_MIN));
}

String getBatteryColor(int percent) {
  if (percent >= 80) return "green";
  else if (percent >= 60) return "yellow";
  else if (percent >= 36) return "orange";
  else return "red";
}

String getUptime() {
  unsigned long seconds = (millis() - startMillis) / 1000;
  int hours = seconds / 3600;
  int minutes = (seconds % 3600) / 60;
  int secs = seconds % 60;
  char buffer[32];
  sprintf(buffer, "%dh %dm %ds", hours, minutes, secs);
  return String(buffer);
}

void handleRoot() {
  int raw = readAverageRaw(10);
  float voltage = raw * CALIB_RAW_TO_V;
  int percent = voltageToPercent(voltage);
  String color = getBatteryColor(percent);

  String warning = (percent <= 35) ? "<td style='color:red; font-weight:bold;'>⚠ Nabite batériu!</td>" : "<td>OK</td>";

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta http-equiv='refresh' content='2'>"
                "<title>ESP8266 Battery Monitor</title>"
                "<style>"
                "body{font-family:Arial;text-align:center;background:#f9f9f9;}"
                "table{margin:auto;border-collapse:collapse;width:70%;}"
                "th,td{border:1px solid #999;padding:10px;font-size:18px;}"
                "th{background:#4CAF50;color:#fff;}"
                "</style></head><body>"
                "<h2>🔋 ESP8266 Battery Monitor</h2>"
                "<p>raw ADC (avg): " + String(raw) + " &nbsp;&nbsp; Kalibr. faktor: " + String(CALIB_RAW_TO_V, 8) + "</p>"
                "<table>"
                "<tr><th>Napätie (V)</th><th>Nabitie (%)</th><th>Uptime</th><th>Výstraha</th></tr>";

  html += "<tr><td>" + String(voltage, 2) + "</td>";
  html += "<td style='color:" + color + "; font-weight:bold;'>" + String(percent) + " %</td>";
  html += "<td>" + getUptime() + "</td>";
  html += warning + "</tr></table>";
  html += "<p>Stránka sa obnovuje každé 2 sekundy</p>";
  html += "</body></html>";

  server.send(200, "text/html; charset=utf-8", html);
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("ESP8266 Battery Monitor - start");
  Serial.print("Kalibracny faktor (editovatelny): ");
  Serial.println(CALIB_RAW_TO_V, 8);

  WiFi.softAP(ssid, password);
  Serial.print("AP SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
  startMillis = millis();
}

void loop() {
  server.handleClient();
}
