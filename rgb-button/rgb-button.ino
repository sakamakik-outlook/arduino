#define BUTTON_PIN_GREEN 2
#define LED_PIN_GREEN 11


void setup() {
  pinMode(BUTTON_PIN_GREEN, INPUT_PULLUP);
  pinMode(LED_PIN_GREEN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int greenButtonState = digitalRead(BUTTON_PIN_GREEN);

  if(greenButtonState == 0) {
    digitalWrite(LED_PIN_GREEN, HIGH);
    Serial.print("GREEN");
  } else {
    digitalWrite(LED_PIN_GREEN, LOW);
  }
}