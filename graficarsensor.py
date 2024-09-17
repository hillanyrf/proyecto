import time
import esp32

# Configuración del pin de entrada analógica
sensor_pin = analogio.AnalogIn(board.D27)

while True:
    # Leer el valor del sensor
    sensor_value = sensor_pin.value

    # Convertir el valor a milivoltios
    millivolt = (sensor_value / 65535) * 5 * 1000

    # Imprimir los valores
    print("Sensor Value:", sensor_value)
    print("Voltage:", millivolt, "mV")
    print()

    # Esperar 1 milisegundo
    time.sleep(0.001)