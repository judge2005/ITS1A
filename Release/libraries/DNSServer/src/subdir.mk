################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\DNSServer\src\DNSServer.cpp 

LINK_OBJ += \
.\libraries\DNSServer\src\DNSServer.cpp.o 

CPP_DEPS += \
.\libraries\DNSServer\src\DNSServer.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries\DNSServer\src\DNSServer.cpp.o: C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\DNSServer\src\DNSServer.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"C:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-g++" -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ "-IC:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/include" "-IC:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/lwip/include" "-IC:\Users\mpand\eclipse-workspace\ITS1A/Release/core" -c -Wall -Wextra -Os -g -mlongcalls -mtext-section-literals -fno-exceptions -fno-rtti -falign-functions=4 -std=c++11 -ffunction-sections -fdata-sections -DF_CPU=80000000L -DLWIP_OPEN_SRC  -DARDUINO=10802 -DARDUINO_ESP8266_ESP01 -DARDUINO_ARCH_ESP8266 "-DARDUINO_BOARD=\"ESP8266_ESP01\"" -DESP8266  -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\cores\esp8266" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\variants\generic" -I"C:\Users\mpand\Documents\Arduino\libraries\ESPAsyncHttpClient" -I"C:\Users\mpand\Documents\Arduino\libraries\ESPAsyncTCP" -I"C:\Users\mpand\Documents\Arduino\libraries\ESPAsyncTCP\src" -I"C:\Users\mpand\Documents\Arduino\libraries\ESPAsyncWiFiManager" -I"C:\Users\mpand\Documents\Arduino\libraries\NixieDriver" -I"C:\Users\mpand\Documents\Arduino\libraries\NixieMisc" -I"C:\Users\mpand\Documents\Arduino\libraries\OneNixieClock" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\DNSServer" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\DNSServer\src" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266WiFi\src" -I"C:\Users\mpand\Documents\Arduino\libraries\ESPAsyncWebServer" -I"C:\Users\mpand\Documents\Arduino\libraries\ESPAsyncWebServer\src" -I"C:\Users\mpand\Documents\Arduino\libraries\Time" -I"C:\Users\mpand\Documents\Arduino\libraries\Adafruit_NeoPixel" -I"C:\Users\mpand\Documents\Arduino\libraries\ArduinoJson" -I"C:\Users\mpand\Documents\Arduino\libraries\ArduinoJson\src" -I"C:\Users\mpand\Documents\Arduino\libraries\Configs" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\Hash" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\Hash\src" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\SPI" -I"C:\Users\mpand\Documents\Arduino\libraries\UPS" -I"C:\Users\mpand\Documents\Arduino\libraries\Adafruit_LIS3DH" -I"C:\Users\mpand\Documents\Arduino\libraries\Adafruit_MCP23017_Arduino_Library" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\EEPROM" -I"C:\Users\mpand\Documents\Arduino\libraries\U8g2" -I"C:\Users\mpand\Documents\Arduino\libraries\U8g2\src" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\Wire" -I"C:\Users\mpand\eclipse\arduinoPlugin\libraries\Adafruit_Unified_Sensor\1.0.2" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ESP8266mDNS" -I"C:\Users\mpand\eclipse\arduinoPlugin\packages\esp8266\hardware\esp8266\2.3.0\libraries\ArduinoOTA" -I"C:\Users\mpand\Documents\Arduino\libraries\FauxmoESP" -I"C:\Users\mpand\Documents\Arduino\libraries\FauxmoESP\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


