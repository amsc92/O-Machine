

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "mux.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_LINES 8
String oledBuffer[MAX_LINES];

// ---- MOSTRAR VARIAS LINEAS CON TIEMPO ----
void oledPrintLine(const String *lineas, int numLineas, unsigned long displayTimePerLine = 1500)
{
  for (int i = 0; i < numLineas; i++)
  {
    // Mover buffer
    for (int j = 0; j < MAX_LINES - 1; j++)
      oledBuffer[j] = oledBuffer[j + 1];

    oledBuffer[MAX_LINES - 1] = lineas[i];

    // Dibujar buffer
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    for (int j = 0; j < MAX_LINES; j++)
      display.println(oledBuffer[j]);

    display.display();

    // Mantener línea en pantalla el tiempo indicado, sin bloquear MUX
    unsigned long start = millis();
    while (millis() - start < displayTimePerLine)
    {
      mux_sequence_runner(); // mantener secuencia de bombas activa
      delay(5);
    }
  }
}

// ---- INICIALIZACION OLED ----
void set_disA()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 error"));
    for (;;)
      ;
  }
  display.clearDisplay();
  display.display();
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(255);
}
void showTouchedMessage()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("TOUCHED!");
  display.display();
}
void displayOn()
{
  display.display(); // refrescar y encender
}

void displayOff()
{
  display.clearDisplay(); // borrar
  display.display();      // se queda negro
}