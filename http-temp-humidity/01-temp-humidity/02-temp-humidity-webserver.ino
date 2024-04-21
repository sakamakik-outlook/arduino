
#include <Adafruit_AHTX0.h>
#include <WiFi.h>

const char* ssid = "";
const char* password = "";

Adafruit_AHTX0 aht;

WiFiServer server(80);

String header;

String ledPinState = "off";

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


void setup() {
  Wire.begin(5, 6);  // SDA -> 0, SCL -> 1
  Serial.begin(9600);
  Serial.println("Temp/Humidity Server!");
  delay(5000);
  while (!aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

const char* html_start = R"html(
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <title>VIPSDK MONITOR</title>
    <meta http-equiv="refresh" content="10">
  </head>
  <style type="text/css">
  </style>
  <body>
)html";

const char* html_end = R"html(
  </body>
  </html>
)html";


void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  WiFiClient client = server.available();

  if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println(html_start);

            client.print("<h1 style='background-color:MediumSeaGreen;'>Temperature:");
            client.print(temp.temperature);
            client.println("</h1>");

            client.print("<h1>Humidity: ");
            client.print(humidity.relative_humidity);
            client.println("</h1>");
            
            client.println(html_end);
            
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
