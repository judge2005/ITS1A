//#define DEBUG(...) { Serial.println(__VA_ARGS__); }
#define DEBUG(...) { }
#define ALEXA
//#define OTA

#include "Arduino.h"
#include <ConfigItem.h>
#include <EEPROMConfig.h>
#ifdef OTA
#include <ArduinoOTA.h>
#endif
#include <EEPROM.h>
#include <SPIFFSEditor.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
//#include <ESP8266HTTPClient.h>
#include <ESPAsyncHTTPClient.h>
#include <ESPAsyncWiFiManagerOTC.h>
#include <DNSServer.h>
#ifdef ALEXA
#include <fauxmoESP.h>
#endif
#include <TimeLib.h>

#include <ITS1ANixieDriver.h>
#include <OneNixieClock.h>
#include <TwoNixieClock.h>
#include <FourNixieClock.h>
#include <SixNixieClock.h>
#include <LEDs.h>
#include <SoftMSTimer.h>
#include <MovementSensor.h>

#include <WSHandler.h>
#include <WSMenuHandler.h>
#include <WSConfigHandler.h>
#include <WSGlobalConfigHandler.h>
#include <WSPresetValuesHandler.h>
#include <WSPresetNamesHandler.h>
#include <WSInfoHandler.h>

const byte MovPin = 3;	// PIR/Radar etc.

unsigned long nowMs = 0;

String chipId = String(ESP.getChipId(), HEX);
String ssid = "STC-";

StringConfigItem hostName("hostname", 63, "ITS1AClock");

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncHTTPClient httpClient;
DNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
#ifdef ALEXA
fauxmoESP fauxmo;
#endif
ITS1ANixieDriver nixieDriver(6);
NixieDriver *pDriver = &nixieDriver;
SixNixieClock sixNixieClock(pDriver);
NixieClock *pNixieClock = &sixNixieClock;
bool timeInitialized = false;

class Configurator {
public:
	virtual void configure() = 0;
};

namespace ConfigSet1 {
#include <ITS1AConfigSet1.h>
}

namespace ConfigSet2 {
#include <ITS1AConfigSet2.h>
}	// End namespace

namespace ConfigSet3 {
#include <ITS1AConfigSet3.h>
}	// End namespace

namespace ConfigSet4 {
#include <ITS1AConfigSet4.h>
}	// End namespace

namespace ConfigSet5 {
#include <ITS1AConfigSet5.h>
}	// End namespace

StringConfigItem set1Name("set1_name", 12, "24 Hour");
StringConfigItem set2Name("set2_name", 12, "12 Hour");
StringConfigItem set3Name("set3_name", 12, "Fast Clock");
StringConfigItem set4Name("set4_name", 12, "Test");
StringConfigItem set5Name("set5_name", 12, "Manual");
BaseConfigItem *configSetPresetNames[] = {
	&set1Name,
	&set2Name,
	&set3Name,
	&set4Name,
	&set5Name,
	0
};

CompositeConfigItem presetNamesConfig("preset_names", 0, configSetPresetNames);

StringConfigItem currentSet("current_set", 4, "set1");

// Alexa config values
StringConfigItem date_name("date_name", 20, String("date"));
StringConfigItem backlight_name("backlight_name", 20, String("backlight"));
StringConfigItem underlight_name("underlight_name", 20, String("underlight"));
StringConfigItem clock_name("clock_name", 20, String("clock"));
StringConfigItem test_name("test_name", 20, String("test"));
StringConfigItem cycling_name("cycling_name", 20, String("hue cycling"));
StringConfigItem twelve_hour_name("twelve_hour_name", 20, String("12 hour"));
StringConfigItem zero_name("zero_name", 20, String("leading zero"));

BaseConfigItem *alexaSet[] = {
	// Alexa
	&date_name,
	&backlight_name,
	&underlight_name,
	&clock_name,
	&test_name,
	&cycling_name,
	&twelve_hour_name,
	&zero_name,
	0
};

CompositeConfigItem alexaConfig("alexa", 0, alexaSet);

