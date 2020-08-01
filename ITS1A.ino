//#define DEBUG_ITS1A
//#define DEBUG_ALT
#if defined DEBUG_ITS1A //|| defined DEBUG_ALT
#define DEBUG(...) { Serial.println(__VA_ARGS__); }
Print *debugPrint = &Serial;
#else
#define DEBUG(...) { }
#endif

#define ALEXA
// Not enough space in ESP01 for SPIFFS, the app and an upload space.
//#define OTA

#include "Arduino.h"
#include "ConfigItem.h"             // https://github.com/judge2005/Configs
#include "EEPROMConfig.h"           //                  "
#ifdef OTA
#include "ASyncOTAWebUpdate.h"		// https://github.com/judge2005/ASyncOTAWebUpdate
#endif

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>            // https://github.com/me-no-dev/ESPAsyncTCP
#include <ESPAsyncWebServer.h>      // https://github.com/me-no-dev/ESPAsyncWebServer
#include <AsyncJson.h>              //                 "
#include <ESPAsyncDNSServer.h>		// https://github.com/devyte/ESPAsyncDNSServer
#include <ESPAsyncWiFiManager.h>	// https://github.com/alanswx/ESPAsyncWiFiManager
#include <DNSServer.h>
#include <NeoPixelBus.h>
#include <EspSNTPTimeSync.h>		// https://github.com/judge2005/TimeSync
#ifdef ALEXA
#include <fauxmoESP.h>              // https://bitbucket.org/judge2005/fauxmoesp
#include <HueColorUtils.h>			//                 "
#endif

#include <ITS1ANixieDriver.h>       // https://github.com/judge2005/NixieDriver
#include <SixNixieClock.h>          //                 "
#include <SoftMSTimer.h>            //                 "
#include <MovementSensor.h>         //                 "

#include <WSHandler.h>              //                 "
#include <WSMenuHandler.h>          //                 "
#include <WSConfigHandler.h>        //                 "
#include <WSGlobalConfigHandler.h>  //                 "
#include <WSPresetValuesHandler.h>  //                 "
#include <WSPresetNamesHandler.h>   //                 "
#include <WSInfoHandler.h>          //                 "

#ifdef DEBUG_ALT
const byte MovPin = 13;	// PIR/Radar etc.
#else
const byte MovPin = 3;	// PIR/Radar etc.
#endif

unsigned long nowMs = 0;

String chipId = String(ESP.getChipId(), HEX);
String ssid = "STC-";

StringConfigItem hostName("hostname", 63, "ITS1AClock");

AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws
AsyncDNSServer dns;
AsyncWiFiManager wifiManager(&server,&dns);
TimeSync *timeSync;

#ifdef OTA
ASyncOTAWebUpdate otaUpdater(Update, "update", "secretsauce");
#endif
#ifdef ALEXA
fauxmoESP fauxmo;
#endif
ITS1ANixieDriver nixieDriver(6);
NixieDriver *pDriver = &nixieDriver;
SixNixieClock sixNixieClock(pDriver);
NixieClock *pNixieClock = &sixNixieClock;

class Configurator {
public:
	virtual void configure() = 0;
};

void infoCallback();

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
	ByteConfigItem *underlight_scale = &ConfigSet1::underlight_scale;
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
			underlight_scale = static_cast<ByteConfigItem*>(config->get("underlight_scale"));
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
BlankTimeMonitor blankingMonitor;

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
		if (timeSync->initialized() || !*CurrentConfig::display) {
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
	pNixieClock->setTimeSync(timeSync);
	pNixieClock->setNixieDriver(pDriver);
	pNixieClock->init();
}

void ledTimerHandler();
SoftMSTimer::TimerInfo ledTimer = {
		60000,
		0,
		false,
		ledTimerHandler
};

SoftMSTimer::TimerInfo eepromUpdateTimer = {
		60000,
		0,
		true,
		eepromUpdate
};

#if defined DEBUG_ITS1A //|| defined DEBUG_ALT
SoftMSTimer::TimerInfo memoryDumpTimer = {
		5000,
		0,
		true,
		memoryDumpHandler
};
#endif

void memoryDumpHandler() {
	DEBUG(ESP.getFreeHeap());
}

void timeHandler(AsyncWebServerRequest *request) {
	DEBUG("Got time request")
	String wifiTime = request->getParam("time", true, false)->value();

	timeSync->setTime(wifiTime);

	request->send(SPIFFS, "/time.html");
}

