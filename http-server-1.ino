#include <WiFi.h>

const char* ssid = "ssid";
const char* password = "pass";

WiFiServer server(80);

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

int count = 0;
void loop(){
  WiFiClient client = server.available();   


  if (client) {           

    client.println("<!DOCTYPE html><html>");
    client.println("<head></head>");
    
    // Web Page Heading
    client.println("<body>");
    client.println("<h1>Hello World! </h1>");
    client.println(millis());
    client.println("</body></html>");
    client.println();

    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
