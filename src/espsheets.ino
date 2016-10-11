//-----------------------------------------------
//This sketch is combined from Adafruit DHT sensor and tdicola
//for dht.h library
//Along with some  esp8266 NodeMCU specifics from teos009
// https://learn.adafruit.com/dht/overview
// https://gist.github.com/teos0009/acad7d1e54b97f4b2a88
//other Authors Arduino and associated Google Script:
//Aditya Riska Putra
//Ahmed Reza Rafsanzani
//Ryan Eko Saputro
//See Also
//http://jarkomdityaz.appspot.com/
//
//ELINS UGM
//
//Modified for Hackster.io project to the ESP8266 from
//a Ethernet Sheild project
//by Stephen Borsay
//Since Arduino can't handle https, we need to use Pushingbox API
//(uses http)to run
//the Google Script (uses https).


#include "ESP8266WiFi.h"
#include "DHT.h"
#define DHTPIN 2    // what digital pin we're connected to
                    // pin2 to D4 on esp board

#define DHTTYPE DHT11
#define GOOGLE_SHEETS_QUOTA_TIMEOUT 10000
DHT dht(DHTPIN,DHTTYPE);

const char WEBSITE[] = "api.pushingbox.com"; // pushingbox API server
const String devid = "DEVICE_ID"; // device ID from Pushingbox

const char* MY_SSID = "ssid";
const char* MY_PWD =  "pwd";

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.print("Connecting to "+*MY_SSID);
  WiFi.begin(MY_SSID, MY_PWD);
  Serial.println("going into wl connect");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("wl connected");
  Serial.println("");
  Serial.println("Credentials accepted! Connected to wifi\n ");
  Serial.println("");
}

void loop() {
  delay(GOOGLE_SHEETS_QUOTA_TIMEOUT);

  float humidityData = dht.readHumidity();
  float celData = dht.readTemperature();
  float fehrData = dht.readTemperature(true);

  if (isnan(humidityData) || isnan(celData) || isnan(fehrData)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float hicData = dht.computeHeatIndex(celData, humidityData, false);
  float hifData = dht.computeHeatIndex(fehrData, humidityData);

  Serial.print("Humidity: ");
  Serial.print(humidityData);
  Serial.print(" %\t");
  Serial.print("Temperature in Cel: ");
  Serial.print(celData);
  Serial.print(" *C ");
  Serial.print("Temperature in Fehr: ");
  Serial.print(fehrData);
  Serial.print(" *F\t");
  Serial.print("Heat index in Cel: ");
  Serial.print(hicData);
  Serial.print(" *C ");
  Serial.print("Heat index in Fehr: ");
  Serial.print(hifData);
  Serial.print(" *F\n");

  WiFiClient client;
  if (client.connect(WEBSITE, 80)) {
    client.print("GET /pushingbox?devid=" + devid
      + "&humidityData=" + (String) humidityData
      + "&celData="      + (String) celData
      + "&fehrData="     + (String) fehrData
      + "&hicData="      + (String) hicData
      + "&hifData="      + (String) hifData
    );

    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(WEBSITE);
    client.println("User-Agent: ESP8266/1.0");
    client.println("Connection: close");
    client.println();
  }
}
