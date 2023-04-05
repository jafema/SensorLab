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
 * 
 *    ESP32 Data Logging to Firebase Realtime Database
 *    https://randomnerdtutorials.com/esp32-data-logging-firebase-realtime-database/
 *    
 *    ESP32/ESP8266: Firebase Web App to Display Sensor Readings (with Authentication)
 *    https://randomnerdtutorials.com/esp32-esp8266-firebase-web-app-sensor/
 *    
 *    ESP32/ESP8266: Firebase Data Logging Web App (Gauges, Charts, and Table)
 *    https://randomnerdtutorials.com/esp32-esp8266-firebase-gauges-charts/
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

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#include <Wire.h>
#include "time.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID //"REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD //"REPLACE_WITH_YOUR_PASSWORD"

// Insert Firebase project API Key
#define API_KEY //"REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL  //"REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL" L" 

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL  //"REPLACE_WITH_THE_USER_EMAIL"
#define USER_PASSWORD //"REPLACE_WITH_THE_USER_PASSWORD"


/**
 * @brief Define Firebase Data Objetc 
 * 
 */
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER ID
String uid;

// Database main path (to be updated in the setup with the user ID)
String databasePath;
// Database child nodes
String tempPath = "/temperature";
String humPath = "/humidity";
String timePath = "/timestamp";

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

//DTH11 sensor
#define DHTPIN 13 /* Digital pin connected to the DHT sensor */

/* Uncommented the type of sensor in use: */
#define DHTTYPE DHT11 // DHT 11
//#define DHTTYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE DHT21 // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

float temperature;
float humidity;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 180000;

// Initialize DTH11
void initDHT11(){
  dht.begin();
}

// Initialize WiFi
void initWiFi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();

}


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

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // Initialize sensor
  initDHT11();
  initWiFi();
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid + "/readings";

}

void loop() {
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath= databasePath + "/" + String(timestamp);

    json.set(tempPath.c_str(), String(dht.readTemperature()));
    json.set(humPath.c_str(), String(dht.readHumidity()));
    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }

}