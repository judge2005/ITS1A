################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ..\makefile.init

RM := del 

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries\Wire\subdir.mk
-include libraries\UPS\subdir.mk
-include libraries\U8g2\src\clib\subdir.mk
-include libraries\U8g2\src\subdir.mk
-include libraries\Time\subdir.mk
-include libraries\SPI\subdir.mk
-include libraries\OneNixieClock\subdir.mk
-include libraries\NixieMisc\subdir.mk
-include libraries\NixieDriver\subdir.mk
-include libraries\Hash\src\sha1\subdir.mk
-include libraries\Hash\src\subdir.mk
-include libraries\FauxmoESP\src\subdir.mk
-include libraries\ESPAsyncWiFiManager\subdir.mk
-include libraries\ESPAsyncWebServer\src\subdir.mk
-include libraries\ESPAsyncTCP\src\subdir.mk
-include libraries\ESPAsyncHttpClient\subdir.mk
-include libraries\ESP8266mDNS\subdir.mk
-include libraries\ESP8266WiFi\src\subdir.mk
-include libraries\EEPROM\subdir.mk
-include libraries\DNSServer\src\subdir.mk
-include libraries\Configs\subdir.mk
-include libraries\ArduinoOTA\subdir.mk
-include libraries\Adafruit_NeoPixel\subdir.mk
-include libraries\Adafruit_MCP23017_Arduino_Library\subdir.mk
-include libraries\Adafruit_LIS3DH\subdir.mk
-include core\core\umm_malloc\subdir.mk
-include core\core\spiffs\subdir.mk
-include core\core\libb64\subdir.mk
-include core\core\subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ..\makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
C:\Users\mpand\eclipse-workspace\ITS1A\Release\arduino.ar \

ELF += \
ITS1A.elf \

HEX += \
ITS1A.hex \


# All Target
all: ITS1A

# Tool invocations
ITS1A: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"C:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-size" -A "C:/Users/mpand/eclipse-workspace/ITS1A/Release/ITS1A.elf"
	@echo 'Finished building target: $@'
	@echo ' '

C:\Users\mpand\eclipse-workspace\ITS1A\Release\arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"C:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-ar" cru "C:\Users\mpand\eclipse-workspace\ITS1A/Release/arduino.ar"    C:/Users/mpand/eclipse-workspace/ITS1A/Release/arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

ITS1A.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"C:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9-2/bin/xtensa-lx106-elf-gcc" -g -Wall -Wextra -Os -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static "-LC:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/lib" "-LC:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/tools/sdk/ld" "-Teagle.flash.1m256.ld" -Wl,--gc-sections -Wl,-wrap,system_restart_local -Wl,-wrap,register_chipv6_phy -o "C:\Users\mpand\eclipse-workspace\ITS1A/Release/ITS1A.elf" -Wl,--start-group   $(LINK_OBJ)  "C:\Users\mpand\eclipse-workspace\ITS1A/Release/arduino.ar" -lm -lgcc -lhal -lphy -lpp -lnet80211 -lwpa -lcrypto -lmain -lwps -laxtls -lsmartconfig -lmesh -lwpa2 -llwip_gcc -lstdc++ -Wl,--end-group "-LC:\Users\mpand\eclipse-workspace\ITS1A/Release" C:/Users/mpand/eclipse-workspace/ITS1A/Release/arduino.ar
	@echo 'Finished building: $@'
	@echo ' '

ITS1A.hex: $(ELF)
	@echo 'Create eeprom image'
	"C:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/tools/esptool/0.4.13/esptool.exe" -eo "C:\Users\mpand\eclipse\/arduinoPlugin/packages/esp8266/hardware/esp8266/2.3.0/bootloaders/eboot/eboot.elf" -bo "C:/Users/mpand/eclipse-workspace/ITS1A/Release/ITS1A.bin" -bm qio -bf 40 -bz 1M -bs .text -bp 4096 -ec -eo "C:/Users/mpand/eclipse-workspace/ITS1A/Release/ITS1A.elf" -bs .irom0.text -bs .text -bs .data -bs .rodata -bc -ec
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ..\makefile.targets
