#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

// Access point credentials
const char* ssid = "ESP32_AP";
const char* password = "your_AP_PASSWORD";

WebServer server(80);

const size_t maxLines = 25;
String logContent;

String readLastNLines(const char* path, size_t n) {
  File file = SPIFFS.open(path, "r");
  if (!file) {
    Serial.printf("Failed to open file %s for reading\n", path);
    return String();
  }

  size_t lineCount = 0;
  String lines[maxLines];
  while (file.available()) {
    String line = file.readStringUntil('\n');
    lines[lineCount % maxLines] = line;
    lineCount++;
  }
  file.close();

  String lastNLines;
  if (lineCount < maxLines) {
    for (size_t i = 0; i < lineCount; i++) {
      lastNLines += lines[i] + '\n';
    }
  } else {
    for (size_t i = lineCount % maxLines; i < maxLines; i++) {
      lastNLines += lines[i] + '\n';
    }
    for (size_t i = 0; i < lineCount % maxLines; i++) {
      lastNLines += lines[i] + '\n';
    }
  }
  return lastNLines;
}

void updateLogContent(void* parameter) {
  while (1) {
    logContent = readLastNLines("/log.txt", maxLines);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void handleRoot() {
  String htmlContent = "<html><head><title>ESP32 Log Viewer</title></head><body><pre>" + logContent + "</pre><br><a href=\"/download\" download=\"log.txt\">Download Log</a></body></html>";
  server.send(200, "text/html", htmlContent);
}

void handleDownload() {
  server.streamFile(SPIFFS.open("/log.txt", "r"), "text/plain");
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handleRoot);
  server.on("/download", handleDownload);
  server.begin();

  xTaskCreate(updateLogContent, "Update Log Content", 4096, NULL, 1, NULL);
}

void loop() {
  server.handleClient();
}