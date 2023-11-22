# DHT Master Library for Arduino framework

<p xmlns:cc="http://creativecommons.org/ns#" xmlns:dct="http://purl.org/dc/terms/"><a property="dct:title" rel="cc:attributionURL" href="https://github.com/diogo-dgc/dht_master">DHT Master Library for Arduino framework</a> by <a rel="cc:attributionURL dct:creator" property="cc:attributionName" href="https://github.com/diogo-dgc">Diogo Cunha</a> is licensed under <a href="http://creativecommons.org/licenses/by/4.0/?ref=chooser-v1" target="_blank" rel="license noopener noreferrer" style="display:inline-block;">CC BY 4.0<img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/cc.svg?ref=chooser-v1"><img style="height:22px!important;margin-left:3px;vertical-align:text-bottom;" src="https://mirrors.creativecommons.org/presskit/icons/by.svg?ref=chooser-v1"></a></p>

![Static Badge](https://img.shields.io/badge/version-1.0-blue)
![Static Badge](https://img.shields.io/badge/status-stable-green)
![Static Badge](https://img.shields.io/badge/building_ESP8266-pass-green)

This is an improvement of <a href="https://github.com/adafruit/DHT-sensor-library">Adafruit DHT Sensor Library</a>. Many thanks for all folks whom developed the original software.

## IDE Compatible
You can use this library in:
1. Arduino IDE any version;
2. VS Code using Platform.io choosing "Framework: Arduino";
    - Include the library in your `platformio.ini` file as bellow:
 ```INI
    lib_deps = 
        diogo-dgc/dht-master@^1.0
 ```

## Microcontroler Compatible
This library is optimezed for ESP-8266. However you can also use this library with ESP-32 and AVR microcontrolles.

> **Note**: Building was tested only on ESP-8266 microcontrollers

## Simple Example
```C++
#include <Arduino.h>
#include <DHT.h>

DHT myDht;

void setup(){
  myDht.setSensorPinout(D1);
  myDht.setSensorType(DHT22);

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
    Serial.printf("Umidade: %4.2f \t\n\n-----\n\n", myDht.getHumidity());
  }
}
```

## Public Methods
The public methods that you can use are presented bellow:

<details>
<summary>DHT</summary>

### `DHT <object>`
Use to instance and use your DHT sensor, for example:
```C++
DHT mySensor;
```
> **Note**: By default, the constructor initialize the pinout as D7 for de microcontroller and the sensor type DHT11
</details>

<details>
<summary>
void setSensorPinout(uint8_t pinout)
</summary>

### `setSensorPinout(<PINOUT>)`
You can define a different pinout to connect your sensor, for exemple:
```C++
void setup(){
    mySensor.setSensorPinout(D1);
}
```
</details>

<details>
<summary>
uint8_t getSensorPinout()
</summary>
</details>

<details>
<summary>
void setSensorType(DhtType sensorType)
</summary>

### `setSensorType(<SENSOR TYPE>)`
You can define which DHT sensor you want to use. The possible values are the constants DHT11, DHT12, DHT22, DHT23 or AMS3201, for exemple:
```C++
void setup(){
    mySensor.setSensorType(DHT22);
}
```
</details>

<details>
<summary>
Getters Sensor Type and Sensor Pinout
</summary>

### Getters
You can verify, any time, which pinout or sensor type you are difined using, respectively, `getSensorPinout()` or `getSensorType()` that will return an 8 bits integer, for example:
```C++
void setup(){
    Serial.begin(9600);

    mySensor.setSensorPinout(D1);
    uint8_t pinoutDefined = mySensor.getSensorPinout();

    mySensor.setSensorType(DHT22);
    uint8_t sensorTypeDefined = mySensor.getSensorType();

    mySensor.begin();
    Serial.printf("Pinout: %d | Type: DHT%d\n", pinoutDefined, sensorTypeDefined);
}
```
> **Note**: The method `getSensorPinout()` will return the PORT PIN from the microcontroller. For example: D1 in ESP-8266 will return 5 and the same D1 in Arduino Uno will return 1.
</details>

<details>
<summary>
void setPullupTime(uint8_t timeBeforeStart)
</summary>

### `setPullupTime(<TIME>);`
By default this value is setted to 55 us due to DHT datasheet instructs to define pullup.

Although, you can change the pullup time, for exemple:
```C++
void setup(){
    mySensor.setPullupTime(180);
}
```
> [!WARNING] 
> **Important**: It is not recommended change this value unless you know exactly what you want according the DHT datasheet.
</details>

<details>
<summary>
void turnOnForcedMode() e
void turnOffForcedMode()
</summary>

### `Force Mode`
Force Mode is used to ignore the predetermined time between get samples, which is 2 seconds.
By default the Force Mode is setted as `false`. But you can change this state using the methods `turnOnForcedMode()` or `turnOffForcedMode()`, for exemple:
```C++
void setup(){
    mySensor.turnOnForcedMode();
}
```
</details>


<details>
<summary>
void begin()
</summary>

### `begin();`
After define all attributes of your sensor you will need initiate it, for exemple:
```C++
void setup(){
    mySensor.setSensorPinout(D1);
    mySensor.setSensorType(DHT22);
    mySensor.begin();
}
```
</details>

<details>
<summary>
Getting data
</summary>

#### Getting Temperature, Relative Umidity and Heat Index
Your can easily get relative humidity, temperature (in Celsius degree, Fahrenheit degree or Kelvin) and heat index (in Celsius degree or Fahrenheit degree).

All methods to get data will return the value in `float` or a value `NAN` if any error occur.

```C++
void setup(){
  delay(2000);
  float umidity = mySensor.getHumidity();
  float temperatureC = mySensor.getTemperatureCelsius();
  float temperatureF = mySensor.getTemperatureFahrenheit();
  float temperatureK = mySensor.getTemperatureKelvin();
  float HeatIndexF = mySensor.getHeatIndexFahrenheit();
  float HeatIndexC = mySensor.getHeatIndexCelcius();
}
```
</details>












