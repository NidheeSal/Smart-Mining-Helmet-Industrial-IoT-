
//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Libraries for bmp 180
#include <Adafruit_BMP085.h>

//Libraries for mq-135
#include <MQ135.h>

//Infrared sensor
int inputPin = 34; // choose input pin (for Infrared sensor) 
int val = 0; // variable for reading the pin status
int Buzzer = 02;

//bmp sensor
Adafruit_BMP085 bmp;
float temp = 0;
float pressure = 0;
float altitude = 0;

//gas sensor       
int Gas_analog = 25;   
int Gas_digital= 15;
int Buzzer_1 = 13;  
int gas=0; 

//define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26

//433E6 for Asia
//866E6 for Europe
//915E6 for North America
#define BAND 433E6

//OLED pins
#define OLED_SDA 21
#define OLED_SCL 22 
#define OLED_RST 23
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {
  //initialize Serial Monitor
  Serial.begin(9600);

  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("LORA SENDER ");
  display.display();
  
  //Serial.println("LoRa Sender Test");

  //SPI LoRa pins
  SPI.begin(SCK, MISO, MOSI, SS);
  //setup LoRa transceiver module
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
  display.setCursor(0,10);
  display.print("LoRa Initializing OK!");
  display.display();
  delay(2000);
  
  //setup for infrared sensor & gas sensor
   
   pinMode(inputPin, INPUT);// declare Infrared sensor as input
   
   pinMode(Gas_digital, INPUT);//declare gas sensor as input
  

   //setup for buzzer
  //pinMode (Buzzer, OUTPUT);
   pinMode (Buzzer_1, OUTPUT);
   
  //setup for bmp sensor
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }

}

void loop() {
  
   //infrared
   val = digitalRead(inputPin); // read input value 
   /*if (val == LOW)
   { // check if the input is HIGH
      digitalWrite(Buzzer, LOW); // turn LED OFF   
   } 
   else 
   { 
      digitalWrite(Buzzer, HIGH); // turn LED ON 
   } 
   Serial.println(val);*/

   //gas
   
  int gassensorAnalog = analogRead(Gas_analog);
  int gassensorDigital = digitalRead(Gas_digital);
  float gas = map(gassensorAnalog,100,450,0,100);
  
  Serial.print("Gas Sensor: ");
  Serial.print(gassensorAnalog);
  Serial.print("\t");
  Serial.print("Gas Class: ");
  Serial.print(gassensorDigital);
  Serial.print("\t");
  Serial.print("\t");
  
  /*if (gassensorAnalog > 60) {
    Serial.println("Gas");
    digitalWrite (Buzzer_1, HIGH) ; //send tone
  }
  else {
    Serial.println("No Gas");
    digitalWrite (Buzzer_1, LOW) ;  //no tone
  }*/
 


   //bmp 
  Serial.print("Temperature = ");
  temp = bmp.readTemperature();
  Serial.print(temp);
  Serial.println(" *C");

  Serial.print("Pressure = ");
  pressure = bmp.readPressure();
  Serial.print(pressure);
  Serial.println(" Pa");

  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("Altitude = ");
  altitude = bmp.readAltitude();
  Serial.print(altitude);
  Serial.println(" meters");

  if (gassensorAnalog > 60)
  {
    Serial.println("Gas");
    digitalWrite (Buzzer_1, HIGH) ; 
  }
  else if ( temp > 40 || temp < 20)
  {
    Serial.println("tenperature problem");
    digitalWrite (Buzzer_1, HIGH) ; 
  }
  else if ( pressure < 12170 || pressure > 1.013e+7)
  {
    Serial.println("pressure problem");
    digitalWrite (Buzzer_1, HIGH) ; 
  }
  else if (  altitude < 19 || altitude > 8 )
  {
    Serial.println("altitude problem");
    digitalWrite (Buzzer_1, HIGH) ; 
  }

  else if (val == HIGH)
   { // check if the input is HIGH
      digitalWrite(Buzzer_1, HIGH); // turn LED OFF 
      Serial.println ("Helmet not weared");  
   } 
   
  else 
  {
    Serial.println("No problem");
    digitalWrite (Buzzer_1, LOW) ;
  }

  

  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.print(val);
  LoRa.print("|");
  LoRa.print(temp);
  LoRa.print("|");
  LoRa.print(pressure);
  LoRa.print("|");
  LoRa.print(altitude);
  LoRa.print("|");
  LoRa.print(gas);
  LoRa.print("|");
  LoRa.endPacket();
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("LORA SENDER");
  display.setCursor(0,10);
  display.setTextSize(1);
  display.print("Infrared Status:");
  display.setCursor(95,10);
  display.print(val);
  display.setCursor(0,20);
  display.print("Temperature:");
  display.setCursor(80,20);
  display.print(temp);
   display.print("*C");
  display.setCursor(0,30);
  display.print("Pressure:");
  display.setCursor(60,30);
  display.print(pressure);
  display.print("Pa");
  display.setCursor(0,40);
  display.print("Altitude:");
  display.setCursor(60,40);
  display.print(altitude);
  display.print("Meter");
  display.setCursor(0,50);
  display.print("Gas Sensor:");
  display.setCursor(80,50);
  display.print(gas);
  display.print("%");
  display.setCursor(0,60);
  display.display();
  
  delay(1000);
}