const byte numLEDs = 10;
NeoPixelBus <NeoGrbFeature, NeoEsp8266Uart0800KbpsMethod> leds(numLEDs);

void ledDisplay(bool backLight=true, bool underLight=true) {
	uint8_t scale = *CurrentConfig::led_scale;
	if (!backLight) {
		scale = 0;
	}
	HsbColor color((byte)(*CurrentConfig::hue)/256.0, (byte)(*CurrentConfig::saturation)/256.0, scale/256.0);

	for (int i=0; i<6; i++) {
		leds.SetPixelColor(i, color);
	}

	scale = *CurrentConfig::underlight_scale;
	if (!underLight) {
		scale = 0;
	}

	color = HsbColor((byte)(*CurrentConfig::hue)/256.0, (byte)(*CurrentConfig::saturation)/256.0, scale/256.0);

	for (int i=6; i<10; i++) {
		leds.SetPixelColor(i, color);
	}

#ifndef DEBUG_ITS1A
	leds.Show();
#endif
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

#ifdef ALEXA
	updateFauxMoValues();
#endif
}

WSConfigHandler wsClockHandler(rootConfig, "clock");
WSConfigHandler wsLEDHandler(rootConfig, "leds");
WSConfigHandler wsExtraHandler(rootConfig, "extra");
WSGlobalConfigHandler wsAlexaHandler(rootConfig, "alexa");
WSPresetValuesHandler wsPresetValuesHandler(rootConfig);
WSInfoHandler wsInfoHandler(infoCallback);
WSPresetNamesHandler wsPresetNamesHandler(rootConfig);

void infoCallback() {
	wsInfoHandler.setSsid(ssid);
	wsInfoHandler.setBlankingMonitor(&blankingMonitor);
	TimeSync::SyncStats &syncStats = timeSync->getStats();

	wsInfoHandler.setFailedCount(syncStats.failedCount);
	wsInfoHandler.setLastFailedMessage(syncStats.lastFailedMessage);
	wsInfoHandler.setLastUpdateTime(syncStats.lastUpdateTime);
}

