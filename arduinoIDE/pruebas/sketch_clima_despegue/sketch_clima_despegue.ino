//InfluxDB & Grafana Tutorial
//The DIY Life by Michael Klements
//21 January 2022

#include <Wire.h>                                                   //Import the required libraries
#include "DHT.h"
#include "BMP085.h"
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "Matunosos"
// WiFi password
#define WIFI_PASSWORD "1112223334"
// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://186.65.72.253:8086"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> <select token>)
#define INFLUXDB_TOKEN "Yz9MbTBkG8XzEx9gQ7ybTkoHFY7ULlXQh_i-VBKudteNyPIp2J1SX8vIJGTfPzulXDWZjNZ1JzHvLlCwz_UGMg=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "dalmacia"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "clima"
#define TZ_INFO "CLST"                                                                                                 //InfluxDB v2 timezone

DHT dht(4,DHT22);                                                   //DHT and BMP sensor parameters
//BMP180 bmp180;

int temp = 0;                                                       //Variables to store sensor readings
int humid = 0;
int pressure = 0;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);                 //InfluxDB client instance with preconfigured InfluxCloud certificate

Point sensor("weather");                                            //Data point

void setup() 
{
  Serial.begin(115200);                                             //Start serial communication
  client.setInsecure();                                             // Set insecure connection to skip server certificate validation 
//  dht.begin();                                                      //Connect to the DHT Sensor
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
}

void loop()                                                          //Loop function
{
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
  
  Serial.print("Temp: ");                                            //Display readings on serial monitor
  Serial.println(temp);
  Serial.print("Humedad: ");
  Serial.println(humid);
//  Serial.print("Pressure: ");
//  Serial.println(pressure);
  delay(10000);                                                      //Wait 60 seconds
}