// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
#include <Wire.h>                                                   //Import the required libraries
#include "BMP085.h"
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include "DHT.h"


#define WIFI_SSID "Matunosos"
#define WIFI_PASSWORD "1112223334"
#define INFLUXDB_URL "https://186.65.72.253:8086"
#define INFLUXDB_TOKEN "Yz9MbTBkG8XzEx9gQ7ybTkoHFY7ULlXQh_i-VBKudteNyPIp2J1SX8vIJGTfPzulXDWZjNZ1JzHvLlCwz_UGMg=="
#define INFLUXDB_ORG "dalmacia"
#define INFLUXDB_BUCKET "clima"
#define TZ_INFO "CLST"                  


#define DHTPIN 4     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);


//// HAL Effect
// volatile byte half_revolutions;
// unsigned int rpm;
// unsigned long timeold;
//// HAL Effect


int temp = 0;                                                       //Variables to store sensor readings
int humid = 0;
int pressure = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);                 //InfluxDB client instance with preconfigured InfluxCloud certificate

Point sensor("weather");                                            //Data point

void setup() 
{
  Serial.begin(9600);                                             //Start serial communication
  client.setInsecure();                                             // Set insecure connection to skip server certificate validation 
  dht.begin();                                                      //Connect to the DHT Sensor
//  if(!bmp180.init())                                                //Connect to pressure sensor
//    Serial.println("bmp180 init error!");

//// HAL Effect
//   attachInterrupt(19, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 19)
//   half_revolutions = 0;
//   rpm = 0;
//   timeold = 0;
//// HAL Effect

  WiFi.mode(WIFI_STA);                                              //Setup wifi connection
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");                               //Connect to WiFi
  while (wifiMulti.run() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  sensor.addTag("device", DEVICE);                                   //Add tag(s) - repeat as required
  sensor.addTag("SSID", WIFI_SSID);

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");                 //Accurate time is necessary for certificate validation and writing in batches

  if (client.validateConnection())                                   //Check server connection
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } 
  else 
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}


void loop() {
  temp = dht.readTemperature();                                      //Record temperature
  humid = dht.readHumidity();                                        //Record temperature
 // pressure = bmp280.getPressure()/100;                               //Record pressure

  sensor.clearFields();                                              //Clear fields for reusing the point. Tags will remain untouched

  sensor.addField("temperatura", temp);                              // Store measured value into point
  sensor.addField("humedad", humid);                                // Store measured value into point
 // sensor.addField("pressure", pressure);                             // Store measured value into point

    
  if (wifiMulti.run() != WL_CONNECTED)                               //Check WiFi connection and reconnect if needed
    Serial.println("Wifi connection lost");

  if (!client.writePoint(sensor))                                    //Write data point
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }



  // Check if any reads failed and exit early (to try again).
  if (isnan(humid) || isnan(temp)) {
    Serial.println(F("Fallo la lectura desde el sensor DHT!"));
    return;
  }



  Serial.print("Temp: ");                                            //Display readings on serial monitor
  Serial.println(temp);
  Serial.print("Humedad: ");
  Serial.println(humid);
//  Serial.print("Pressure: ");
//  Serial.println(pressure);
  delay(10000);                                                      //Wait 60 seconds  
  
  
  
  
}

////HAL Effect
// void loop()//Measure RPM
// {
//   if (half_revolutions >= 20) { 
//     rpm = 30*1000/(millis() - timeold)*half_revolutions;
//     timeold = millis();
//     half_revolutions = 0;
//     //Serial.println(rpm,DEC);
//   }
// }
//  void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
// {
//   half_revolutions++;
//   Serial.println("detect");
// }
// //HAL Effect