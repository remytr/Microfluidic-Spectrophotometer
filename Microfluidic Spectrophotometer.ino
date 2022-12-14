//#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AS7341.h>

Adafruit_AS7341 as7341;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//pins
#define STATUS_PIN 10
#define POWER_PIN 5
#define LED_POWER 9
#define BBUT_PIN 7
#define ABUT_PIN 8

volatile bool pause = true;
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
int STATE = 1;
volatile int menu = 0;
volatile bool Selected = false;
volatile bool repeat = false;
bool BLANK = false;
uint16_t BLreadings[12];
float BLcounts[12];
  uint16_t readings[12];
  float counts[12];
volatile unsigned long millis_BISR = 0;
volatile unsigned long millis_AISR = 0;
volatile unsigned long last_millis_BISR = 0;
volatile unsigned long last_millis_AISR = 0;


const unsigned char myBitmap [] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0x00, 0x60, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0x00, 0x60, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0x00, 0x60, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0x00, 0x60, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xf1, 0xff, 0xfe, 0xfe, 0x3f, 0x7f, 0xfb, 0xfb, 0xff, 0xc7, 0x83, 0x87, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xfc, 0x7c, 0x06, 0x3c, 0x71, 0xf1, 0xcf, 0x03, 0x01, 0x81, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xfc, 0x3c, 0x02, 0x3c, 0x60, 0xf1, 0xce, 0x1f, 0x03, 0x80, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xf8, 0x3c, 0xe2, 0x3c, 0xe0, 0xf0, 0xce, 0x3f, 0x1f, 0x9c, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xf8, 0x3c, 0xf1, 0x18, 0xe0, 0xf0, 0xcc, 0x7f, 0x1f, 0x9e, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xf9, 0x1c, 0xf1, 0x18, 0xe0, 0xf0, 0x4c, 0x7f, 0x07, 0x9e, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xf1, 0x1c, 0xf1, 0x99, 0xc4, 0x70, 0x0c, 0x7f, 0x03, 0x9e, 0x3f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xf1, 0x1c, 0xf1, 0x81, 0xc4, 0x72, 0x0c, 0x7f, 0x0f, 0x9e, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xf0, 0x0c, 0xf1, 0x81, 0xc0, 0x73, 0x0c, 0x7f, 0x1f, 0x9e, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xe0, 0x0c, 0xf3, 0xc3, 0x80, 0x33, 0x0c, 0x7f, 0x1f, 0x9c, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xe3, 0x8c, 0x63, 0xc3, 0x8e, 0x33, 0x8e, 0x3f, 0x1f, 0x88, 0x7f, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xe7, 0xc4, 0x07, 0xc3, 0x9f, 0x33, 0x8f, 0x03, 0x01, 0x80, 0xff, 0xff, 0xff, 0xff,
  0xf0, 0xff, 0xe7, 0xcc, 0x0f, 0xe7, 0x9f, 0x33, 0xcf, 0x83, 0x01, 0x81, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xe7, 0x9f, 0x9f, 0xe0, 0xe7, 0x1c, 0x07, 0x03, 0xf1, 0xf8, 0x18, 0x0f, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x9f, 0x0f, 0x80, 0xc6, 0x38, 0x06, 0x01, 0xe1, 0xf0, 0x18, 0x0f, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x9f, 0x0f, 0x8f, 0xc6, 0x38, 0xfe, 0x38, 0xe1, 0xf1, 0xf8, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x9e, 0x07, 0x1f, 0xc0, 0x78, 0xfe, 0x38, 0xc0, 0xe3, 0xf8, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xe3, 0x1e, 0x07, 0x1f, 0xc0, 0xf8, 0x3e, 0x38, 0xc0, 0xe3, 0xf8, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xe0, 0x1e, 0x47, 0x1f, 0xc0, 0xfc, 0x0e, 0x10, 0xcc, 0xe3, 0xf8, 0x1f, 0xff, 0x0f,
  0xff, 0xff, 0xe0, 0x1e, 0x67, 0x1f, 0xc0, 0xfe, 0x06, 0x01, 0x8c, 0xe3, 0xf8, 0x1f, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x9c, 0x03, 0x1f, 0xc0, 0x7f, 0xc2, 0x07, 0x80, 0x63, 0xf8, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x9c, 0x03, 0x1f, 0xc4, 0x3f, 0xe2, 0x3f, 0x80, 0x63, 0xf8, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x9c, 0x63, 0x1f, 0xc6, 0x3f, 0xe2, 0x3f, 0x0c, 0x63, 0xf8, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x98, 0xf1, 0x84, 0xc7, 0x18, 0x06, 0x3f, 0x1e, 0x30, 0x98, 0x0f, 0xff, 0x0f,
  0xff, 0xff, 0xe7, 0x99, 0xf1, 0xc0, 0xe7, 0x18, 0x0f, 0x3f, 0x3e, 0x38, 0x18, 0x07, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfe, 0x7f, 0xff, 0xff, 0x9f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x06, 0x00, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0x00, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0x00, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0x00, 0x0f,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

