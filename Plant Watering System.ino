#include <Arduino.h>
#include <LiquidCrystal_I2C.h>    // For Liquid Crystal
#include <Wire.h>                 //For the Liquid Crystal
#include <NewPing.h>              //For the Ultrasonic Sensor
#include <SoftwareSerial.h>       //For the GSM module
String number = "+6281222329xxx"; //-> change with your number
String _buffer;
int _timeout;

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial sim(8, 7);

int IN1 = 4; // Relay 1 input pin
int IN2 = 3; // Relay 2 input pin

int SEN1 = A4; // Moisture Sensor 1 Value
int SEN2 = A3; // Moisture Sensor 2 Value

float sensor1Value = 0; // Storing the value of the Moisture Sensor 1
float sensor2Value = 0; // Storing the value of the Moisture Sensor 1

NewPing ultrason(10, 9, 400); // Ultrasonic Sensor
void setup()
{
  // put your setup code here, to run once:

  Serial.begin(9600);
  delay(500);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(SEN1, INPUT);
  pinMode(SEN2, INPUT);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);

  sensor1Value = analogRead(SEN1); // Assigning the value for using in the looping argument
  sensor2Value = analogRead(SEN2); // Assigning the value for using in the looping argument

  sim.begin(9600);
  _buffer.reserve(50);
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (ultrason.ping_cm() > 20)
  {
    pumppower();
    displaydata();
  }
  else
  {
    SendWaterLevel(); // Send Message
    delay(60UL * 60UL * 1000UL);
  }
}

void pumppower()
{

  if (sensor1Value > 450)
  { // The value set is can be changed based on the needs of the plant water pump 1 is on
    digitalWrite(IN1, LOW);
  }
  else if (sensor2Value > 450)
  { // The value set is can be changed based on the needs of the plant water pump 2 is on
    digitalWrite(IN2, LOW);
  }

  else if (sensor1Value <= 450)
  { // The value set is can be changed based on the needs of the plant water pump 1 is off
    digitalWrite(IN1, HIGH);
  }

  else if (sensor2Value <= 450)
  { // The value set is can be changed based on the needs of the plant water pump 2 is off
    digitalWrite(IN2, HIGH);
  }
}

void displaydata()
{

  lcd.backlight();
  lcd.print("Sensor 1 Moisture:");
  lcd.setCursor(0, 1);
  lcd.print(sensor1Value);
  delay(1000);
  lcd.clear();

  lcd.print("Sensor 2 Moisture:");
  lcd.setCursor(0, 1);
  lcd.print(sensor2Value);
  delay(1000);
  lcd.clear();

  lcd.print("The distance is: ");
  lcd.setCursor(0, 1);
  lcd.println(ultrason.ping_cm());
  delay(1000);
  lcd.clear();
}

void SendMessageData()
{
  String Sensor1 = "";
  String Sensor2 = "";
  String WaterLevel = "";
  Sensor1.concat(sensor1Value);
  Sensor2.concat(sensor2Value);
  WaterLevel.concat(ultrason.ping_cm());
  // Serial.println ("Sending Message");
  sim.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
  delay(200);
  // Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); // Mobile phone number to send message
  delay(200);
  String SMS = "Soil Moisture 1 :" + Sensor1 + "Soil Moisture 2 :" + Sensor2 + "The Water Level is: " + WaterLevel;
  sim.println(SMS);
  delay(100);
  sim.println((char)26); // ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();
}

void SendWaterLevel()
{

  // Serial.println ("Sending Message");
  sim.println("AT+CMGF=1"); // Sets the GSM Module in Text Mode
  delay(200);
  // Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); // Mobile phone number to send message
  delay(200);
  String SMS = "The water Level is not Enough";
  sim.println(SMS);
  delay(100);
  sim.println((char)26); // ASCII code of CTRL+Z
  delay(200);
  _buffer = _readSerial();
}

String _readSerial()
{
  _timeout = 0;
  while (!sim.available() && _timeout < 12000)
  {
    delay(13);
    _timeout++;
  }
  if (sim.available())
  {
    return sim.readString();
  }
}
