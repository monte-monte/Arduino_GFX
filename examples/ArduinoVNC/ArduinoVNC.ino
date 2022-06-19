/*******************************************************************************
 * Arduino VNC
 * This is a simple VNC sample
 * 
 * Dependent libraries:
 * ArduinoVNC: https://github.com/Links2004/arduinoVNC
 *
 * Setup steps:
 * 1. Fill your own SSID_NAME, SSID_PASSWORD, VNC_IP, VNC_PORT and VNC_PASSWORD
 * 2. Change your LCD parameters in Arduino_GFX setting
 ******************************************************************************/

/* WiFi settings */
const char *SSID_NAME = "YourAP";
const char *SSID_PASSWORD = "PleaseInputYourPasswordHere";

const char *VNC_IP = "192.168.12.34";
const uint16_t VNC_PORT = 5901;
const char *VNC_PASSWORD = "PleaseInputYourPasswordHere";

/*******************************************************************************
 * Start of Arduino_GFX setting
 *
 * Arduino_GFX try to find the settings depends on selected board in Arduino IDE
 * Or you can define the display dev kit not in the board list
 * Defalult pin list for non display dev kit:
 * Arduino Nano, Micro and more: CS:  9, DC:  8, RST:  7, BL:  6
 * ESP32 various dev board     : CS:  5, DC: 27, RST: 33, BL: 22
 * ESP32-C3 various dev board  : CS:  7, DC:  2, RST:  1, BL:  3
 * ESP32-S2 various dev board  : CS: 34, DC: 35, RST: 33, BL: 21
 * ESP32-S3 various dev board  : CS: 40, DC: 41, RST: 42, BL: 48
 * ESP8266 various dev board   : CS: 15, DC:  4, RST:  2, BL:  5
 * Raspberry Pi Pico dev board : CS: 17, DC: 27, RST: 26, BL: 28
 * RTL8720 BW16 old patch core : CS: 18, DC: 17, RST:  2, BL: 23
 * RTL8720_BW16 Official core  : CS:  9, DC:  8, RST:  6, BL:  3
 * RTL8722 dev board           : CS: 18, DC: 17, RST: 22, BL: 23
 * RTL8722_mini dev board      : CS: 12, DC: 14, RST: 15, BL: 13
 * Seeeduino XIAO dev board    : CS:  3, DC:  2, RST:  1, BL:  0
 * Teensy 4.1 dev board        : CS: 39, DC: 41, RST: 40, BL: 22
 ******************************************************************************/
#include <Arduino_GFX_Library.h>

#define GFX_BL DF_GFX_BL // default backlight pin, you may replace DF_GFX_BL to actual backlight pin

/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */
#if defined(DISPLAY_DEV_KIT)
Arduino_GFX *gfx = create_default_Arduino_GFX();
#else /* !defined(DISPLAY_DEV_KIT) */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = create_default_Arduino_DataBus();

/* More display class: https://github.com/moononournation/Arduino_GFX/wiki/Display-Class */
Arduino_GFX *gfx = new Arduino_ILI9341(bus, DF_GFX_RST, 1 /* rotation */, false /* IPS */);

#endif /* !defined(DISPLAY_DEV_KIT) */
/*******************************************************************************
 * End of Arduino_GFX setting
 ******************************************************************************/

#if defined(ESP32)
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti WiFiMulti;
#elif defined(RTL8722DM)
#include <WiFi.h>
#endif

#include "VNC_GFX.h"
#include <VNC.h>

VNC_GFX *vnc_gfx = new VNC_GFX((Arduino_TFT *)gfx);
arduinoVNC vnc = arduinoVNC(vnc_gfx);

void TFTnoWifi(void)
{
    gfx->fillScreen(BLACK);
    gfx->setCursor(0, ((gfx->height() / 2) - (5 * 8)));
    gfx->setTextColor(RED);
    gfx->setTextSize(5);
    gfx->println("NO WIFI!");
    gfx->setTextSize(2);
    gfx->println();
}

void TFTnoVNC(void)
{
    gfx->fillScreen(BLACK);
    gfx->setCursor(0, ((gfx->height() / 2) - (4 * 8)));
    gfx->setTextColor(GREEN);
    gfx->setTextSize(4);
    gfx->println("connect VNC");
    gfx->setTextSize(2);
    gfx->println();
    gfx->print(VNC_IP);
    gfx->print(":");
    gfx->println(VNC_PORT);
}

void setup(void)
{
    Serial.begin(115200);
    // while (!Serial);
    // Serial.setDebugOutput(true);
    Serial.println("Arduino VNC");

    Serial.println("Init display");
    gfx->begin();
    gfx->fillScreen(BLACK);

#ifdef GFX_BL
    pinMode(GFX_BL, OUTPUT);
    digitalWrite(GFX_BL, HIGH);
#endif
    TFTnoWifi();

    Serial.println("Init WiFi");
    gfx->println("Init WiFi");
#if defined(ESP32)
    wifiMulti.addAP(SSID_NAME, SSID_PASSWORD);
    while ((wifiMulti.run() == WL_CONNECTED))
#elif defined(ESP8266)
    // disable sleep mode for better data rate
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(SSID_NAME, SSID_PASSWORD);
    while ((wifiMulti.run() == WL_CONNECTED))
#elif defined(RTL8722DM)
    WiFi.begin(SSID_NAME, SSID_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
#endif
    {
        delay(500);
        Serial.print(".");
        gfx->print(".");
    }
    Serial.println(" CONNECTED");
    gfx->println(" CONNECTED");
    Serial.println("IP address: ");
    gfx->println("IP address: ");
    Serial.println(WiFi.localIP());
    gfx->println(WiFi.localIP());
    TFTnoVNC();

    Serial.println(F("[SETUP] VNC..."));

    vnc.begin(VNC_IP, VNC_PORT);
    vnc.setPassword(VNC_PASSWORD); // optional
}

void loop()
{
#if defined(ESP32)
    if ((wifiMulti.run() != WL_CONNECTED))
#elif defined(ESP8266)
    if ((WiFiMulti.run() != WL_CONNECTED))
#elif defined(RTL8722DM)
    if (WiFi.begin(SSID_NAME, SSID_PASSWORD) != WL_CONNECTED)
#endif
    {
        vnc.reconnect();
        TFTnoWifi();
        delay(100);
    }
    else
    {
        vnc.loop();
        if (!vnc.connected())
        {
            TFTnoVNC();
            // some delay to not flood the server
            delay(5000);
        }
    }
}