BaseConfigItem *configSetGlobal[] = {
	&hostName,
	&currentSet,
	&alexaConfig,
	0
};

CompositeConfigItem globalConfig("global", 0, configSetGlobal);

BaseConfigItem *configSetRoot[] = {
	&globalConfig,
	&presetNamesConfig,
	&ConfigSet1::config,
	&ConfigSet2::config,
	&ConfigSet3::config,
	&ConfigSet4::config,
	&ConfigSet5::config,
	0
};

CompositeConfigItem rootConfig("root", 0, configSetRoot);

EEPROMConfig config(rootConfig);

namespace CurrentConfig {
	String name("set1");
	CompositeConfigItem *config = &ConfigSet1::config;

	// Clock config values
	BooleanConfigItem *time_or_date = &ConfigSet1::time_or_date;
	ByteConfigItem *date_format = &ConfigSet1::date_format;
	BooleanConfigItem *hour_format = &ConfigSet1::hour_format;
	BooleanConfigItem *leading_zero = &ConfigSet1::leading_zero;
	ByteConfigItem *fading = &ConfigSet1::fading;
	ByteConfigItem *colons = &ConfigSet1::colons;
	ByteConfigItem *display_on = &ConfigSet1::display_on;
	ByteConfigItem *display_off = &ConfigSet1::display_off;
	StringConfigItem *time_url = &ConfigSet1::time_url;

	// LED config values
	ByteConfigItem *hue = &ConfigSet1::hue;
	ByteConfigItem *saturation = &ConfigSet1::saturation;
	BooleanConfigItem *backlight = &ConfigSet1::backlight;
	BooleanConfigItem *underlight = &ConfigSet1::underlight;
	BooleanConfigItem *hue_cycling = &ConfigSet1::hue_cycling;
	ByteConfigItem *led_scale = &ConfigSet1::led_scale;
	IntConfigItem *cycle_time = &ConfigSet1::cycle_time;

	// Extra config values
	ByteConfigItem *show_date = &ConfigSet1::show_date;
	ByteConfigItem *out_effect = &ConfigSet1::out_effect;
	ByteConfigItem *in_effect = &ConfigSet1::in_effect;
	BooleanConfigItem *display = &ConfigSet1::display;
	ByteConfigItem *test_speed = &ConfigSet1::test_speed;
	IntConfigItem *reset_time = &ConfigSet1::reset_time;
	IntConfigItem *set_time = &ConfigSet1::set_time;
	BooleanConfigItem *hv = &ConfigSet1::hv;
	ByteConfigItem *mov_delay = &ConfigSet1::mov_delay;

	// Alexa config values
	StringConfigItem *date_name = &::date_name;
	StringConfigItem *backlight_name = &::backlight_name;
	StringConfigItem *underlight_name = &::underlight_name;
	StringConfigItem *clock_name = &::clock_name;
	StringConfigItem *test_name = &::test_name;
	StringConfigItem *cycling_name = &::cycling_name;
	StringConfigItem *twelve_hour_name = &::twelve_hour_name;
	StringConfigItem *zero_name = &::zero_name;

