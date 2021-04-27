#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoHttpClient.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

Adafruit_SSD1306 display(-1);

// NETWORKING --------------------------------------------------------

char ssid[] = SECRET_SSID;    // network ID stored in arduino_secrets.h
char pass[] = SECRET_PASS;    // network password

WiFiClient wifi;
int status = WL_IDLE_STATUS;

// PROD

char serverAddress[] = "app.happyplants.io";
int port = 80;

// DEV

// char serverAddress[] = "192.168.1.2";
// int port = 3000;


HttpClient client = HttpClient(wifi, serverAddress, port);
// WebSocketClient client = WebSocketClient(wifi, serverAddress, port); // might try this later



String httpMethod = "POST";
String pathWater = "/gardener/Douglas/George/water";
String pathLight = "/gardener/Douglas/George/light";
String query = "";


// SOIL VARS --------------------------------------------------------

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

// LIGHT VARS --------------------------------------------------------

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

  // SETUP NETWORK
  while ( status != WL_CONNECTED) {
    Serial.print("=> Attempting to connect to Network named: ");
    Serial.println(ssid);            // print the network name (SSID)

    // display network status on screen
    displayPrep(0, 0, 1);
    displayNetworkData("Connecting: ", ssid);

    status = WiFi.begin(ssid, pass);
  }

  Serial.print("=> SSID: ");         // print the SSID of the network
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("=> IP Address: ");   // print your Arduino's IP address
  Serial.println(ip);

  // display network status on screen
  displayPrep(0, 0, 1);
  displayNetworkData("Connected at: ", String(ip));

  delay(1000); // so you can read connection confirmation

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
  displayPrep(0, 0, 2);

  // print to debugger
  Serial.print("Soil Moisture: ");
  Serial.println(readSoil());

  // display to oled display
  displaySensorData("Soil: ", soilVal);

  delay(2000);

  // -----------------LIGHT SENSOR---------------------

  // get display ready
  displayPrep(40, 0, 2);

  // print to debugger
  Serial.print("Light Levels: ");
  Serial.println(readLight());

  // display to oled display
  displaySensorData("Light: ", lightVal);

  delay(2000);

  // ------------------NETWORK WATER------------------------

  if (wifi.connect(serverAddress, port)) {

    String data = "value=" + String(soilVal);

    client.beginRequest();

    client.post(pathWater);
    client.sendHeader("Content-Type", "application/json");
    client.beginBody();
    client.print(data);
    client.endRequest();

    // // send HTTP request header
    // wifi.println(httpMethod + " " + pathWater + " HTTP/1.1");
    // wifi.println("Host: " + String(serverAddress));
    // wifi.println("Connection: close");
    // wifi.println(); // end HTTP request header

    // // send HTTP body
    // wifi.println(query);

    while (wifi.connected()) {
      if (wifi.available()) {
        char c = wifi.read();
        Serial.print(c);
      }
    }

    wifi.stop();
    Serial.println(); // add line between end of server response and our messages
    Serial.println("--------------");
    Serial.println("disconnected");
    
    // show confirmation on oled display
    displayPrep(0, 0, 1);
    displayNetworkData("Water Data Sent", "");
    
  } else {
    Serial.println("connection failed");
  }

  delay(1000);

    // ------------------NETWORK LIGHT------------------------

  if (wifi.connect(serverAddress, port)) {

    String data = "{value:" + String(lightVal) + "}";

    client.beginRequest();

    client.post(pathLight);
    client.sendHeader("Content-Type", "application/json");
    client.beginBody();
    client.print(data);
    client.endRequest();



    // // send HTTP request header
    // wifi.println(httpMethod + " " + pathLight + " HTTP/1.1");
    // wifi.println("Host: " + String(serverAddress));
    // wifi.println("Connection: close");
    // wifi.println(); // end HTTP request header

    // // send HTTP body
    // wifi.println(query);

    while (wifi.connected()) {
      if (wifi.available()) {
        char c = wifi.read();
        Serial.print(c);
      }
    }

    wifi.stop();
    Serial.println(); // add line between end of server response and our messages
    Serial.println("--------------");
    Serial.println("disconnected");
    
    // show confirmation on oled display
    displayPrep(0, 0, 1);
    displayNetworkData("Light Data Sent", "");
    
  } else {
    Serial.println("connection failed");
  }

  delay(1000);
  
}

// DISPLAY FUNCTIONS

void displayPrep(int xVal, int yVal, int textSize) {
  display.clearDisplay();
  display.setTextSize(textSize);
  display.setTextColor(WHITE);
  display.setCursor(xVal, yVal);
}

void displaySensorData(String label, int val) {
  display.println(label);
  display.println(val);
  display.display();
}

void displayNetworkData(String label, String status) {
  display.println(label);
  display.println(status);
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
    lightVal = analogRead(lightPin);
    return lightVal;
}
