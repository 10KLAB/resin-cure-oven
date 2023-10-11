#include "screen_control.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <SPI.h>
#include <Wire.h>

#define i2c_Address 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display =
    Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
namespace _10klab {
namespace screen {

void ShowLineOne(String text) {

  display.clearDisplay();
  display.setTextSize(2);
  int center_text1 = (display.width() / 2) - ((11.5 * text.length()) / 2);
  display.setCursor(center_text1, 5);
  display.println(text);
  display.display();
}
void ShowLineTwo(String text) {

  // display.clearDisplay();
  display.setTextSize(2);
  int center_text1 = (display.width() / 2) - ((11.5 * text.length()) / 2);
  display.setCursor(center_text1, 40);
  display.println(text);
  display.display();
}

void SetUpScreen() {
  // delay(3000);
  while(!display.begin(i2c_Address, true)){
  display.begin(i2c_Address, true);
  delay(500);
  }
  display.setTextWrap(false);
  display.setTextColor(SH110X_WHITE);
  display.clearDisplay();
  ShowLineOne("10KLAB");
  ShowLineTwo("Cure oven");
  delay(3000);
}

void drawAxes() {
  // y axis labels
  display.setCursor(0, 0);
  display.print(F("85C"));
  display.setCursor(0, 11);
  display.print(F("75C"));
  display.setCursor(0, 22);
  display.print(F("55C"));
  display.setCursor(0, 32);
  display.print(F("35C"));
  display.setCursor(0, 43);
  display.print(F("25C"));

  display.drawLine(20, 0, 25, 0, SH110X_WHITE);
  display.drawLine(20, 11, 25, 11, SH110X_WHITE);
  display.drawLine(20, 22, 25, 22, SH110X_WHITE);
  display.drawLine(20, 32, 25, 32, SH110X_WHITE);
  display.drawLine(20, 43, 25, 43, SH110X_WHITE);
  // x & y axes
  display.drawLine(0, 53, 127, 53, SH110X_WHITE);
  display.drawLine(25, 53, 25, 0, SH110X_WHITE);
}

void drawGraph(float value, int setpoint) {

  static int x[128] = {9999};
  static int y[128] = {9999};
  static unsigned long current_time = 0;
  const int refresh_time = 500;

  if(millis() < current_time + refresh_time) {
    return;
  }

  display.setTextSize(1);

    display.clearDisplay();
    drawAxes();

  static float min_value = 0;
  static float max_value = 85;

    float graph_value = map(value, min_value, max_value, 53, 0);
    if(graph_value > max_value) {
      graph_value = max_value;
      }
    if (graph_value < min_value) {
      graph_value = min_value;
    }
    
    x[127] = graph_value;

    for (int i = 25; i <= 127; i++) {
      display.drawPixel(i, x[i], SH110X_WHITE); // dibuja punto a punto el contenido de x
      y[i - 1] = x[i]; // guarda la informacion desplazada una posicion
                       // temporalmente en y
    }

    display.setCursor(25, 57);
    display.print(value);
    display.print(F(" C    "));

    display.print(F("SP "));
    display.print(setpoint);

    display.display();

    for (int i = 0; i <= 127; i++) {
      x[i] = y[i]; // envia los datos desplazados de vuelta a la variable x
    }
    current_time = millis();



}

} // namespace screen
} // namespace _10klab