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
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Keypad.h>

Servo miServo;
int pinPulsadorH = 12;  // Cambiar al pin correspondiente
int pinPulsadorA = 13;
const int sensorPin = 34;
int mostrarangulo = 0;

/*
//switchs
int pinswPulsador = 32;
int pinswGiroscopio = 35;
int pinswBluetooth = 33;
int pinswWifi = 25;
int pinswSensor = 26;
*/

int estadoPulsadorH = 0;
int estadoPulsadorA = 0;
int angulo = 0;
int bluenc = 0;
int contador =0;

int contador1 =0;
int contador2 =0;

//uint8_t pinServo1 = 23;

//Sensor muscular
const int numReadings = 10; // Número de lecturas para promediar
float readings[numReadings]; // Arreglo para almacenar las lecturas
int indice = 0; // Índice para la siguiente lectura
float total = 0; // Suma total de las lecturas
float average = 0; // Promedio
int cont3 = 0;

//MPU
const int MPU_ADDR = 0x68; // Dirección I2C del MPU-6050
int16_t accelerometer_x; // Variables para datos crudos del acelerómetro
float filtered_x = 0; // Variable para la lectura filtrada
const float alpha = 0.1; // Factor de suavizado

//wifi
const char* ssid = "Soulstealer";          // Reemplaza con tu SSID.
const char* password = "Julio26O57776";  // Reemplaza con tu contraseña.

WiFiServer server(80);  // Crea un servidor en el puerto 80.

//Pantalla
//string textoestado
char* textoestado;
// Definir el tamaño de la pantalla
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Definir la dirección I2C (la dirección predeterminada suele ser 0x3C)
#define OLED_RESET -1  // No es necesario para este modelo específico

// Crear una instancia del objeto SSD1306
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);

// Variable que se mostrará en la pantalla
int grado = 0;  // Inicia en 0
int estado = 0; // Inicializar en 0 para indicar que no hay selección

// Configuración del teclado
const byte ROWS = 3; 
const byte COLS = 2; 

char keys[ROWS][COLS] = {
  {'2', '3'},
  {'5', '6'},
  {'8', '9'}
};

byte rowPins[ROWS] = {19, 18, 5}; // Conectar filas a los pines 27, 26 y 25
byte colPins[COLS] = {17, 16}; // Conectar columnas a los pines 12 y 33

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);


void actualizarDisplay() {
  display.clearDisplay();
  
  // Mostrar el número
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 30);
  display.print(mostrarangulo);
  
  // Mostrar la opción seleccionada
  display.setTextSize(1);
  display.setCursor(0, 0);
  if (estado > 0) {
    display.print(F("Opcion "));
    display.print(textoestado);
    if(estado == 6){
      display.print("\nIP: " ); 
      display.print(WiFi.localIP());
    }
  } else {
    display.print(F("No se ha seleccionado ninguna opcion"));
  }

  display.display(); // Renderizar la pantalla
}


void setup() {
  miServo.attach(23);  // Cambiar al pin correspondiente
  pinMode(pinPulsadorH, INPUT_PULLDOWN);
  pinMode(pinPulsadorA, INPUT_PULLDOWN);

  /*
  pinMode(pinswPulsador, INPUT_PULLDOWN);
  pinMode(pinswGiroscopio, INPUT_PULLDOWN);
  pinMode(pinswBluetooth, INPUT_PULLDOWN);
  pinMode(pinswWifi, INPUT_PULLDOWN);
  pinMode(pinswSensor, INPUT_PULLDOWN);
  */

  Serial.begin(115200); // Cambiado a 115200 para una mejor velocidad de comunicación
  
  //Giroscopio
  Wire.begin(15,4); // Inicializa la comunicación I2C
  Wire.beginTransmission(MPU_ADDR); 
  Wire.write(0x6B); // Registro PWR_MGMT_1
  Wire.write(0); // Configura a cero para despertar el MPU-6050
  Wire.endTransmission(true);

  miServo.write(0);

  //Pantalla OLED
  // Inicializar I2C con los pines personalizados
  Wire1.begin(21, 22);
  
  // Inicializar la pantalla OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("No se encuentra la pantalla SSD1306"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("Presione 2,3,5,6,8 o 9:"));
  display.display();

  //teclado

}

