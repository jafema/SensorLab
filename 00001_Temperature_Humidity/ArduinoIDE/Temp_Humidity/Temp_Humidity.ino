/* 
 *  Temperature and Humidity
 *  Sensor: DHT11
 *  Ref:
 *    Random Nerd Tutorials
 *    https://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/#more-39319
 *  Board:
 *     ESP32 Dev Moduler
 *     https://www.az-delivery.de/en/blogs/azdelivery-blog-fur-arduino-und-raspberry-pi/esp32-jetzt-mit-boardverwalter-installieren
 *     https://www.az-delivery.de/es/products/esp32-dev-kit-c-unverlotet
 *     
 *     
 *     Issue when there is connected to 3V3 pin, board is not able to flash.
 *    
*/


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Estoy aqui 7");

}