	void setCurrent(const String &name) {
		if (CurrentConfig::name == name) {
			return;	// Already set to this
		}

		BaseConfigItem *newConfig = rootConfig.get(name.c_str());

		if (newConfig) {
			DEBUG("Changing preset to:");
			DEBUG(name);
			CurrentConfig::name = name;
			config = static_cast<CompositeConfigItem*>(newConfig);

			/*
			 * I hate doing this.
			 */

			// Clock config values
			time_or_date = static_cast<BooleanConfigItem*>(config->get("time_or_date"));
			date_format = static_cast<ByteConfigItem*>(config->get("date_format"));
			hour_format = static_cast<BooleanConfigItem*>(config->get("hour_format"));
			leading_zero = static_cast<BooleanConfigItem*>(config->get("leading_zero"));
			fading = static_cast<ByteConfigItem*>(config->get("fading"));
			colons = static_cast<ByteConfigItem*>(config->get("colons"));
			display_on = static_cast<ByteConfigItem*>(config->get("display_on"));
			display_off = static_cast<ByteConfigItem*>(config->get("display_off"));
			time_url = static_cast<StringConfigItem*>(config->get("time_url"));

			// LED config values
			hue = static_cast<ByteConfigItem*>(config->get("hue"));
			saturation = static_cast<ByteConfigItem*>(config->get("saturation"));
			backlight = static_cast<BooleanConfigItem*>(config->get("backlight"));
			underlight = static_cast<BooleanConfigItem*>(config->get("underlight"));
			hue_cycling = static_cast<BooleanConfigItem*>(config->get("hue_cycling"));
			led_scale = static_cast<ByteConfigItem*>(config->get("led_scale"));
			cycle_time = static_cast<IntConfigItem*>(config->get("cycle_time"));

			// Extra config values
			show_date = static_cast<ByteConfigItem*>(config->get("show_date"));
			out_effect = static_cast<ByteConfigItem*>(config->get("out_effect"));
			in_effect = static_cast<ByteConfigItem*>(config->get("in_effect"));
			display = static_cast<BooleanConfigItem*>(config->get("display"));
			test_speed = static_cast<ByteConfigItem*>(config->get("test_speed"));
			reset_time = static_cast<IntConfigItem*>(config->get("reset_time"));
			set_time = static_cast<IntConfigItem*>(config->get("set_time"));
			hv = static_cast<BooleanConfigItem*>(config->get("hv"));
			mov_delay = static_cast<ByteConfigItem*>(config->get("mov_delay"));

			BaseConfigItem *currentSetName = rootConfig.get("current_set");
			currentSetName->fromString(name);
			currentSetName->put();
		}
	}
}

MovementSensor mov(MovPin);

class ITS1ANixieDriverConfigurator : Configurator {
public:
	ITS1ANixieDriverConfigurator(ITS1ANixieDriver &driver) : driver(driver) {

	}

	virtual void configure() {
		driver.setIndicator(*CurrentConfig::colons);
		driver.setResetTime(*CurrentConfig::reset_time);
		driver.setSetTime(*CurrentConfig::set_time);
	}

private:
	ITS1ANixieDriver &driver;
};

class SixNixieClockConfigurator : Configurator {
public:
	SixNixieClockConfigurator(SixNixieClock &clock) : clock(clock) {
	}

	virtual void configure() {
		if (timeInitialized || !*CurrentConfig::display) {
			clock.setClockMode(*CurrentConfig::display);
			clock.setCountSpeed(*CurrentConfig::test_speed);
		} else {
			clock.setClockMode(false);
			clock.setCountSpeed(60);
		}

		clock.setHV(*CurrentConfig::hv);
		clock.setMov(mov.isOn());
		clock.setFadeMode(*CurrentConfig::fading);
		clock.setTimeMode(*CurrentConfig::time_or_date);
		clock.setDateFormat(*CurrentConfig::date_format);
		clock.set12hour(*CurrentConfig::hour_format);
		clock.setLeadingZero(*CurrentConfig::leading_zero);
		clock.setOnOff(*CurrentConfig::display_on, *CurrentConfig::display_off);
		clock.setAlternateInterval(*CurrentConfig::show_date);
		clock.setOutEffect(*CurrentConfig::out_effect);
		clock.setInEffect(*CurrentConfig::in_effect);
	}

private:
	SixNixieClock &clock;
};

ITS1ANixieDriverConfigurator driverConfigurator(nixieDriver);
SixNixieClockConfigurator clockConfigurator(sixNixieClock);

void initClock() {
	pDriver->init();
	pNixieClock->setNixieDriver(pDriver);
	pNixieClock->init();
}

void grabInts(String s, int *dest, String sep) {
	int end = 0;
	for (int start = 0; end != -1; start = end + 1) {
		end = s.indexOf(sep, start);
		if (end > 0) {
			*dest++ = s.substring(start, end).toInt();
		} else {
			*dest++ = s.substring(start).toInt();
		}
	}
}

void grabBytes(String s, byte *dest, String sep) {
	int end = 0;
	for (int start = 0; end != -1; start = end + 1) {
		end = s.indexOf(sep, start);
		if (end > 0) {
			*dest++ = s.substring(start, end).toInt();
		} else {
			*dest++ = s.substring(start).toInt();
		}
	}
}

