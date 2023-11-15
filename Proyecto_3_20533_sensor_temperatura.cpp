#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

const int ledPin = 23;
const int numLeds = 16;

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(numLeds, ledPin, NEO_GRB + NEO_KHZ800);

float temperaturaCelsius = 0.0;
bool botonesActivados[2] = {false};  // Array para manejar el estado de los botones

void efectoBoton(int boton, int colorR, int colorG, int colorB);
void iluminarTodos(int colorR, int colorG, int colorB);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  ledStrip.begin();
  ledStrip.show();  // Inicializa los LEDs

  Wire.begin();  // Inicializa la comunicación I2C

  // Establecer todos los LEDs en blanco cálido al inicio
  iluminarTodos(255, 235, 180);  // Color blanco cálido (R, G, B)
  delay(2000);  // Mantener los LEDs encendidos durante 3 segundos al inicio
  ledStrip.clear();
  ledStrip.show();
}

void loop() {
  while (Serial2.available() > 0) {
    char comando = Serial2.read();

    if (comando == 't') {
      // Leer temperatura desde el sensor LM75 a través de I2C
      Wire.beginTransmission(0x48);
      Wire.write(0x00);
      Wire.endTransmission();
      Wire.requestFrom(0x48, 2);

      if (Wire.available() >= 2) {
        int byte1 = Wire.read();
        int byte2 = Wire.read();
        int temperaturaRaw = (byte1 << 8) | byte2;
        temperaturaCelsius = temperaturaRaw / 256.0;

        Serial.print("La temperatura actual es: ");
        Serial2.println(temperaturaCelsius);
        Serial.println(temperaturaCelsius);
      }
    }

    // Controlar los efectos de los botones
    for (int i = 0; i < 2; ++i) {
      if (comando == 'X' + i && !botonesActivados[i]) {
        botonesActivados[i] = true;
        efectoBoton(i, 100 * (i == 0), 200, 50 * (i == 1)); // Verde para 'X', Amarillo para 'Y'
      }
    }
  }
}

void efectoBoton(int boton, int colorR, int colorG, int colorB) {
  // Efecto de destello
  for (int i = 0; i < 256; ++i) {
    int intensidad = abs(128 - i);

    ledStrip.setPixelColor(boton, ledStrip.Color((colorR * intensidad) / 128, (colorG * intensidad) / 128, (colorB * intensidad) / 128));
    ledStrip.show();
    delay(10);
  }

  for (int i = 255; i >= 0; --i) {
    int intensidad = abs(128 - i);

    ledStrip.setPixelColor(boton, ledStrip.Color((colorR * intensidad) / 128, (colorG * intensidad) / 128, (colorB * intensidad) / 128));
    ledStrip.show();
    delay(10);
  }

  // Apagar el LED después de la rutina
  ledStrip.setPixelColor(boton, ledStrip.Color(0, 0, 0));
  ledStrip.show();

  // Resetear el estado del botón después de la rutina
  botonesActivados[boton] = false;
}

void iluminarTodos(int colorR, int colorG, int colorB) {
  // Ilumina todos los LEDs con el color especificado
  for (int i = 0; i < numLeds; ++i) {
    ledStrip.setPixelColor(i, ledStrip.Color(colorR, colorG, colorB));
  }
  ledStrip.show();
}