//group WeatherU
// this file contion 3 parts
//#1 DHT11
//#2 ALSPT19 Light Sensor
//#3 Raindrops modules
//#4 Air Pressure sensor
//#5 Ambien light sensor

#include "DHT.h"
#include "Adafruit_VEML7700.h"
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>

//notice our sensor number here
#define DHTTYPE DHT11
#define LED 2
#define DHTPIN 7
#define LIGHTPIN A0
#define UVPIN A1
#define RAINPIN A2
#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

SoftwareSerial espSerial(5, 6);// 5TX 6RX
DHT dht(DHTPIN, DHTTYPE);//DHT
Adafruit_BMP280 bmp(BMP_CS); // hardware SPI
Adafruit_VEML7700 veml = Adafruit_VEML7700();//VEML7700 I2C

String str;
float lightthreshold = 2.5;
int rainthreshold = 500;

//-----------------DHT11 Part--------------------
float send_humidity()
{
//  delay(1000); //wait a few seconds between measurement.
  float humidity = dht.readHumidity();
  Serial.print(F(" Humidity: "));
  Serial.print(humidity);
  Serial.println("%");
  return humidity; 
  }

float send_temperature()
{
//  delay(1000); //wait a few seconds between measurement.
  float temperature = dht.readTemperature();
  Serial.print(F(" Temperature: "));
  Serial.print(temperature);
  Serial.println("C");
  return temperature; 
  }

//-----------------Light Part--------------------
float send_light()
{
//  delay(1000); //wait a few seconds between measurement.
  float lightValue = analogRead(LIGHTPIN);
  float lightVoltage = lightValue/1024*5.0;
  if(lightVoltage < lightthreshold) {
    digitalWrite(LED, HIGH);
    }
  else{
    digitalWrite(LED, LOW);
    }

  Serial.print(F(" Light Value: "));
  Serial.print(lightValue);
  Serial.println("lux");
  
  return lightValue;
  }

//-----------------Rain Part--------------------
float send_rain()
{
//  delay(1000); //wait a few seconds between measurement.
  float rainValue = analogRead(RAINPIN);
  if(rainValue < rainthreshold){
  Serial.println(" - It's wet");
  }
  else {
  Serial.println(" - It's dry");
  }

  Serial.print(F(" Rain Value: "));
  Serial.print(rainValue);
  Serial.println("L");
  
  return rainValue;
  
  }
//-----------------Pressure Part--------------------
float send_pressure()
{
//  delay(1000); //wait a few seconds between measurement.
  float pressure = bmp.readPressure();
  Serial.print(F("Pressure = "));
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  return pressure;
  }

//-----------------Ambient light Part--------------------
float send_Lux()
{
//  delay(1000); //wait a few seconds between measurement.
  float Lux = veml.readLux();
  Serial.print(F("LUX = "));
  Serial.print(Lux);
  Serial.println("lux");
  return Lux;
  }

void setup() {
  Serial.begin(115200);
  while ( !Serial ) delay(100);  
  Serial.println(F("WeatherU test!"));
  espSerial.begin(115200);
  
  pinMode(LED, OUTPUT);
  dht.begin();
  bmp.begin();
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */      
  veml.begin();
  veml.setGain(VEML7700_GAIN_1);
  veml.setIntegrationTime(VEML7700_IT_800MS);
  switch (veml.getGain()) {
    case VEML7700_GAIN_1: Serial.println("1"); break;
    case VEML7700_GAIN_2: Serial.println("2"); break;
    case VEML7700_GAIN_1_4: Serial.println("1/4"); break;
    case VEML7700_GAIN_1_8: Serial.println("1/8"); break;
    }
  veml.setLowThreshold(10000);
  veml.setHighThreshold(20000);
  veml.interruptEnable(true);
}

void loop() {
  float a,b,c,d,e,f;
  a = send_humidity();
  b = send_temperature();
  c = send_light();
  d = send_rain();
  e = send_pressure();
  f = send_Lux();

  str = String("Humidity:")+String(a)+String(" Temperature:")+String(b)+String(" Light:")+String(c)+String(" RainValue:")+String(d)+String(" AirPressure:")+String(e)+String(" Lux:")+String(f)+String("&");   
  espSerial.print(str);
  Serial.println(str);
  delay(1000);
}