void setup() {
  Serial.begin(9600);
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }


  display.clearDisplay();
  display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.display();
  if (!as7341.begin()) {
    display.clearDisplay();
    Text(1, 29, 0, "Spectrometer");
    Text(1, 37, 10, "Not Found");
    Text(1, 0, 30, "Plug in and Reboot.");
    display.display();
    while (1) {
      delay(10);
    }
  }
  as7341.setATIME(100);
  as7341.setASTEP(999);
  as7341.setGain(AS7341_GAIN_256X);

  pinMode(POWER_PIN, OUTPUT);
  digitalWrite(POWER_PIN, HIGH);
  pinMode(STATUS_PIN, OUTPUT);
  pinMode(BBUT_PIN, INPUT_PULLUP);
  pinMode(ABUT_PIN, INPUT_PULLUP);
  pinMode(LED_POWER, OUTPUT);
  digitalWrite(LED_POWER, LOW);


  attachInterrupt(digitalPinToInterrupt(BBUT_PIN), BISR, LOW);
  delay(100);
  attachInterrupt(digitalPinToInterrupt(ABUT_PIN), AISR, LOW);
  delay(1000);
  display.clearDisplay();
  Text(1, 17, 0, "10");
  display.drawPixel(31, 6, SSD1306_WHITE);
  display.drawPixel(32, 5, SSD1306_WHITE);
  display.drawPixel(33, 4, SSD1306_WHITE);
  display.drawPixel(35, 6, SSD1306_WHITE);
  display.drawPixel(34, 5, SSD1306_WHITE);
  display.drawPixel(33, 3, SSD1306_WHITE);
  display.drawPixel(32, 2, SSD1306_WHITE);
  display.drawPixel(31, 1, SSD1306_WHITE);
  Text(1, 40, 0, "Spectrometer");
  Text(1, 0, 10, "by Advanced Hackspace");
  Text(1, 17, 40, "PRESS ANY BUTTON");
  Text(1, 40, 55, "TO START");
  display.display();
  while (pause) {
    analogWrite(STATUS_PIN, brightness);

    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;

    // reverse the direction of the fading at the ends of the fade:
    if (brightness <= 0 || brightness >= 255) {
      fadeAmount = -fadeAmount;
    }
    // wait for 30 milliseconds to see the dimming effect
    delay(30);
  }
  analogWrite(STATUS_PIN, 255);
  STATE = 2; //MENU

  menu = 1;
  Serial.println("Setup end");
}



void loop() {

  while (Selected == false) {
    menu_screen();
  }
  if (Selected == true) {
    selected();
  }
}

void BISR() {
  millis_BISR = millis();
  if (millis_BISR - last_millis_BISR > 100) {
    if (STATE == 1) {
      pause = false;
    }
    else if (STATE == 2) {
      Selected = true;
    }
    else if (STATE == 3) {
      repeat = true;
      
    }
    last_millis_BISR = millis_BISR;
  }
}

void AISR() {
  millis_AISR = millis();
  if (millis_AISR - last_millis_AISR > 100) {
    if (STATE == 1) {
      pause = false;
    }
    else if (STATE == 2) {
      menu = menu + 1;
      if (BLANK == false && menu == 2) {
        menu = menu + 1;
      }
      if (menu == 4) {
        menu = 1;
      }
    }
    else if (STATE == 3) {
      Selected = true;
    }
    last_millis_AISR = millis_AISR;
  }
}