void readTimeFailed(String msg) {
	DEBUG(msg);
}

#define SYNC_HOURS 3
#define SYNC_MINS 4
#define SYNC_SECS 5
#define SYNC_DAY 2
#define SYNC_MONTH 1
#define SYNC_YEAR 0

void setTimeFromInternet() {
	String body = httpClient.getBody();
	DEBUG(String("Got response") + body);
	int intValues[6];
	grabInts(body, &intValues[0], ",");

	timeInitialized = true;
    setTime(intValues[SYNC_HOURS], intValues[SYNC_MINS], intValues[SYNC_SECS], intValues[SYNC_DAY], intValues[SYNC_MONTH], intValues[SYNC_YEAR]);
}

void setTimeFromWifiManager() {
	static String oldWifiTime = "";

	const String &wifiTime = wifiManager.getWifiTime();
	if (wifiTime != oldWifiTime) {
		DEBUG(String("Setting time from wifi manager") + wifiTime);
		int intValues[6];
		grabInts(wifiTime, &intValues[0], ",");

		timeInitialized = true;
		oldWifiTime = wifiTime;
		setTime(intValues[SYNC_HOURS], intValues[SYNC_MINS], intValues[SYNC_SECS], intValues[SYNC_DAY], intValues[SYNC_MONTH], intValues[SYNC_YEAR]);
	}
}

const byte numLEDs = 10;
#define LED_PIN 1
LEDRGB leds(numLEDs, LED_PIN);

void ledDisplay(bool backLight=true, bool underLight=true) {
	// Scale normalized brightness to range 0..255
	static byte brightness = 255;
	if (!backLight && !underLight) {
		if (brightness == 0) {
			return;
		} else {
			brightness = 0;
		}
	} else {
		if (brightness == 0) {
			pinMode(LED_PIN, OUTPUT);
			digitalWrite(LED_PIN, LOW);
		}
		brightness = *CurrentConfig::led_scale;
	}

	if (!backLight) {
		leds.setLedColorHSV(*CurrentConfig::hue, *CurrentConfig::saturation, 0);
	} else {
		leds.setLedColorHSV(*CurrentConfig::hue, *CurrentConfig::saturation, brightness);
	}

	for (int i=0; i<6; i++) {
		leds.ledDisplay(i);
	}

	if (!underLight) {
		leds.setLedColorHSV(*CurrentConfig::hue, *CurrentConfig::saturation, 0);
	} else {
		leds.setLedColorHSV(*CurrentConfig::hue, *CurrentConfig::saturation, brightness);
	}

	for (int i=6; i<10; i++) {
		leds.ledDisplay(i);
	}

	leds.show();

	if (brightness == 0) {
		pinMode(LED_PIN, INPUT);
//		digitalWrite(LED_PIN, HIGH);
	}
}

void ledTimerHandler() {
	ledDisplay(*CurrentConfig::backlight, *CurrentConfig::underlight);
	if (*CurrentConfig::hue_cycling) {
		broadcastUpdate(*CurrentConfig::hue);
		*CurrentConfig::hue = (*CurrentConfig::hue + 1) % 256;
	}
}

AsyncWiFiManagerParameter *hostnameParam;

void initFromEEPROM() {
//	config.setDebugPrint(debugPrint);
	config.init();
//	rootConfig.debug(debugPrint);
	DEBUG(hostName);
	rootConfig.get();	// Read all of the config values from EEPROM
	String currentSetName = currentSet;
	CurrentConfig::setCurrent(currentSetName);
	DEBUG(hostName);

	hostnameParam = new AsyncWiFiManagerParameter("Hostname", "clock host name", hostName.value.c_str(), 63);
}

void createSSID() {
	// Create a unique SSID that includes the hostname. Max SSID length is 32!
	ssid = (chipId + hostName).substring(0, 31);
}

void getTime() {
	if (WiFi.status() == WL_CONNECTED) {
		httpClient.makeRequest(setTimeFromInternet, readTimeFailed);
	}
}