String *items[] = {
	&WSMenuHandler::clockMenu,
	&WSMenuHandler::ledsMenu,
	&WSMenuHandler::extraMenu,
#ifdef ALEXA
	&WSMenuHandler::alexaMenu,
#endif
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
			if (strcmp(key, CurrentConfig::time_url->name) == 0) {
				timeSync->setTz(value);
				timeSync->sync();
			}
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

#ifdef OTA
void sendUpdateForm(AsyncWebServerRequest *request) {
	request->send(SPIFFS, "/update.html");
}

void sendUpdatingInfo(AsyncResponseStream *response, boolean hasError) {
    response->print("<html><head><meta http-equiv=\"refresh\" content=\"10; url=/\"></head><body>");

    hasError ?
    		response->print("Update failed: please wait while the device reboots") :
    		response->print("Update OK: please wait while the device reboots");

    response->print("</body></html>");
}
#endif

void eepromUpdate() {
	config.commit();
}

void snoozeUpdate();

#ifdef ALEXA
void updateFauxMoValues() {
	fauxmo.setState((unsigned char)0, !(*CurrentConfig::time_or_date), *CurrentConfig::time_or_date ? 0 : 255, 0, 0);

	XYPoint xy = HueColorUtils::HSToXY(
			round(((int)(*CurrentConfig::hue)) * 360.0 / 255.0),
			((double)(*CurrentConfig::saturation)) / 255.0,
			0
			);
	fauxmo.setState(1, *CurrentConfig::backlight, *CurrentConfig::led_scale, xy.x, xy.y);
	fauxmo.setState(2, *CurrentConfig::underlight, *CurrentConfig::underlight_scale, xy.x, xy.y);

	fauxmo.setState(3, *CurrentConfig::hv, *CurrentConfig::hv ? 255 : 0, 0, 0);
	fauxmo.setState(4, !(*CurrentConfig::display), !(*CurrentConfig::display) ? 0 : 255, 0, 0);
	fauxmo.setState(5, *CurrentConfig::hue_cycling, *CurrentConfig::hue_cycling ? 255 : 0, 0, 0);
	fauxmo.setState(6, *CurrentConfig::hour_format, *CurrentConfig::hour_format ? 255 : 0, 0, 0);
	fauxmo.setState(7, *CurrentConfig::leading_zero, *CurrentConfig::leading_zero ? 255 : 0, 0, 0);
}

void startFauxMo() {
    fauxmo.createServer(false);
    fauxmo.setPort(80); // This is required for gen3 devices

    // You have to call enable(true) once you have a WiFi connection
    // You can enable or disable the library at any moment
    // Disabling it will prevent the devices from being discovered and switched
	fauxmo.enable(true);

	fauxmo.addDevice(CurrentConfig::date_name->value.c_str(), "Dimmable Light", "LOM001");
	fauxmo.addDevice(CurrentConfig::backlight_name->value.c_str(), "Extended Color light", "LCT015");
	fauxmo.addDevice(CurrentConfig::underlight_name->value.c_str(), "Extended Color light", "LCT015");
	fauxmo.addDevice(CurrentConfig::clock_name->value.c_str(), "Dimmable Light", "LOM001");
	fauxmo.addDevice(CurrentConfig::test_name->value.c_str(), "Dimmable Light", "LWB004");
	fauxmo.addDevice(CurrentConfig::cycling_name->value.c_str(), "Dimmable Light", "LOM001");
	fauxmo.addDevice(CurrentConfig::twelve_hour_name->value.c_str(), "Dimmable Light", "LOM001");
	fauxmo.addDevice(CurrentConfig::zero_name->value.c_str(), "Dimmable Light", "LOM001");

	updateFauxMoValues();

	// These two callbacks are required for gen1 and gen3 compatibility
    server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
        if (fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), String((char *)data))) {
        	return;
        }
        // Handle any other body request here...
		DEBUG("Passing on: ");
		DEBUG(request->host());
		DEBUG(request->url());
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        String body = (request->hasParam("body", true)) ? request->getParam("body", true)->value() : String();
        if (!fauxmo.process(request->client(), request->method() == HTTP_GET, request->url(), body)) {
        	// re-direct to root, i.e. brute force the captive portal if in AP mode.
        	if (!request->host().equals(request->client()->localIP().toString())) {
				DEBUG(String("Redirecting: ") + request->client()->localIP().toString());
				DEBUG(request->host());
				DEBUG(request->url());
				AsyncWebServerResponse *response = request->beginResponse(302,"text/plain","");
				response->addHeader("Location", String("http://") + request->client()->localIP().toString());
				response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
				response->addHeader("Pragma", "no-cache");
				response->addHeader("Expires", "-1");
				request->send ( response);
			} else {
				DEBUG("Sending not found");
				AsyncWebServerResponse *response = request->beginResponse(404,"text/plain","Not found");
				response->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
				response->addHeader("Pragma", "no-cache");
				response->addHeader("Expires", "-1");
				request->send (response );
			}
        }
    });

    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value, float x, float y) {
    	BooleanConfigItem *item = 0;
    	ByteConfigItem *valueItem = 0;
    	ByteConfigItem *hueItem = 0;
    	ByteConfigItem *saturationItem = 0;
    	switch (device_id) {
    	case 0:
    		item = &(*CurrentConfig::time_or_date = !state);
    		break;
    	case 1:
    		{
				item = &(*CurrentConfig::backlight = state);
				HS hs = HueColorUtils::XYToHS(x, y, 0);
				hueItem = &(*CurrentConfig::hue = (byte)round(hs.h * 255.0 / 360.0));
				saturationItem = &(*CurrentConfig::saturation = (byte)round(hs.s * 255));
				valueItem = &(*CurrentConfig::led_scale = value);
    		}
        	break;
    	case 2:
			{
				item = &(*CurrentConfig::underlight = state);
				HS hs = HueColorUtils::XYToHS(x, y, 0);
				hueItem = &(*CurrentConfig::hue = (byte)round(hs.h * 255.0 / 360.0));
				saturationItem = &(*CurrentConfig::saturation = (byte)round(hs.s * 255));
				valueItem = &(*CurrentConfig::underlight_scale = value);
			}
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
        	broadcastUpdate(*item);
    	}

    	if (hueItem != 0) {
    		hueItem->put();
        	broadcastUpdate(*hueItem);
    	}

    	if (saturationItem != 0) {
    		saturationItem->put();
        	broadcastUpdate(*saturationItem);
    	}

    	if (valueItem != 0) {
    		valueItem->put();
        	broadcastUpdate(*valueItem);
    	}
    });
}
#endif

