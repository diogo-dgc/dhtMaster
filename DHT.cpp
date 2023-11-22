/*!
 *  @file DHT.cpp
 *  @author Diogo Cunha | @diogo-dgc
 *  @brief DHT1x and DHT2x Lib. This code is derivated from Adafruit DHT.cpp, many thanks!
 *  @section License
 *  MIT license, all text above must be included in any redistribution
 */
#include "DHT.h"

DHT::DHT() {
  pinout = D7;
  sensorType = DHT11;

  setSensorPinout(pinout);
  setSensorType(DHT11);

  turnOffForcedMode();
  setPullupTime(55);
  #ifdef __AVR
    _bit = digitalPinToBitMask(pin);
    _port = digitalPinToPort(pin);
  #endif
  numberOfCycles =microsecondsToClockCycles(SAMPLING_PERIOD); 
}

void DHT :: setSensorPinout(uint8_t pinout){
  this->pinout = pinout;
}

uint8_t DHT :: getSensorPinout(){
  return pinout;
}

void DHT :: setSensorType(DhtType sensorType){
  this->sensorType = sensorType;
}

DhtType DHT :: getSensorType(){
  return sensorType;
}

void DHT :: setPullupTime(uint8_t timeBeforeStart){
  pullTime = timeBeforeStart;
}

void DHT::begin() {
  pinMode(pinout, INPUT_PULLUP);
  lastReadingTime = millis() - TIME_BETWEEN_SAMPLES;
  
  PRINT("Initializing DHT%d\n", getSensorType());
  PRINT("Pinout defiened: %d\n", getSensorPinout());
  PRINT("Force Mode: %d\n", forcedMode);
  DEBUG_PRINT("DHT max clock cycles: %d", numberOfCycles);
 }

void DHT :: turnOnForcedMode(){
  forcedMode = true;
}

void DHT :: turnOffForcedMode(){
  forcedMode = false;
}

bool DHT :: isSensorAvailable(){
  uint32_t currentTime = millis();
  uint32_t timeSinceLastReading = currentTime - lastReadingTime;
  bool availableToRead;

  if (!forcedMode && (timeSinceLastReading < TIME_BETWEEN_SAMPLES)) {
    availableToRead = false;
  } else{
    availableToRead = true;
  }
  lastReadingTime = currentTime;
  return availableToRead;
}

void DHT :: tryGetSamples(){
  if(isSensorAvailable()){
    getSamples();
  }
}

void DHT :: getSamples(){
  for(uint8_t i = 0; i < SENSOR_LENGTH; i++){
    data[i] = RESET_BYTES;
  }

  #if defined(ESP8266)
    // Handle WiFi / reset software watchdog
    yield(); 
  #endif

  setHighImpedance();
  setDataLineLow();
  getDataMeasure();
  isChecksumPassed();
}

void DHT :: setHighImpedance(uint16_t microssecondsToWait){
  pinMode(pinout, INPUT_PULLUP);
  delayMicroseconds(microssecondsToWait);
}

void DHT :: setDataLineLow(){
  pinMode(pinout, OUTPUT);
  digitalWrite(pinout, LOW);
  waitLineInLow();
}

void DHT :: waitLineInLow(){
  const uint16_t PERIOD_DHT2X = 1100;
  const uint8_t PERIOD_DHT11 = 20;
  switch (sensorType) {
    case DHT22: case DHT21:
      delayMicroseconds(PERIOD_DHT2X);
      break;
    case DHT11: default:
      delay(PERIOD_DHT11);
      break;
  }
}

void DHT :: verifyTimeout(uint32_t inputToTest){
  if (inputToTest == TIMEOUT) {
    DEBUG_PRINT("TIMEOUT!\n");
    timeToReadSensor = false;
  }
}

