#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0

void setup() {
  Serial.begin(9600);
}

void loop() {
  int left, middle, right;

  left = analogRead(PIN_ITR20001_LEFT);
  middle = analogRead(PIN_ITR20001_MIDDLE);
  right = analogRead(PIN_ITR20001_RIGHT);

  // > 900 detecta linea negra
  

  Serial.print("LEFT: ");
  Serial.println(left);
  Serial.print("MIDDLE: ");
  Serial.println(middle);
  Serial.print("RIGHT: ");
  Serial.println(right);
  delay(2000);
}
