#include <Arduino.h>


// board = az-delivery-devkit-v4
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("Estoy aqui");
  Serial.printf("Holaa");
}