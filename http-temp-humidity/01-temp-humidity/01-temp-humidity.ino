#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setup() {
  Wire.begin(0,1); // SDA -> 0, SCL -> 1
  Serial.begin(9600);
  Serial.println("Adafruit AHT10/AHT20 demo!");

  while (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    delay(10);
  }
  Serial.println("AHT10 or AHT20 found");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println("% rH");
  delay(1000);
}