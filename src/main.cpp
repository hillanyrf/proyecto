/*
Universidad Simón Bolívar
Ingeniería Electrónica
Estudiante: Hillany Rodríguez
Carnet: 14-10937
*/

#define CUSTOM_SETTINGS
#define INCLUDE_MOTORCONTROL_MODULE
#include <DabbleESP32.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <WiFi.h>

Servo miServo;
int pinPulsadorH = 12;  // Cambiar al pin correspondiente
int pinPulsadorA = 13;
int pinswPulsador = 34;
int pinswGiroscopio = 21;
int pinswBluetooth = 33;
int pinswWifi = 17;
int estadoPulsadorH = 0;
int estadoPulsadorA = 0;
int angulo = 0;
int bluenc = 0;
int contador =0;

int contador1 =0;
int contador2 =0;

uint8_t pinServo1 = 19;

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

//wifi
const char* ssid = "Soulstealer";          // Reemplaza con tu SSID.
const char* password = "Julio26O57776";  // Reemplaza con tu contraseña.

WiFiServer server(80);  // Crea un servidor en el puerto 80.

void setup() {
  miServo.attach(19);  // Cambiar al pin correspondiente
  pinMode(pinPulsadorH, INPUT_PULLDOWN);
  pinMode(pinPulsadorA, INPUT_PULLDOWN);
  pinMode(pinswPulsador, INPUT_PULLDOWN);
  pinMode(pinswGiroscopio, INPUT_PULLDOWN);
  pinMode(pinswBluetooth, INPUT_PULLDOWN);
  pinMode(pinswWifi, INPUT_PULLDOWN);
  Serial.begin(115200); // Cambiado a 115200 para una mejor velocidad de comunicación
  
  //Giroscopio
  Wire.begin(15,4); // Inicializa la comunicación I2C
  Wire.beginTransmission(MPU_ADDR); 
  Wire.write(0x6B); // Registro PWR_MGMT_1
  Wire.write(0); // Configura a cero para despertar el MPU-6050
  Wire.endTransmission(true);

  //wifi
  //pinMode(pinServo1, OUTPUT);
    
  miServo.write(0);
}

void loop() {
    //Pulsador
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
  }//fin pulsador

//Giroscopio
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
  }//fin giroscopio

//bluetooth
  if(digitalRead(pinswBluetooth) == HIGH && contador == 0){
    
    Dabble.begin("MyEsp32");
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    contador = 1;
    Serial.println("Begin");
  }

  if(digitalRead(pinswBluetooth) == HIGH){
    Serial.println("Bluetooth dabble");
    Dabble.processInput(); //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.              //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
    Controls.runServo1(pinServo1);
    bluenc=1;
  }

  //detener el bluetooth
  if(digitalRead(pinswBluetooth) == LOW && bluenc == 1){
    Serial.println("Bluetooth stop");
    esp32ble.stop();
    bluenc = 0;
    contador = 0;
    //freeMemoryAllocated(); revisar
  }//fin bluetooth

    //wifi
    //comenzar a conectar
    if(digitalRead(pinswWifi) == HIGH && contador1 == 0 && digitalRead(pinswBluetooth) == LOW && digitalRead(pinswPulsador) == LOW && digitalRead(pinswGiroscopio) == LOW){
    WiFi.begin(ssid, password);  // Conéctate a la red Wi-Fi.

    // Espera hasta que esté conectado
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConectado a la red Wi-Fi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());  // Imprime la IP local asignada al ESP

    server.begin();  // Comienza el servidor
    contador1=1; //se encendio
}
//Mantener wifi

  if(contador1 == 1 && digitalRead(pinswWifi) == HIGH && digitalRead(pinswBluetooth) == LOW && digitalRead(pinswPulsador) == LOW && digitalRead(pinswGiroscopio) == LOW){
    
    WiFiClient client = server.available();  // Verifica si hay un cliente conectado.

    if (client) {
        Serial.println("Nuevo cliente conectado");
        String request = "";      // Para almacenar la solicitud completa.

        while (client.connected() || client.available()) {
            if (client.available()) {
            char c = client.read();  // Lee un carácter del cliente.
            request += c;           // Agrega el carácter a la solicitud completa.

            Serial.write(c);  // Imprime el carácter recibido en la consola.

            // Cuando se encuentra una nueva línea, verifica si es una solicitud GET.
            if (c == '\n') {
                if (request.indexOf("GET /") != -1) {
                    // Envía una respuesta HTTP.
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type: text/html");
                    client.println("Connection: close");
                    client.println();

                    // Envía la página HTML.
                    client.println("<!DOCTYPE html>");
                    client.println("<html>");
                    client.println("<body>");
                    client.println("<h1>Enviar un numero</h1>");
                    client.println("<form action=\"/\" method=\"GET\">");
                    client.println("Numero: <input type=\"number\" name=\"number\">");
                    client.println("<input type=\"submit\" value=\"Enviar\">");
                    client.println("</form>");

                    // Si se recibe una solicitud GET con un número, imprímelo en la consola.
                    int index = request.indexOf("GET /?number=");
                    if (index != -1) {
                        int startIndex = index + 13;  // Longitud de "GET /?number=" es 13.
                        int endIndex = request.indexOf(' ', startIndex);
                        if (endIndex == -1) endIndex = request.length();
                            String number = request.substring(startIndex, endIndex);
                            Serial.print("Numero recibido: ");
                            Serial.println(number);
                            delay(1000);
                            int numberInt = number.toInt();
                            miServo.write(numberInt);
                        }

                        client.println("</body>");
                        client.println("</html>");
                        break;
                    } else {
                        request = "";  // Reinicia la solicitud actual si no es una solicitud GET.
                    }
                }
            }
        }

        delay(1);
        client.stop();
        Serial.println("Cliente desconectado");
    }
    contador2=1; //entro en mantener
  }//fin wifi
  
//cerrar wifi
  if(digitalRead(pinswWifi) == LOW && contador2==1){
    WiFi.disconnect();
    contador1 = 0;
    contador2 = 0;
    Serial.println("Wifi desconectado.");
    }

  //todo apagado
  if(digitalRead(pinswPulsador) == LOW && digitalRead(pinswGiroscopio) == LOW && digitalRead(pinswBluetooth) == LOW && digitalRead(pinswWifi) == LOW ){
    miServo.write(0);
    delay(15);
    Serial.println("todo apagado");
  }
}