// write me an arduino program that will have a simple file browser on a web page
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiServer.h>
#include <FS.h>


// Replace with your network credentials
const char* ssid = "ESP32-Access-Point";
const char* password = "123456789";

WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputState = "off";

// Assign output variables to GPIO pins
const int output = 2;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // display a file browser using spiffs on the web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 File Browser</h1>");
            
            // Check if there is any SPIFFS file
            File root = SPIFFS.open("/");
            File file = root.openNextFile();
            if (!file) {
              client.println("<h2>No files found in SPIFFS!</h2>");
            } else {
              client.println("<h2>Files found in SPIFFS:</h2>");
              while (file) {
                client.print("<a href=\"");
                client.print(file.name());
                client.print("\">");
                client.print(file.name());
                client.println("</a>");
                file = root.openNextFile();
              }
            }
            client.println("</body></html>");

            // add a way to upload files to the SPIFFS
            client.println("<form method=\"POST\" action=\"/upload\" enctype=\"multipart/form-data\">");
            client.println("<input type=\"file\" name=\"data\">");
            client.println("<input class=\"button\" type=\"submit\" value=\"Upload\">");
            client.println("</form>");

            // save the file to the SPIFFS
            if (header.indexOf("GET /upload") >= 0) {
              Serial.println("Saving file to SPIFFS");
              File file = SPIFFS.open("/test.txt", FILE_WRITE);
              if (!file) {
                Serial.println("Failed to create file");
                return;
              }
              if (file.print("This is a test file")) {
                Serial.println("File was written");
              } else {
                Serial.println("File write failed");
              }
              file.close();
            }

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character, 

          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}