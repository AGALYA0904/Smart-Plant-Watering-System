#include "DHT.h"

#define DHTPIN 2

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);
#include <ESP8266WiFi.h>
 
String apiKey = "4AUC7U13QHRC5QDY";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "venkat";     // replace with your wifi ssid and wpa2 key
const char *pass =  "lamborghini";
const char* server = "api.thingspeak.com";
int soilDry = 35;
const int sensor_pin = A0;  // Connect Soil moisture analog sensor pin to A0 of NodeMCU 
WiFiClient client;

void setup() 
{
  pinMode(D2,OUTPUT);
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));
  dht.begin();

  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
}

void loop()
{ 
  // Wait a few seconds between measurements.
  delay(2000);

  int mp;
 
  mp = ( 100.00 - ( (analogRead(sensor_pin)/1023.00) * 100.00 ) );
 
    Serial.print("Soil Moisture(in Percentage) = ");
    Serial.print(mp);
    Serial.println("%");
    
    if(mp > soilDry)
    {
      digitalWrite(D2, HIGH);
      delay(2000);
    }
    else
    {
      digitalWrite(D2, LOW);
    }
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) 
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));

  if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
  {
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(h);
    postStr +="&field2=";
    postStr += String(t);
    postStr +="&field3=";
    postStr += String(f);
    postStr +="&field4=";
    postStr += String(hic);
    postStr +="&field5=";
    postStr += String(hif);
    postStr +="&field6=";
    postStr += String(mp);
    postStr += "\r\n\r\n";
    
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    
    Serial.println("%. Send to Thingspeak.");
    client.stop();
    
    Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
  }
  }