void StartOTA() {
#ifdef OTA
	// Port defaults to 8266
	ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	ArduinoOTA.setHostname(((String)hostName).c_str());

	// No authentication by default
//	ArduinoOTA.setPassword("in14");

	ArduinoOTA.onStart([]() {DEBUG("OTA Start");});
	ArduinoOTA.onEnd([]() {DEBUG("\nOTA End");});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		DEBUG("OTA Progress: ");DEBUG(progress / (total / 100));DEBUG("\r");
	});
	ArduinoOTA.onError([](ota_error_t error) {
		DEBUG("OTA Error:")
		switch (error) {
			case OTA_AUTH_ERROR: DEBUG("Auth Failed"); break;
			case OTA_BEGIN_ERROR: DEBUG("Begin Failed"); break;
			case OTA_CONNECT_ERROR: DEBUG("Connect Failed"); break;
			case OTA_RECEIVE_ERROR: DEBUG("Receive Failed"); break;
			case OTA_END_ERROR: DEBUG("End Failed"); break;
		}
	});

	ArduinoOTA.begin();
#endif //OTA
}

void mainHandler(AsyncWebServerRequest *request) {
	DEBUG("Got request")
	request->send(SPIFFS, "/index.html");
}

void sendFavicon(AsyncWebServerRequest *request) {
	DEBUG("Got favicon request")
	request->send(SPIFFS, "/assets/favicon-32x32.png", "image/png");
}

void broadcastUpdate(const BaseConfigItem& item) {
	const size_t bufferSize = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(1);
	DynamicJsonBuffer jsonBuffer(bufferSize);

	JsonObject& root = jsonBuffer.createObject();
	root["type"] = "sv.update";

	JsonObject& value = root.createNestedObject("value");
	String rawJSON = item.toJSON();	// This object needs to hang around until we are done serializing.
	value[item.name] = ArduinoJson::RawJson(rawJSON.c_str());

//	root.printTo(*debugPrint);

    size_t len = root.measureLength();
    AsyncWebSocketMessageBuffer * buffer = ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
    if (buffer) {
        root.printTo((char *)buffer->get(), len + 1);
    	ws.textAll(buffer);
    }
}

WSConfigHandler wsClockHandler(rootConfig, "clock");
WSConfigHandler wsLEDHandler(rootConfig, "leds");
WSConfigHandler wsExtraHandler(rootConfig, "extra");
WSGlobalConfigHandler wsAlexaHandler(rootConfig, "alexa");
WSPresetValuesHandler wsPresetValuesHandler(rootConfig);
WSInfoHandler wsInfoHandler(ssid);
WSPresetNamesHandler wsPresetNamesHandler(rootConfig);

String *items[] = {
	&WSMenuHandler::clockMenu,
	&WSMenuHandler::ledsMenu,
	&WSMenuHandler::extraMenu,
	&WSMenuHandler::alexaMenu,
	&WSMenuHandler::presetsMenu,
	&WSMenuHandler::infoMenu,
	&WSMenuHandler::presetNamesMenu,
	0
};

WSMenuHandler wsMenuHandler(items);

WSHandler *wsHandlers[] = {
	&wsMenuHandler,
	&wsClockHandler,
	&wsLEDHandler,
	&wsExtraHandler,
	&wsPresetValuesHandler,
	&wsInfoHandler,
	&wsPresetNamesHandler,
	&wsAlexaHandler
};