void SetupServer() {
	DEBUG("SetupServer()");
	hostName = String(hostnameParam->getValue());
	hostName.put();
	config.commit();
	DEBUG(hostName.value);
	if (WiFi.status() == WL_CONNECTED) {
		DEBUG("WiFi connected, setting up responder");
		MDNS.begin(hostName.value.c_str(), WiFi.localIP());
		MDNS.addService("http", "tcp", 80);
		timeSync->init();
	}
}

SoftMSTimer::TimerInfo *infos[] = {
		&ledTimer,
		&eepromUpdateTimer,
#if defined DEBUG_ITS1A //|| defined DEBUG_ALT
		&memoryDumpTimer,
#endif
		0
};

SoftMSTimer timedFunctions(infos);

void setup()
{
#if !defined DEBUG_ITS1A && !defined DEBUG_ALT
	pinMode(MovPin, FUNCTION_3);
//	pinMode(LED_PIN, FUNCTION_3);
#endif

#ifndef DEBUG_ITS1A
	pinMode(MovPin, INPUT_PULLUP);
#endif

	chipId.toUpperCase();
//	Serial.begin(921600);
#if defined DEBUG_ITS1A || defined DEBUG_ALT
	Serial.begin(115200);
#endif

	EEPROM.begin(2048);
	SPIFFS.begin();

	initFromEEPROM();

	timeSync = new EspSNTPTimeSync(*CurrentConfig::time_url, NULL, NULL);

	// Enable LEDs
#ifndef DEBUG_ITS1A
	leds.Begin();
	ledDisplay(*CurrentConfig::backlight, *CurrentConfig::underlight);
#endif

	initClock();

//	dns.setTTL(1);
	WiFi.setSleepMode(WIFI_NONE_SLEEP);
	WiFi.setAutoReconnect(true);

	createSSID();

	DEBUG("Set wifiManager")
#if defined DEBUG_ITS1A //|| defined DEBUG_ALT
	wifiManager.setDebugOutput(true);
#else
	wifiManager.setDebugOutput(false);
#endif

	wifiManager.setCustomOptionsElement("<br><form action='/t' name='time_form' method='post'><button name='time' onClick=\"{var now=new Date();this.value=now.getFullYear()+','+(now.getMonth()+1)+','+now.getDate()+','+now.getHours()+','+now.getMinutes()+','+now.getSeconds();} return true;\">Set Clock Time</button></form><br><form action=\"/app.html\" method=\"get\"><button>Configure Clock</button></form>");
	wifiManager.setConnectTimeout(10);
	wifiManager.addParameter(hostnameParam);
	wifiManager.setSaveConfigCallback(SetupServer);
    wifiManager.startConfigPortalModeless(ssid.c_str(), "secretsauce");

    // This has to be done AFTER the wifiManager setup, because the wifimanager resets
    // the server which causes a crash...
	server.serveStatic("/", SPIFFS, "/");
	server.on("/", HTTP_GET, mainHandler).setFilter(ON_STA_FILTER);
	server.on("/assets/favicon-32x32.png", HTTP_GET, sendFavicon);
	server.serveStatic("/assets", SPIFFS, "/assets");
	server.on("/t", HTTP_POST, timeHandler).setFilter(ON_AP_FILTER);
#ifdef OTA
	otaUpdater.init(server, "/update", sendUpdateForm, sendUpdatingInfo);
#endif

	// attach AsyncWebSocket
	ws.onEvent(wsHandler);
	server.addHandler(&ws);
	server.begin();
	ws.enable(true);

#ifdef ALEXA
	startFauxMo();
#endif

	nowMs = millis();

#ifndef DEBUG_ITS1A
	mov.setDelay(1);
	mov.setOnTime(nowMs);
#endif

	DEBUG("Exit setup")
}

int r = 256;
unsigned long nextMs = 0;

void loop()
{
	if (WiFi.status() == WL_CONNECTED) {
		MDNS.update();
	}
	wifiManager.loop();
#ifdef ALEXA
	fauxmo.handle();
#endif

	nowMs = millis();

	mov.setDelay(*CurrentConfig::mov_delay);

	driverConfigurator.configure();
	clockConfigurator.configure();

	pNixieClock->loop(nowMs);

#ifndef DEBUG_ITS1A
	bool clockOn = pNixieClock->isOn() && mov.isOn();
#else
	bool clockOn = pNixieClock->isOn();
#endif

	blankingMonitor.on(clockOn);

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
