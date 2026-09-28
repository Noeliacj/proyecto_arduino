#include "FastLED.h"

#define TRIG_PIN 13  
#define ECHO_PIN 12 
#define PIN_ITR20001_LEFT   A2
#define PIN_ITR20001_MIDDLE A1
#define PIN_ITR20001_RIGHT  A0
#define PIN_RBGLED 4
#define NUM_LEDS 1
#define PIN_Motor_STBY 3  // Enable/Disable motor control. HIGH/LOW
#define PIN_Motor_AIN_1 7  // Motor A (right): Digital HIGH: Forward, LOW: Backward 
#define PIN_Motor_PWMA 5  // Motor A (right): Analog [0-255] speed
#define PIN_Motor_BIN_1 8  // Motor B (left): Digital HIGH: Forward, LOW: Backward 
#define PIN_Motor_PWMB 6  // Motor B (left): Analog [0-255] speed
#define VEL_SON 34000

int r, g, b;
CRGB leds[NUM_LEDS];

int left, middle, right;
int state = 3, last_state;
bool end_lap = false;
bool lost_line_once = true;
bool lost = false;

void follow_line() {
  // Infrarrojo -- > 900 detecta linea negra
  left = analogRead(PIN_ITR20001_LEFT);
  middle = analogRead(PIN_ITR20001_MIDDLE);
  right = analogRead(PIN_ITR20001_RIGHT);

  digitalWrite(PIN_Motor_AIN_1, HIGH);
  digitalWrite(PIN_Motor_BIN_1, HIGH);

  if (left > 300) { //900 cinta profe
    state = 1;
  }
  else if (right > 300) { //900 cinta profe
    state = 2;
  }
  else if (middle > 300) { //900 cinta profe
    state = 3;
  }
  else {
    state = 4;
  }

  switch (state) {
    Serial.print("State: ");
    Serial.println(state);
    Serial.print(" Last state: ");
    Serial.println(last_state);
    case 1: //LEFT DETECT
      if (lost) {
        Serial.print("F");
        lost = false;
      }
      last_state = 1;
      lost_line_once = true;
      analogWrite(PIN_Motor_PWMA, 140); // 60
      analogWrite(PIN_Motor_PWMB, 20);
      // LED verde
      r=0,g=255,b=0;
      FastLED.showColor(Color(r, g, b));
      break;

    case 2: //RIGHT DETECT
      if (lost) {
        Serial.print("F");
        lost = false;
      }
      last_state = 2;
      lost_line_once = true;
      analogWrite(PIN_Motor_PWMA, 20);
      analogWrite(PIN_Motor_PWMB, 140); //60
      // LED verde
      r=0,g=255,b=0;
      FastLED.showColor(Color(r, g, b));
      break;

    case 3: //MIDDLE DETECT
      if (lost) {
        Serial.print("F");
        lost = false;
      }
      last_state = 3;
      lost_line_once = true;
      analogWrite(PIN_Motor_PWMA, 160); //80
      analogWrite(PIN_Motor_PWMB, 160); //80
      // LED verde
      r=0,g=255,b=0;
      FastLED.showColor(Color(r, g, b));
      break;
  
    case 4: //MEPERDI
      if (lost_line_once) {
        Serial.print("L");
        lost_line_once = false;
      }
      // LED rojo
      r=255,g=0,b=0;
      FastLED.showColor(Color(r, g, b));
      state = last_state;
      lost = true;
      break;
  }
}

// Funcion LED
uint32_t Color(uint8_t r, uint8_t g, uint8_t b) {
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

void iniciar_trigger() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2); 
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
}

void ultrasonido() {
  iniciar_trigger();
  long tiempo = pulseIn(ECHO_PIN, HIGH); //timepo que demora en llegar el eco
  long distance = tiempo * 0.000001 * VEL_SON / 2; //distancia en centimetros

  if (distance <= 10 and distance != 0){
    r=0,g=0,b=255;
    FastLED.showColor(Color(r, g, b));
    digitalWrite(PIN_Motor_STBY, LOW);
    iniciar_trigger();
    tiempo = pulseIn(ECHO_PIN, HIGH);
    distance = tiempo * 0.000001 * VEL_SON / 2;
    end_lap = true;
    Serial.print("O");
    Serial.print(distance);
    Serial.print("-");
    Serial.print("E");
  }
}

void ready_to_move() {
  // Cambia el color del LED
  int r = 255, g = 255, b = 255;
  FastLED.showColor(CRGB(r, g, b));

  String buffer = ""; // Buffer para almacenar los caracteres leídos
  char read;

  while (buffer != "G") {
    if (Serial.available() > 0) { // Verifica si hay datos disponibles
      read = Serial.read(); // Lee el siguiente carácter
      if (read != '\n' && read != '\r') { // Ignora caracteres de nueva línea
        buffer += read; // Añade el carácter al buffer
      }
      Serial.println(buffer); // Imprime el buffer para depuración
    }
  }
  Serial.print("S");
}


void setup() {
  Serial.begin(9600);

  // Motores
  pinMode(PIN_Motor_AIN_1, OUTPUT);
  pinMode(PIN_Motor_BIN_1, OUTPUT);
  pinMode(PIN_Motor_STBY, OUTPUT);
  digitalWrite(PIN_Motor_STBY, HIGH);

  // LED
  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);
  FastLED.setBrightness(20);  // Subir a 100 para examen

  // Ultrasonido
  pinMode(TRIG_PIN, OUTPUT); //pin como salida
  pinMode(ECHO_PIN, INPUT);  //pin como entrada
  
  ready_to_move();
}

void loop() {
  if (!end_lap) {
    follow_line();
    ultrasonido();
  }
}

