# esp8266-DS18B20-emoncms
This is an arduino sketch for an ESP8266 wifi module. It reads temperature from a DS18B20 and logs it to [emoncms](https://emoncms.org/) once every 30 seconds. The wifi module is put into deep-sleep between logging.

The project can be built using [PlatformIO](http://platformio.org/) after installing the DallasTemperature library.

The ESP8266 should have D0 connected to RST, in order to be able to wake from deep-sleep. The DS18B20 needs a 4.7k resistor beween DQ and VCC, and DQ should be connected to D4 on the ESP8266.
