#include <WiFi.h>

const char* ssid = "Helix";
const char* password = "Sakamakik7!";

WiFiServer server(80);

String header;

String ledPinState = "off";

const int ledPin = 8;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);


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

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /ledPin/on") >= 0) {
              Serial.println("GPIO 26 on");
              ledPinState = "on";
              digitalWrite(ledPin, HIGH);
            } else if (header.indexOf("GET /ledPin/off") >= 0) {
              Serial.println("GPIO 26 off");
              ledPinState = "off";
              digitalWrite(ledPin, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head></head>");
            
            // Web Page Heading
            client.println("<body>");
            client.println("<h1>ESP32 Web Server</h1>");
            client.println("<p>GPIO 26 - State " + ledPinState + "</p>");
            if (ledPinState=="off") {
              client.println("<p><a href=\"/ledPin/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/ledPin/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");
            client.println();
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
