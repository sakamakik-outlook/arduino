
#include <Adafruit_AHTX0.h>
#include <WiFi.h>

const char* ssid = "Helix";
const char* password = "Sakamakik7!";

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
    <html>
    <head>
      <meta http-equiv="Content-Type" content="text/html; charset=windows-1252">
      <script type="text/javascript" src="https://cdn.fusioncharts.com/fusioncharts/latest/fusioncharts.js"></script>
      <script type="text/javascript" src="https://cdn.fusioncharts.com/fusioncharts/latest/themes/fusioncharts.theme.fusion.js"></script>
      <script type="text/javascript">
        FusionCharts.ready(function(){
)html";

const char* html_end = R"html(
			var tempObj = new FusionCharts({
			type: 'thermometer',
			renderAt: 'temperature-container',
			dataFormat: 'json',
			dataSource: {
				"chart": {
					"lowerLimit": "0",
					"upperLimit": "50",
					"numberSuffix": " C",
					"thmFillColor": "#008e00",
					"theme": "fusion"
				},
				"value": temperature,
			},
			}
		);

		var humidityObj = new FusionCharts({
		type: 'angulargauge',
		renderAt: 'humidity-container',
		dataFormat: 'json',
		dataSource: {
			"chart": {
				"lowerLimit": "0",
				"upperLimit": "100",
				"theme": "fusion"
			},
			"colorRange": {
				"color": [{
					"minValue": "0",
					"maxValue": "20",
					"code": "#f8bd19"
				}, {
					"minValue": "20",
					"maxValue": "75",
					"code": "#6baa01"
				}, {
					"minValue": "75",
					"maxValue": "100",
					"code": "#e44a00"
				}]
			},
			"dials": {
				"dial": [{
					"value": humidity
				}]
			}
		}
		}
		);
		tempObj.render();
		humidityObj.render();
		});
	</script>
	
</head>
<body>
  <div id="temperature-container">Temperature</div>
  <div id="humidity-container">Humidity</div>
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

            client.print("temperature=");
            client.print(temp.temperature);
            client.println(";");

            client.print("humidity=");
            client.print(humidity.relative_humidity);
            client.println(";");


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