void loop() {

  char key = keypad.getKey(); // Leer la tecla presionada

if (key) { // Si se presiona una tecla
    if (key >= '2' && key <= '9') {
      estado = key - '0'; // Convertir carácter a número
    } else {
      estado = 0;
      Serial.println(F("Número invalido. Ingrese 2, 3, 5, 6, 8 o 9."));
    }
  }

  //Pulsador
  if(estado == 2){
    textoestado = "Pulsador";
    Serial.println("pulsador");
    estadoPulsadorH = digitalRead(pinPulsadorH);
    estadoPulsadorA = digitalRead(pinPulsadorA);
    
    if (estadoPulsadorH == HIGH && estadoPulsadorA == LOW) {
      if (angulo < 180) {
        angulo++;
      } else {
          angulo = 180;
        }
      mostrarangulo = angulo;
      miServo.write(angulo);
      delay(5);  // Ajustar la velocidad del movimiento del servo según sea necesario
    }

    if (estadoPulsadorH == LOW && estadoPulsadorA == HIGH) {
      if (angulo > 0) {
        angulo--;
      } else {
          angulo = 0;
        }
        
      mostrarangulo = angulo;
      miServo.write(angulo);
      delay(5); 
    }
  }//fin pulsador

  //Giroscopio
  if(estado == 3){
  textoestado = "Giroscopio";
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);
  
  // Leer datos
  accelerometer_x = Wire.read() << 8 | Wire.read();
  
  // Aplicar filtro exponencial
  filtered_x = alpha * accelerometer_x + (1 - alpha) * filtered_x;

  if(filtered_x > 10000 && filtered_x < 25000) {
    int angulop = map(filtered_x, 10000, 17000, 0, 180);
    mostrarangulo = angulop;
    miServo.write(angulop);
  }
  
  delay(20);
  }//fin giroscopio

  //Bluetooth
  if(estado == 5 && contador == 0){
    
    Dabble.begin("MyEsp32");
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    contador = 1;
    Serial.println("Begin");
  }

  if(estado == 5){
    textoestado = "Bluetooth";
    Serial.println("Bluetooth dabble");
    Dabble.processInput(); //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.              //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
    Controls.runServo1(23);
    bluenc=1;
    mostrarangulo = Controls.angleServo1;
  }

  //detener el bluetooth
  if(estado != 5 && bluenc == 1){
    Serial.println("Bluetooth stop");
    esp32ble.stop();
    bluenc = 0;
    contador = 0;
    miServo.attach(23);
    //freeMemoryAllocated(); revisar
  }//fin bluetooth

    //Wifi
    //comenzar a conectar
    if(estado == 6 && contador1 == 0){
      textoestado = "Wi-Fi";
    WiFi.begin(ssid, password);  // Conéctate a la red Wi-Fi

    // Espera hasta que esté conectado
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConectado a la red Wi-Fi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());  // Imprime la IP local asignada al ESP
    //falta imprimir esto en la pantalla

    server.begin();  // Comienza el servidor
    contador1=1; //se encendio
  } 
  //Mantener wifi

  if(contador1 == 1 && estado == 6){
    
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
                            mostrarangulo = numberInt;
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
  if(estado != 6 && contador2==1){
    WiFi.disconnect();
    contador1 = 0;
    contador2 = 0;
    Serial.println("Wifi desconectado.");
    }

  //Sensor

  if(estado == 8){
    textoestado = "Sensor";
    if(estado == 8 && cont3==0){
      //correr solo una vez cada vez que se encienda el interruptor
      // Inicializar el arreglo de lecturas
      for (int i = 0; i < numReadings; i++) {
      readings[i] = 0;
      }
      cont3=1;
    }

    // Restar la lectura más antigua de la suma total
    total -= readings[indice];
  
    // Leer el nuevo valor del sensor
    readings[indice] = analogRead(sensorPin);
  
    // Sumar la nueva lectura al total
    total += readings[indice];
  
    // Avanzar al siguiente índice
    indice = (indice + 1) % numReadings; // Volver al inicio si se llega al final
  
    // Calcular el promedio
    average = total / numReadings;

    Serial.print("Promedio de las últimas 15 lecturas: ");
    Serial.println(average);

    if(average < 600){
    miServo.write(90);
    mostrarangulo = 90;
    }

    if(average > 600){
      miServo.write(0);
      mostrarangulo = 0;
    }

    delay(100);
  }

  //todo apagado
  if(estado == 9){
    textoestado = "Todo apagado";
    miServo.write(0);
    delay(15);
    Serial.println("todo apagado");
    cont3=0;
    mostrarangulo = 0;
  }

  actualizarDisplay(); // Actualizar la pantalla
}