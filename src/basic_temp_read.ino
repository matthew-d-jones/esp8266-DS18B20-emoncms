#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const char* ssid = "********";
const char* password = "********";

char domain[] = "emoncms.org";
char apikey[] = "********";

#define SLEEP_DELAY_IN_SECONDS  600
#define ONE_WIRE_BUS            D4      // DS18B20 pin
// D0 is connected to RST to wake from deep sleep

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

WiFiClient client;
unsigned long old;
char temperatureString[6];

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // setup OneWire bus
  DS18B20.begin();
}

float getTemperature() {
  Serial.println("Requesting DS18B20 temperature...");
  float temp;
  do {
    DS18B20.requestTemperatures();
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));
  return temp;
}

void loop() {
  char c;
  float temperature = getTemperature();
  // convert temperature to a string with two digits before the comma and 2 digits for precision
  dtostrf(temperature, 2, 2, temperatureString);
  // send temperature to the serial console
  Serial.print("Sending temperature: ");
  Serial.println(temperatureString);
  delay(100);

  Serial.println("connect to Server...");
  if( client.connect(domain, 80) )
  {
    Serial.println("connected");
    client.print("GET /input/post.json?json={temp:");
    client.print(temperatureString);
    client.print("}&apikey=");
    client.println(apikey);

    old = millis();
    while( (millis()-old)<500 )    // 500ms timeout for 'ok' answer from server
    {
      while( client.available() )
      {
        c = client.read();
        Serial.write(c);
      }
    }
    client.stop();
    Serial.println("\nclosed");
  }

  Serial.println("Entering deep sleep mode");
  ESP.deepSleep(SLEEP_DELAY_IN_SECONDS * 1000000, WAKE_RF_DEFAULT);

  delay(500);   // wait for deep sleep to happen
}
