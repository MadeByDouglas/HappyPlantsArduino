#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(-1);

// SOIL VARS

int soilVal = 0;
int soilPin = A1;
int soilPower = 7;

/********************************************************
 * Soil Calibration from testing different soil conditions
 *******************************************************/
// well watered > 800
// some water > 700
// dry < 400
// air < 100

// LIGHT VARS

int lightVal = 0;
int lightPin = A2;

/********************************************************
 * Light Calibration from testing different light conditions
 *******************************************************/
// very bright > 600
// indoor 80 - 200
// dark < 50

void setup() {

  //SETUP DISPLAY AND SHOW QUICK LOGO

  // initialize with the I2C addr 0x3C
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  

  // Clear the buffer.
  display.clearDisplay();

  // Display Text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Happy Plants");
  display.display();
  
  display.startscrollright(0x00, 0x07);

  delay(1000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x07);
  delay(1000);
  display.stopscroll();
  
  display.clearDisplay();

  // SETUP DEBUG OUTPUT
  Serial.begin(9600);

  // SETUP SOIL WATER SENSOR

  // pulse the soil water sensor to prevent it degrading if it were to always be on (what the internet says to do)
  pinMode(soilPower, OUTPUT);
  digitalWrite(soilPower, LOW);

  // SETUP LIGHT SENSOR
  // no need its always on and A2 already set to input

}

void loop() {
  // -----------------SOIL SENSOR-----------------------

  // get display ready
  displaySensorDisplayPrep(0, 0);

  // print to debugger
  Serial.print("Soil Moisture: ");
  Serial.println(readSoil());

  // display to oled display
  displaySensorData(soilVal, "Soil: ");

  delay(2000);

  // -----------------LIGHT SENSOR---------------------

  // get display ready
  displaySensorDisplayPrep(40, 0);

  // print to debugger
  Serial.print("Light Levels: ");
  Serial.println(readLight());

  // display to oled display
  displaySensorData(lightVal, "Light: ");

  delay(2000);

  // --------------------------------------------------
  
}





// DISPLAY FUNCTIONS

void displaySensorDisplayPrep(int xVal, int yVal) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(xVal, yVal);
}

void displaySensorData(int val, String label) {
  display.println(label);
  display.println(val);
  display.display();
}

// SOIL WATER SENSOR FUNCTIONS

int readSoil() {
    digitalWrite(soilPower, HIGH);
    delay(10);

    soilVal = analogRead(soilPin);
    digitalWrite(soilPower, LOW);
    return soilVal;
}

// LIGHT SENSOR FUNCTIONS

int readLight() {
    // digitalWrite(lightPower, HIGH);
    // delay(10);

    lightVal = analogRead(lightPin);
    // digitalWrite(lightPower, LOW);
    return lightVal;
}