void updateValue(int screen, String pair) {
	int index = pair.indexOf(':');
	DEBUG(pair)
	// _key has to hang around because key points to an internal data structure
	String _key = pair.substring(0, index);
	const char* key = _key.c_str();
	String value = pair.substring(index+1);
	if (screen == 4) {	// Presets
		CurrentConfig::setCurrent(value);
		StringConfigItem temp(key, 10, value);
		broadcastUpdate(temp);
	} else if (screen == 6) {	// Preset names
		BaseConfigItem *item = rootConfig.get(key);
		if (item != 0) {
			item->fromString(value);
			item->put();
			broadcastUpdate(*item);
		}
	} else if (screen == 7) {	// Alexa switch names
		BaseConfigItem *item = rootConfig.get(key);
		if (item != 0) {
#ifdef ALEXA
			StringConfigItem *sItem = static_cast<StringConfigItem*>(item);
			int deviceId = fauxmo.getDeviceId(sItem->value.c_str());
			if (deviceId > 0) {
				fauxmo.renameDevice(deviceId, value.c_str());
			}
			item->fromString(value);
			item->put();
			broadcastUpdate(*item);
#endif
		}
	} else {
		BaseConfigItem *item = CurrentConfig::config->get(key);
		if (item != 0) {
			item->fromString(value);
			item->put();
			// Shouldn't special case this stuff. Should attach listeners to the config value!
			// TODO: This won't work if we just switch change sets instead!
#ifndef USE_NTP
			if (strcmp(key, CurrentConfig::time_url->name) == 0) {
				httpClient.initialize(value);
				getTime();
			}
#endif
			broadcastUpdate(*item);
		}
	}
}

/*
 * Handle application protocol
 */
void handleWSMsg(AsyncWebSocketClient *client, char *data) {
	String wholeMsg(data);
	int code = wholeMsg.substring(0, wholeMsg.indexOf(':')).toInt();

	if (code < 9) {
		wsHandlers[code]->handle(client, data);
	} else {
		String message = wholeMsg.substring(wholeMsg.indexOf(':')+1);
		int screen = message.substring(0, message.indexOf(':')).toInt();
		String pair = message.substring(message.indexOf(':')+1);
		updateValue(screen, pair);
	}
}

/*
 * Handle transport protocol
 */
void wsHandler(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	//Handle WebSocket event
	switch (type) {
	case WS_EVT_CONNECT:
		DEBUG("WS connected")
		;
		break;
	case WS_EVT_DISCONNECT:
		DEBUG("WS disconnected")
		;
		break;
	case WS_EVT_ERROR:
		DEBUG("WS error")
		;
		DEBUG((char* )data)
		;
		break;
	case WS_EVT_PONG:
		DEBUG("WS pong")
		;
		break;
	case WS_EVT_DATA:	// Yay we got something!
		DEBUG("WS data")
		;
		AwsFrameInfo * info = (AwsFrameInfo*) arg;
		if (info->final && info->index == 0 && info->len == len) {
			//the whole message is in a single frame and we got all of it's data
			if (info->opcode == WS_TEXT) {
				DEBUG("WS text data");
				data[len] = 0;
				handleWSMsg(client, (char *) data);
			} else {
				DEBUG("WS binary data");
			}
		} else {
			DEBUG("WS data was split up!");
		}
		break;
	}
}

void eepromUpdate() {
	config.commit();
}

void snoozeUpdate();

void startFauxMo() {
	fauxmo.enable(true);
	fauxmo.addDevice(CurrentConfig::date_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::backlight_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::underlight_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::clock_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::test_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::cycling_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::twelve_hour_name->value.c_str());
	fauxmo.addDevice(CurrentConfig::zero_name->value.c_str());

    fauxmo.onSetState([](unsigned char device_id, const char *device_name, bool state) {
    	BooleanConfigItem *item = 0;
    	switch (device_id) {
    	case 0:
    		item = &(*CurrentConfig::time_or_date = !state);
    		break;
    	case 1:
    		item = &(*CurrentConfig::backlight = state);
    		break;
    	case 2:
    		item = &(*CurrentConfig::underlight = state);
    		break;
    	case 3:
    		item = &(*CurrentConfig::hv = state);
    		break;
    	case 4:
    		item = &(*CurrentConfig::display = !state);
    		break;
    	case 5:
    		item = &(*CurrentConfig::hue_cycling = state);
    		break;
    	case 6:
    		item = &(*CurrentConfig::hour_format = state);
    		break;
    	case 7:
    		item = &(*CurrentConfig::leading_zero = state);
    		break;
    	}

    	if (item != 0) {
    		item->put();
    	}
    	broadcastUpdate(*item);
    });
    fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
    	bool ret = false;
    	switch (device_id) {
    	case 0:
    		ret = *CurrentConfig::time_or_date;
    		ret = !ret;
    		break;
    	case 1:
    		ret = *CurrentConfig::backlight;
    		break;
    	case 2:
    		ret = *CurrentConfig::underlight;
    		break;
    	case 3:
    		ret = *CurrentConfig::hv;
    		break;
    	case 4:
    		ret = *CurrentConfig::display;
    		ret = !ret;
    		break;
    	case 5:
    		ret = *CurrentConfig::hue_cycling;
    		break;
    	case 6:
    		ret = *CurrentConfig::hour_format;
    		break;
    	case 7:
    		ret = *CurrentConfig::leading_zero;
    		break;
    	}

    	return ret;
    });
}