void Text(int scale, int x, int y, String text) {


  display.setTextSize(scale);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(x, y);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print(text);

}

void read_sensor () {

digitalWrite(LED_POWER, HIGH);
  delay(100);
  if (!as7341.readAllChannels(readings)) {
    Serial.println("Error reading all channels!");
    return;
  }

  for (uint8_t i = 0; i < 12; i++) {
    if (i == 4 || i == 5) continue;
    // we skip the first set of duplicate clear/NIR readings
    // (indices 4 and 5)
    counts[i] = as7341.toBasicCounts(readings[i]);
  }
  digitalWrite(LED_POWER, LOW);
}


void read_Blank () {
  digitalWrite(LED_POWER, HIGH);
  delay(100);

  if (!as7341.readAllChannels(BLreadings)) {
    Serial.println("Error reading all channels!");
    return;
  }

  for (uint8_t i = 0; i < 12; i++) {
    if (i == 4 || i == 5) continue;
    // we skip the first set of duplicate clear/NIR readings
    // (indices 4 and 5)
    BLcounts[i] = as7341.toBasicCounts(BLreadings[i]);
  }
  digitalWrite(LED_POWER, LOW);
}

void selected() {
  if (menu == 1) {
    read_Blank();
    BLANK = true;
    Selected = false;
  }
  else if (menu == 2) {
    read_sensor();
    Selected = false;
    STATE = 3;
    while (Selected == false) {reading_screen();if(repeat){read_sensor();repeat = false;}}
    Selected = false;
    STATE = 2;
  }
  else {
    Selected = false;
  }
}

void menu_screen() {

  display.clearDisplay();
  Text(1, 0, 0, "=========MENU========");
  Text(1, 0, 10, "  Measure Blank");
  Text(1, 0, 20, "  Measure Absorbance");
  Text(1, 0, 30, "  Test");
  Text(1, 0, 40, "  Bluetooth");
  if (BLANK == false) {
    display.drawFastHLine(10, 23, 128, SSD1306_WHITE);
  }
  display.drawFastHLine(10, 43, 128, SSD1306_WHITE);
  Text(1, 0, 55, "A:Cycle     B:Select");
  if (menu == 1) {
    Text(1, 0, 10, ">");
  }
  else if (menu == 2) {
    Text(1, 0, 20, ">");
  }
  else if (menu == 3) {
    Text(1, 0, 30, ">");
  }

  display.display();

}

void reading_screen() {
int n1 = (counts[0]/BLcounts[0])*100;
int n2 = (counts[1]/BLcounts[1])*100;
int n3 = (counts[2]/BLcounts[2])*100;
int n4 = (counts[3]/BLcounts[3])*100;
int n5 = (counts[6]/BLcounts[6])*100;
int n6 = (counts[7]/BLcounts[7])*100;
int n7 = (counts[8]/BLcounts[8])*100;
int n8 = (counts[9]/BLcounts[9])*100;
n1 = 2 - log(n1);
n2 = 2 - log(n2);
n3 = 2 - log(n3);
n4 = 2 - log(n4);
n5 = 2 - log(n5);
n6 = 2 - log(n6);
n7 = 2 - log(n7);
n8 = 2 - log(n8);


  display.clearDisplay();
  Text(1, 0, 0, "=======READINGS======");
Text(1, 0, 10, "415:    %  445:    %");
Text(1, 0, 20, "480:    %  515:    %");
Text(1, 0, 30, "555:    %  590:    %");
Text(1, 0, 40, "630:    %  680:    %");
  Text(1, 0, 55, "A:Menu      B:Repeat");
Text(1, 25, 10, String(n1));
Text(1, 25, 20, String(n2));
Text(1, 25, 30, String(n3));
Text(1, 25, 40, String(n4));

Text(1, 90, 10, String(n5));
Text(1, 90, 20, String(n6));
Text(1, 90, 30, String(n7));
Text(1, 90, 40, String(n8));

  display.display();

}
