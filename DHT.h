/*!
 *  @file DHT.cpp
 *  @author Diogo Cunha | @diogo-dgc
 *  @brief DHT1x and DHT2x Lib. This code is derivated from Adafruit DHT.h, many thanks!
 *  @section License
 *  MIT license, all text above must be included in any redistribution
 */
#ifndef DHT_H
#define DHT_H

#include "Arduino.h"

#define DEBUG_PRINTER true
#define DHT_INFO_PRINTER false

#if DEBUG_PRINTER
  #define PRINT(...) Serial.printf(__VA_ARGS__);
#else
  #define PRINT(...) {};
#endif

#if DHT_INFO_PRINTER
  #define DEBUG_PRINT(...) Serial.printf(__VA_ARGS__);
#else
  #define DEBUG_PRINT(...) {};
#endif

typedef enum{
  DHT11 = 11,
  DHT12,
  DHT21 = 21,
  DHT22,
  AM2301 = 21,
} DhtType;

#if defined(TARGET_NAME) && (TARGET_NAME == ARDUINO_NANO33BLE)
  #ifndef microsecondsToClockCycles
  /*!
  * As of 7 Sep 2020 the Arduino Nano 33 BLE boards do not have
  * microsecondsToClockCycles defined.
  */
  #define microsecondsToClockCycles(a) ((a) * (SystemCoreClock / 1000000L))
  #endif
#endif

class DHT {
public:
  DHT();

  void setSensorPinout(uint8_t pinout);
  uint8_t getSensorPinout();
  //! @brief Available sensor type: DHT11, DHT12, DHT22, AMS2301 
  void setSensorType(DhtType sensorType);
  DhtType getSensorType();
  void setPullupTime(uint8_t timeBeforeStart);
  void begin();
  void turnOnForcedMode();
  void turnOffForcedMode();
  float getHumidity();
  float getTemperatureCelsius();
  float getTemperatureFahrenheit();
  float getTemperatureKelvin();
  float getHeatIndexFahrenheit();
  float getHeatIndexCelcius();
  
private:
  #define SAMPLING_PERIOD 1000
  #define SENSOR_LENGTH 5
  #define RESET_BYTES 0x00
  #define TIME_PER_CYCLE_MICROSEC 80
  #define TIME_BETWEEN_SAMPLES 2000
  #define TIMEOUT UINT32_MAX 
  #ifdef __AVR
    // Use direct GPIO access on an 8-bit AVR so keep track of the port and
    // bitmask for the digital pin connected to the DHT.  Other platforms will use
    // digitalRead.
    uint8_t _bit, _port;
  #endif
 
  uint8_t data[SENSOR_LENGTH];
  uint8_t pinout;
  DhtType sensorType;
  bool forcedMode;
  uint32_t lastReadingTime;
  uint32_t numberOfCycles;
  bool timeToReadSensor;
  uint8_t pullTime;

  bool isSensorAvailable();
  void tryGetSamples();
  void getSamples();
  void setHighImpedance(uint16_t microssecondsToWait = 1000);
  void setDataLineLow();
  void waitLineInLow();
  void verifyTimeout(uint32_t inputToTest);
  void getDataMeasure();
  void isChecksumPassed();
  float temperatureAccordingSensorType();
  float umidityAccordingSensorType();
  bool isAdjustmentNeed(float heatIndex);
  float heatIndexAdjustment(float temperature, float relativeHumidity);
  uint32_t expectPulse(bool level);
};

class InterruptLock {
public:
  InterruptLock() {
    #if !defined(ARDUINO_ARCH_NRF52)
      noInterrupts();
    #endif
  }
  ~InterruptLock() {
    #if !defined(ARDUINO_ARCH_NRF52)
      interrupts();
    #endif
  }
};
#endif
