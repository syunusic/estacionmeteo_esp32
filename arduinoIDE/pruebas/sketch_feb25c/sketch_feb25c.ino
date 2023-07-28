// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

// anemometro 
#include <Arduino.h>
// anemometro
#include <Wire.h>                                                   //Import the required libraries
#include "BMP085.h"
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include "DHT.h"


#define WIFI_SSID "YunuNet"
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
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


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


// anemometro
#define pinHall 19
// anemometro


// direccion del viento
#define HALL0 33
#define HALL1 25
#define HALL2 26
#define HALL3 27
// direccion del viento



DHT dht(DHTPIN, DHTTYPE);

int temperatura = 0;                                                       //Variables to store sensor readings
int humedad = 0;
int presion = 0;


// anemometro
// global variables
volatile byte counts = 0;
volatile byte velocidadViento =0;
String direccionViento = "";
volatile unsigned long lastTime = 0;
volatile unsigned long currentTime = 0;
volatile unsigned long deltaTime = 0;
// anemometro


InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);                 //InfluxDB client instance with preconfigured InfluxCloud certificate

Point sensor("weather");                                            //Data point


// anemometro
// functions
void addCount() {
  counts++;
}
// anemometro


void setup() 
{
  Serial.begin(9600);                                             //Start serial communication
  client.setInsecure();                                             // Set insecure connection to skip server certificate validation 
  dht.begin();                                                      //Connect to the DHT Sensor
//  if(!bmp180.init())                                                //Connect to pressure sensor
//    Serial.println("bmp180 init error!");

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

// anemometro
  pinMode(pinHall, INPUT_PULLUP);
  attachInterrupt(pinHall, addCount, FALLING);
// anemometro


// direccion del viento
  // Initialize the HALL sensors
  pinMode(HALL0, INPUT);
  pinMode(HALL1, INPUT);
  pinMode(HALL2, INPUT);
  pinMode(HALL3, INPUT);
// direccion del viento

}


void loop() {
  temperatura = dht.readTemperature();                                      //Record temperature
  humedad = dht.readHumidity();                                        //Record temperature
 // presion = bmp280.getPressure()/100;                               //Record pressure

  sensor.clearFields();                                              //Clear fields for reusing the point. Tags will remain untouched

  sensor.addField("temperatura", temperatura);                              // Store measured value into point
  sensor.addField("humedad", humedad);                                // Store measured value into point
 // sensor.addField("presión", presion);                             // Store measured value into point
// anemometro
  sensor.addField("velocidadViento", velocidadViento);
// direccion del viento
    sensor.addField("direccionViento", direccionViento);
    
  if (wifiMulti.run() != WL_CONNECTED)                               //Check WiFi connection and reconnect if needed
    Serial.println("Conexión WiFi fallida");

  if (!client.writePoint(sensor))                                    //Write data point
  {
    Serial.print("Falló la escritura en InfluxDB: ");
    Serial.println(client.getLastErrorMessage());
  }



  // Check if any reads failed and exit early (to try again).
  if (isnan(humedad) || isnan(temperatura)) {
    Serial.println(F("Fallo la lectura desde el sensor DHT!"));
    return;
  }



  Serial.print("Temp: ");                                            //Display readings on serial monitor
  Serial.println(temperatura);
  Serial.print("Humedad: ");
  Serial.println(humedad);
//  Serial.print("Presión: ");
//  Serial.println(pressure);
  delay(10000);                                                      //Wait 60 seconds  
  
// anomometro
  currentTime = millis();
  deltaTime = currentTime - lastTime;
  if (deltaTime >= 5000) {
    velocidadViento = counts * 0.18;
    Serial.print("Velocidad Viento: ");
    Serial.println(velocidadViento);
    counts = 0;
    lastTime = currentTime;
  }
// anomometro



// Read the HALL sensors
  int hall0 = digitalRead(HALL0);
  int hall1 = digitalRead(HALL1);
  int hall2 = digitalRead(HALL2);
  int hall3 = digitalRead(HALL3);
  // Calculate the direction with the HALL sensors
  if (hall0 == 0 && hall1 == 0 && hall2 == 0 && hall3 == 1) {
    direccionViento = "N";
    Serial.print("Direccion del viento 1: ");
    Serial.println(direccionViento);
  } else if (hall0 == 0 && hall1 == 0 && hall2 == 0 && hall3 == 0) {
    direccionViento = "NE";
    Serial.print("Direccion del viento 2: ");
    Serial.println(direccionViento);
  } else if (hall0 == 0 && hall1 == 0 && hall2 == 1 && hall3 == 0) {
    direccionViento = "E";
    Serial.print("Direccion del viento 3: ");
    Serial.println(direccionViento);
  } else if (hall0 == 1 && hall1 == 0 && hall2 == 1 && hall3 == 0) {
    direccionViento = "SE";
    Serial.print("Direccion del viento 4: ");
    Serial.println(direccionViento);
  }
    else if (hall0 == 1 && hall1 == 0 && hall2 == 1 && hall3 == 1) {
        direccionViento = "S";
    Serial.print("Direccion del viento 5: ");
        Serial.println(direccionViento);
    } else if (hall0 == 1 && hall1 == 1 && hall2 == 1 && hall3 == 1) {
        direccionViento = "SW";
    Serial.print("Direccion del viento 6: ");
        Serial.println(direccionViento);
    } else if (hall0 == 0 && hall1 == 1 && hall2 == 1 && hall3 == 1) {
        direccionViento = "W";
    Serial.print("Direccion del viento 7: ");
        Serial.println(direccionViento);
    } else if (hall0 == 0 && hall1 == 1 && hall2 == 0 && hall3 == 1) {
        direccionViento = "NW";
    Serial.print("Direccion del viento 8: ");
        Serial.println(direccionViento);
    } else {
        direccionViento = "NoHayViento";
    Serial.print("Direccion del viento 9: ");
        Serial.println(direccionViento);
    }
}