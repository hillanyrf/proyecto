/*
Universidad Simón Bolívar
Ingeniería Electrónica
Estudiante: Hillany Rodríguez
Carnet: 14-10937
*/

#include <ESP32Servo.h>
#include <Wire.h>

Servo miServo;
int pinPulsadorH = 12;  // Cambiar al pin correspondiente
int pinPulsadorA = 13;
int pinswPulsador = 34;
int pinswGiroscopio = 33;
int estadoPulsadorH = 0;
int estadoPulsadorA = 0;
int angulo = 0;

const int MPU_ADDR = 0x68; // Dirección I2C del MPU-6050
int16_t accelerometer_x, accelerometer_y, accelerometer_z; // Variables para datos crudos del acelerómetro
int16_t gyro_x, gyro_y, gyro_z; // Variables para datos crudos del giroscopio
int16_t temperature; // Variable para datos de temperatura
char tmp_str[7]; // Variable temporal usada en la función de conversión

// Función para convertir int16 a string
char* convert_int16_to_str(int16_t i) {
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}

void setup() {
  miServo.attach(19);  // Cambiar al pin correspondiente
  pinMode(pinPulsadorH, INPUT_PULLDOWN);
  pinMode(pinPulsadorA, INPUT_PULLDOWN);
  pinMode(pinswPulsador, INPUT_PULLDOWN);
  pinMode(pinswGiroscopio, INPUT_PULLDOWN);
  Serial.begin(115200); // Cambiado a 115200 para una mejor velocidad de comunicación
  Wire.begin(); // Inicializa la comunicación I2C
  Wire.beginTransmission(MPU_ADDR); 
  Wire.write(0x6B); // Registro PWR_MGMT_1
  Wire.write(0); // Configura a cero para despertar el MPU-6050
  Wire.endTransmission(true);
}

void loop() {

  if(digitalRead(pinswPulsador) == HIGH){
    Serial.println("pulsador");
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

  if(digitalRead(pinswGiroscopio) == HIGH){
  //Serial.println("giroscopio");
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Comienza con el registro 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false); 
  Wire.requestFrom(MPU_ADDR, 14); // Solicita un total de 14 registros
  
  // Leer datos
  accelerometer_x = Wire.read() << 8 | Wire.read();
  accelerometer_y = Wire.read() << 8 | Wire.read();
  accelerometer_z = Wire.read() << 8 | Wire.read();
  temperature = Wire.read() << 8 | Wire.read();
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();
  
  // Imprimir datos
  Serial.print(convert_int16_to_str(accelerometer_x));Serial.print(","); 
  Serial.print(convert_int16_to_str(accelerometer_y));Serial.print(","); 
  Serial.print(convert_int16_to_str(accelerometer_z));
  
  //Serial.print(convert_int16_to_str(gyro_z)); Serial.print(",");
  //Serial.print(convert_int16_to_str(gyro_y)); Serial.print(",");
  //Serial.print(convert_int16_to_str(gyro_x));

  //Serial.print(accelerometer_x);Serial.print(","); 
  //Serial.print(accelerometer_y);Serial.print(","); 
  //Serial.print(accelerometer_z);

  Serial.println();

  if(accelerometer_x > -15900 && accelerometer_x < -1500 && accelerometer_z > -1000 && accelerometer_z < 17600){
    int angulop = map(accelerometer_z, 17600, -500, 0, 180); // Escalar el valor leido a un rango de 0 a 180
    miServo.write(angulop); // Enviar el valor escalado al servo
    //Serial.print("angulo: ");
    //Serial.println(angulop);
  }
  
  delay(100);
  }

  if(digitalRead(pinswPulsador) == LOW && digitalRead(pinswGiroscopio) == LOW ){
    miServo.write(0);
    delay(15);
    Serial.println("todo apagado");
  }
}