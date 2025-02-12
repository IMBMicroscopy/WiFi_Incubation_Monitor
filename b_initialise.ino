// Include required libraries
#include "esp_mac.h"             // Exposes esp_mac_type_t values
#include <Wire.h>                // I2C communications for CO2 sensor breakout board
#include <SPI.h>                 // SPI interface for TFT display
#include <Adafruit_Sensor.h>     // Adafruit sensor library
#include <Adafruit_GFX.h>        // Core graphics library for TFT
#include <Adafruit_ST7789.h>     // Hardware-specific library for ST7789
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // TFT variable for Adafruit Feather

// Include sensor libraries
#include "Adafruit_MAX1704X.h"   // LiPo battery measurements
Adafruit_MAX17048 maxlipo;       // Battery monitor variable

#include <BME280I2C.h>           // BME280 air pressure sensor library
BME280I2C myBME280;
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

#include "SparkFun_SCD4x_Arduino_Library.h"  // SCD41 CO2 sensor library
SCD4x mySCD41;

#include "SparkFun_STC3x_Arduino_Library.h"  // STC31 high CO2 % sensor library
STC3x mySTC31;

#include "SparkFun_SHTC3.h"  // SHTC3 Temp/RH sensor library
SHTC3 mySHTC3;

// WiFi and IO libraries
#include <WiFiManager.h>
#include <WiFi.h>
#include "AdafruitIO_WiFi.h"
#include "ArduinoJson.h"
#include <LittleFS.h>
#include "FS.h"

// Define constants and parameters
#define FORMAT_LITTLEFS_IF_FAILED true
#define hotspotPin 2  // Pushbutton pin to activate hotspot for WiFi configuration

// WiFi Manager parameters
char IO_USERNAME[64] = "";
char IO_KEY[64] = "";
char IO_Dashboard[64] = "";
char CO2_FeedName[64] = "";
char Temp_FeedName[64] = "";
char RH_FeedName[64] = "";

// Adafruit IO variables
static uint8_t objStorage[sizeof(AdafruitIO_WiFi)];
AdafruitIO_WiFi *io;             // Adafruit IO pointer
AdafruitIO_Feed *CO2_Feed;
AdafruitIO_Feed *Temp_Feed;
AdafruitIO_Feed *RH_Feed;

// Create WiFiManager object and custom parameters
WiFiManager wifiManager;
WiFiManagerParameter custom_IO_USERNAME("iouser", "Adafruit IO Username", IO_USERNAME, 60);
WiFiManagerParameter custom_IO_KEY("iokey", "Adafruit IO Key", IO_KEY, 60);
WiFiManagerParameter custom_IO_Dashboard("iodashboard", "Adafruit IO Feed", IO_Dashboard, 60);

// Constants for time calculations
#define ms_per_hour 3600000
#define ms_per_min  60000
#define ms_per_sec  1000

// Time variables
byte hour, minute, second;
unsigned long now, curr;
unsigned long elapsedSensorTime = 0, lastSensorTime = 0;
unsigned long elapsedCompensateTime = 0, lastCompensateTime = 0;
unsigned long elapsedBatteryTime = 0, lastBatteryTime = 0;
unsigned long elapsedDashboardTime = 0, lastDashboardTime = 0;
unsigned long elapsedWiFiTime = 0, lastWiFiTime = 0;
unsigned long elapsedIOTime = 0, lastIOTime = 0;
unsigned long elapsedTFTTime = 0, lastTFTTime = 0;
unsigned long setupTime = 0;

// Sensor and system variables
int pascals = 0;
int cursorX = 0, cursorY = 0, cursorX0 = 0, cursorY0 = 0;
float battVoltage = 0.00, battPercent = 0.00, battRate = 0.00;
float stcCO2 = 0.00, stcTemp = 0.00, stcRH = 0.00;
float scdCO2 = 0.00, scdTemp = 0.00, scdRH = 0.00;
float bmeTemp(NAN), bmeRH(NAN), bmePress(NAN);
float myCO2 = 0.00, myTemp = 0.00, myRH = 0.00;
long int compCounter = 0, battCounter = 0;

// Flags
bool firstRun = true;
bool compFlag = false;
bool updatingDashboardFlag = false;
bool battExistsFlag = false;
bool BME280Exists = false;
bool wifiConnectedFlag = false;
bool ioConnectedFlag = false;
bool startPortalFlag = false;