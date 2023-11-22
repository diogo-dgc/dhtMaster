#include <Arduino.h>
#include <DHT.h>

DHT myDht;

void setup(){
  myDht.setSensorPinout(D7);
  myDht.setSensorType(DHT11);

  Serial.begin(9600);
  myDht.begin();
}

void loop(){
  delay(2000);

  float humidity = myDht.getHumidity();
  float temperature = myDht.getTemperatureCelsius();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }else{
    Serial.printf("T: %4.2f ºC|\t HI: %4.2f ºC\n", myDht.getTemperatureCelsius(), myDht.getHeatIndexCelcius());
    Serial.printf("T: %4.2f ºF|\t HI: %4.2f ºF\n", myDht.getTemperatureFahrenheit(), myDht.getHeatIndexFahrenheit());
    Serial.printf("T:  %4.2f K\t\n", myDht.getTemperatureKelvin());
    Serial.printf("RU: %4.2f \t\n\n-----\n\n", myDht.getHumidity());
  }
}