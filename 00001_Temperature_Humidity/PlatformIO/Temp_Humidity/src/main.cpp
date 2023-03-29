/* 
 *  Temperature and Humidity
 *  
 *  Sensor: DHT11
 *  
 *  Ref:
 *    Random Nerd Tutorials
 *    ESP32 with DHT11/DHT22 Temperature and Humidity Sensor using Arduino IDE
 *    https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-sensor-arduino-ide/
 *    
 *    DHT11 vs DHT22 vs LM35 vs DS18B20 vs BME280 vs BMP180
 *    https://randomnerdtutorials.com/dht11-vs-dht22-vs-lm35-vs-ds18b20-vs-bme280-vs-bmp180/#more-87441
 *    
 *    ESP32 DHT11/DHT22 Web Server – Temperature and Humidity using Arduino IDE
 *    https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/#more-39319
 *    
 *  Board:
 *     ESP32 Dev Moduler
 *     https://www.az-delivery.de/en/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/esp32-jetzt-mit-boardverwalter-installieren
 *     https://www.az-delivery.de/es/products/esp32-dev-kit-c-unverlotet
 *     
 *     
 *  Issue   
 *     Issue when there is connected to 3V3 pin, board is not able to flash.
 *     
 *     Connection with GPI012 and use 3V3 is not possible. 
 *     Gpio12 is a bootstrap pin, it can set the (wrong) flash voltage and enable the internal ldo.
 *     https://esp32.com/viewtopic.php?t=9941
 *     
 *     
 *    
*/


#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define DHTPIN 13 /* Digital pin connected to the DHT sensor */

/* Uncommented the type of sensor in use: */
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);


// board = az-delivery-devkit-v4

String readDHTTemperature() {
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  String retVal = "Nan";
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float t = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    retVal = "--";
  }
  else {
    Serial.println(t);
    retVal = String(t);
  }

  return retVal;
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

  // Wait a few seconds between measurements.
  delay(2000);
  
  Serial.println(F("Temperature [ºC]: "));
  readDHTTemperature();

}