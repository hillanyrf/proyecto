/*
Universidad Simón Bolívar
Ingeniería Electrónica
Estudiante: Hillany Rodríguez
Carnet: 14-10937
*/

#include <ESP32Servo.h>
//#include <DabbleESP32.h>
//#include <BluetoothSerial.h>
//#include <WiFi.h>

Servo miServo;
int pinPulsadorH = 32;  
int pinPulsadorA = 33;
int pinswPulsador = 12;
int estadoPulsadorH = 0;
int estadoPulsadorA = 0;
int angulo = 0;

void setup() {
  miServo.attach(22);  
  pinMode(pinPulsadorH, INPUT_PULLDOWN);
  pinMode(pinPulsadorA, INPUT_PULLDOWN);
  pinMode(pinswPulsador, INPUT_PULLDOWN);
}

void loop() {

  if(digitalRead(pinswPulsador) == HIGH){
    estadoPulsadorH = digitalRead(pinPulsadorH);
    estadoPulsadorA = digitalRead(pinPulsadorA);
    
    if (estadoPulsadorH == HIGH && estadoPulsadorA == LOW) {
      if (angulo < 180) {
        angulo++;
      } else {
          angulo = 180;
        }
      miServo.write(angulo);
      delay(15);  // Ajustar la velocidad del movimiento del servo según sea necesario
    }

    if (estadoPulsadorH == LOW && estadoPulsadorA == HIGH) {
      if (angulo > 0) {
        angulo--;
      } else {
          angulo = 0;
        }
      miServo.write(angulo);
      delay(15); 
    }

  }

  if(digitalRead(pinswPulsador) == LOW){
    miServo.write(0);
    delay(15);
  }

}