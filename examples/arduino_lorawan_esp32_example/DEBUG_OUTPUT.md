# Debug Output Configuration in Arduino IDE

## ESP32

1. Select appropriate (USB-)serial port for your board
   
    ![Arduino_IDE-Tools_Port](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/be496bf8-89ce-4db5-b1bf-c88a7f5e99cb)

**or**

   ![Arduino_IDE-Select_Other_Board_and_Port](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/ac847f23-4fe6-4111-929f-ac6d36cb8a53)
  
2. Select desired debug level

    ![Arduino_IDE-Tools_CoreDebugLevel](https://github.com/matthias-bs/BresserWeatherSensorTTN/assets/83612361/72a8b1d9-8d39-41fc-9658-78b432b73d56)

  This passes the define `CORE_DEBUG_LEVEL`to the compiler accordingly.

Refer to the following for some background information
* https://thingpulse.com/esp32-logging/
* https://www.mischianti.org/2020/09/20/esp32-manage-multiple-serial-and-logging-for-debugging-3/
* https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-log.h
