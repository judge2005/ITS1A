#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-09-07 22:57:02

#include "Arduino.h"
#define DEBUG(...) { }
#define ALEXA
#include "Arduino.h"
#include <ConfigItem.h>
#include <EEPROMConfig.h>
#include <EEPROM.h>
#include <SPIFFSEditor.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ESPAsyncHTTPClient.h>
#include <ESPAsyncWiFiManager.h>
#include <DNSServer.h>
#include <fauxmoESP.h>
#include <TimeLib.h>
#include <ITS1ANixieDriver.h>
#include <OneNixieClock.h>
#include <TwoNixieClock.h>
#include <FourNixieClock.h>
#include <SixNixieClock.h>
#include <LEDs.h>
#include <SoftMSTimer.h>
#include <WSHandler.h>
#include <WSMenuHandler.h>
#include <WSConfigHandler.h>
#include <WSGlobalConfigHandler.h>
#include <WSPresetValuesHandler.h>
#include <WSPresetNamesHandler.h>
#include <WSInfoHandler.h>
extern unsigned long nowMs;
extern String chipId;
extern String ssid;
extern StringConfigItem hostName;
extern AsyncWebServer server;
extern AsyncWebSocket ws;
extern AsyncHTTPClient httpClient;
extern DNSServer dns;
extern AsyncWiFiManager wifiManager;
extern fauxmoESP fauxmo;
extern ITS1ANixieDriver nixieDriver;
extern NixieDriver* pDriver;
extern SixNixieClock sixNixieClock;
extern NixieClock* pNixieClock;
extern bool timeInitialized;

void initClock() ;
void grabInts(String s, int *dest, String sep) ;
void grabBytes(String s, byte *dest, String sep) ;
void readTimeFailed(String msg) ;
void setTimeFromInternet() ;
void setTimeFromWifiManager() ;
void ledTimerHandler() ;
void initFromEEPROM() ;
void createSSID() ;
void getTime() ;
void StartOTA() ;
void mainHandler(AsyncWebServerRequest *request) ;
void sendFavicon(AsyncWebServerRequest *request) ;
void broadcastUpdate(const BaseConfigItem& item) ;
void updateValue(int screen, String pair) ;
void handleWSMsg(AsyncWebSocketClient *client, char *data) ;
void wsHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) ;
void eepromUpdate() ;
void startFauxMo() ;
void SetupServer() ;
void setup() ;
void loop() ;

#include "ITS1A.ino"


#endif
