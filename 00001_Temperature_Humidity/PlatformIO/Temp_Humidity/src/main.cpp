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
 * 
 * 
 *    ESP32: Getting Started with Firebase (Realtime Database)
 *    https://randomnerdtutorials.com/esp32-firebase-realtime-database/
 * 
 *    ESP32 with Firebase – Creating a Web App
 *    https://randomnerdtutorials.com/esp32-firebase-web-app/#install-firebase-tools
 * 
 *    authDomain: esp32-firebase-demo-e1d9f.firebaseapp.com
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


#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "" //"REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "" //"REPLACE_WITH_YOUR_PASSWORD"

// Insert Firebase project API Key
#define API_KEY "" //"REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "" //"REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL" 


/**
 * @brief Define Firebase Data Objetc 
 * 
 */

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;


/**
 * @brief Sensor DHT Configuration
 * 
 */

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

  // WiFi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Firebase
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assing the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sing up */
  if (Firebase.signUp(&config, &auth, "","")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);


  // Sensor DHT init
  dht.begin();

}

void loop() {
  // put your main code here, to run repeatedly:

/*   // Wait a few seconds between measurements.
  delay(2000);
  
  Serial.println(F("Temperature [ºC]: "));
  readDHTTemperature();
 */

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/int", count)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    
    // Write an Float number on the database path test/float
    if (Firebase.RTDB.setFloat(&fbdo, "test/float", 0.01 + random(0,100))){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

}