void DHT :: getDataMeasure(){
  uint32_t cycles[TIME_PER_CYCLE_MICROSEC];

  setHighImpedance(pullTime);
  // Turn off interrupts temporarily
  InterruptLock lock;
  verifyTimeout(expectPulse(LOW));
  verifyTimeout(expectPulse(HIGH));

  for (uint8_t i = 0; i < TIME_PER_CYCLE_MICROSEC; i += 2) {
    cycles[i] = expectPulse(LOW);
    cycles[i + 1] = expectPulse(HIGH);
  }

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (uint8_t i = 0; i < (TIME_PER_CYCLE_MICROSEC / 2); ++i) {
    uint32_t lowCycles = cycles[2 * i];
    uint32_t highCycles = cycles[2 * i + 1];
    verifyTimeout(lowCycles);
    verifyTimeout(highCycles);
   
    data[i / 8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      data[i / 8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }
}

void DHT :: isChecksumPassed(){
  DEBUG_PRINT("DHT data HEX:\n\\
              \r| BYTE [0] | BYTE [1] | BYTE [2] | BYTE [3] | BYTE [4] |\n\\
              \r|   %#X    |   %#X    |   %#X    |   %#X    |   %#X    |\n",
              data[0], data[1], data[2], data[3], data[4]);

  uint8_t checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
  DEBUG_PRINT("Checksum:\t%#X\ndata[4]:\t%#X\n", checksum, data[4]);

  // Check we read 40 bits and that the checksum matches.
  if (data[4] == checksum) {
    timeToReadSensor = true;
  } else {
    DEBUG_PRINT("DHT checksum failure!\n");
    timeToReadSensor = false;
  }
}

float DHT :: temperatureAccordingSensorType(){
  float temperature = NAN;
  if (timeToReadSensor) {
    switch (sensorType) {
      case DHT11:
        temperature = data[2];
        if (data[3] & 0x80) temperature = -1 - temperature;
        temperature += (data[3] & 0x0f) * 0.1;
        break;
      case DHT12:
        temperature = data[2];
        temperature += (data[3] & 0x0f) * 0.1;
        if (data[2] & 0x80) temperature *= -1;
        break;
      case DHT22: case DHT21:
        temperature = ((word)(data[2] & 0x7F)) << 8 | data[3];
        temperature *= 0.1;
        if (data[2] & 0x80) temperature *= -1;
    }
  }
  return temperature;
}

float DHT :: umidityAccordingSensorType(){
  float umidity = NAN;
  if (timeToReadSensor) {
    switch (sensorType) {
      case DHT11: case DHT12:
        umidity = data[0] + data[1] * 0.1;
        break;
      case DHT22: case DHT21:
        umidity = ((word)data[0]) << 8 | data[1];
        umidity *= 0.1;
        break;
    }
  }
  return umidity;
}

float DHT :: getHumidity() {
  tryGetSamples();
  return umidityAccordingSensorType();
}

float DHT :: getTemperatureCelsius(){
  tryGetSamples();
  return temperatureAccordingSensorType();
}

float DHT :: getTemperatureFahrenheit(){
  return getTemperatureCelsius() * 1.8 + 32;
}

float DHT :: getTemperatureKelvin(){
  return getTemperatureCelsius() + 273.15;
}

float DHT :: getHeatIndexFahrenheit(){
  //heat index is a refinement of a result obtained by 
  //multiple regression analysis carried out by Rothfusz
  //See more at https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  float heatIndex;
  float temperature = getTemperatureFahrenheit();
  float relativeHumidity = getHumidity();
  //HI value has an error of +- 1.3 ºF

  heatIndex = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) +
              (relativeHumidity * 0.094));
  
  if (isAdjustmentNeed(heatIndex)){
    heatIndex = heatIndexAdjustment(temperature, relativeHumidity);
  }

  return heatIndex;
}

float DHT :: getHeatIndexCelcius(){
  return (getHeatIndexFahrenheit() - 32) * 0.55555;
}

bool DHT :: isAdjustmentNeed(float heatIndex){
  bool adjustmentNeed = false;
  if(heatIndex < 80) adjustmentNeed = true;

  return adjustmentNeed;
}

float DHT :: heatIndexAdjustment(float temperature, float relativeHumidity){
  float heatIndex;

  bool conditionAdjust1 = (relativeHumidity < 13) && 
                          (temperature >= 80.0) &&
                          (temperature <= 112.0); 
  
  bool conditionAdjust2 = (relativeHumidity > 85.0) &&
                          (temperature >= 80.0) &&
                          (temperature <= 87.0);

  heatIndex = -42.379 + 2.04901523 * temperature + 10.14333127 * relativeHumidity +
        -0.22475541 * temperature * relativeHumidity +
        -0.00683783 * pow(temperature, 2) +
        -0.05481717 * pow(relativeHumidity, 2) +
        0.00122874 * pow(temperature, 2) * relativeHumidity +
        0.00085282 * temperature * pow(relativeHumidity, 2) +
        -0.00000199 * pow(temperature, 2) * pow(relativeHumidity, 2);

  
  if (conditionAdjust1){
    heatIndex -= ((13.0 - relativeHumidity) * 0.25) *
          sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);
  } else if (conditionAdjust2){
    heatIndex += ((relativeHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }
  return heatIndex;
}

uint32_t DHT :: expectPulse(bool level) {
  #if (F_CPU > 16000000L) || (F_CPU == 0L)
    uint32_t count = 0;
  #else
    uint16_t count = 0; // To work fast enough on slower AVR boards
  #endif

  // On AVR platforms use direct GPIO port access as it's much faster and better
  // for catching pulses that are 10's of microseconds in length:
  #ifdef __AVR
    uint8_t portState = level ? _bit : 0;
    while ((*portInputRegister(_port) & _bit) == portState) {
      if (count++ >= numberOfCycles) {
        return TIMEOUT; // Exceeded timeout, fail.
      }
    }
  #else
    while (digitalRead(pinout) == level) {
      if (count++ >= numberOfCycles) {
        count = TIMEOUT;
      }
    }
  #endif

  return count;
}