void SetupServer() {
	DEBUG("SetupServer()");
	hostName = String(hostnameParam->getValue());
	hostName.put();
	config.commit();
	DEBUG(hostName.value);
	MDNS.begin(hostName.value.c_str());
    MDNS.addService("http", "tcp", 80);
	StartOTA();

	getTime();

	startFauxMo();
}

SoftMSTimer::TimerInfo syncTimeTimer = {
		3600000,	// 1 hour between syncs
		0,
		true,
		getTime
};

SoftMSTimer::TimerInfo ledTimer = {
		60000,
		0,
		true,
		ledTimerHandler
};

SoftMSTimer::TimerInfo eepromUpdateTimer = {
		60000,
		0,
		true,
		eepromUpdate
};

SoftMSTimer::TimerInfo *infos[] = {
		&syncTimeTimer,
		&ledTimer,
		&eepromUpdateTimer,
		0
};

SoftMSTimer timedFunctions(infos);

void setup()
{
	pinMode(MovPin, FUNCTION_3);
	pinMode(MovPin, INPUT_PULLUP);

	chipId.toUpperCase();
//	Serial.begin(921600);
//	Serial.begin(115200);

	EEPROM.begin(1024);
	SPIFFS.begin();

	initFromEEPROM();

	// Enable LEDs
	leds.begin();
	ledDisplay(*CurrentConfig::backlight, *CurrentConfig::underlight);

	initClock();

	createSSID();

	server.serveStatic("/", SPIFFS, "/");
	server.on("/", HTTP_GET, mainHandler).setFilter(ON_STA_FILTER);
	server.on("/assets/favicon-32x32.png", HTTP_GET, sendFavicon);
	server.serveStatic("/assets", SPIFFS, "/assets");

	// attach AsyncWebSocket
	ws.onEvent(wsHandler);
	server.addHandler(&ws);
	server.begin();
	ws.enable(true);

	DEBUG("Set wifiManager")
	wifiManager.setDebugOutput(true);
	wifiManager.setConnectTimeout(10);
	wifiManager.addParameter(hostnameParam);
	wifiManager.setSaveConfigCallback(SetupServer);
    wifiManager.startConfigPortalModeless(ssid.c_str(), "secretsauce");

	httpClient.initialize(*CurrentConfig::time_url);
	getTime();

	nowMs = millis();

	mov.setDelay(1);
	mov.setOnTime(nowMs);

	DEBUG("Exit setup")
}

int r = 256;
unsigned long nextMs = 0;

void loop()
{
#ifdef OTA
	ArduinoOTA.handle();
#endif
	wifiManager.loop();
	setTimeFromWifiManager();
#ifdef ALEXA
	fauxmo.handle();
#endif

	nowMs = millis();

	mov.setDelay(*CurrentConfig::mov_delay);

	driverConfigurator.configure();
	clockConfigurator.configure();

	bool clockOn = pNixieClock->isOn() && mov.isOn();

	pNixieClock->loop(nowMs);

	if ((*CurrentConfig::backlight || *CurrentConfig::underlight) && clockOn) {
		ledTimer.interval = *CurrentConfig::cycle_time * 1000L / 256;
		ledTimer.enabled = true;
	} else {
		if (ledTimer.enabled) {
			ledTimer.enabled = false;
			ledDisplay(false, false);
		}
	}

	timedFunctions.loop();
}
