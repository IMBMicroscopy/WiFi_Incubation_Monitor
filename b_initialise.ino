// Include required libraries
#include "esp_mac.h"             // Exposes esp_mac_type_t values
#include <cstring>               // For strcmp, strcpy, strcasecmp
#include <Wire.h>                // I2C communications for CO2 sensor breakout board
#include <SPI.h>                 // SPI interface for TFT display
#include <Adafruit_Sensor.h>     // Adafruit sensor library
#include <Adafruit_GFX.h>        // Core graphics library for TFT
#include <Adafruit_ST7789.h>     // Hardware-specific library for ST7789
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST); // TFT variable for Adafruit Feather

// Include sensor libraries
#include "Adafruit_MAX1704X.h"   // LiPo battery measurements
Adafruit_MAX17048 maxlipo;       // Battery monitor variable

#include <Adafruit_BME280.h>
Adafruit_BME280 myBME280;

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

// save parameters to preferences file
#include <Preferences.h>
Preferences preferences;

// Define constants and parameters
#define hotspotPin 2  // Pushbutton pin to activate hotspot for WiFi configuration

// WiFi Manager parameters stored in character array buffers
char IO_USERNAME_buff[64] = "";
char IO_KEY_buff[64] = "";
char IO_Dashboard_buff[64] = "";
char CO2_FeedName_buff[64] = "";
char Temp_FeedName_buff[64] = "";
char RH_FeedName_buff[64] = "";
char roomTemp_FeedName_buff[64] = "";
char roomRH_FeedName_buff[64] = "";
char roomPress_FeedName_buff[64] = "";

//optional configuration parameters to store character array of values for use in WiFiManager
//Featuer Toggles
char low_CO2_Monitor_buff[6] = "";
char high_CO2_Monitor_buff[6] = "";
char pressure_Monitor_buff[6] = "";
char battery_Monitor_buff[6] = "";
char dashboard_Monitor_buff[6] = "";
char room_Monitor_buff[6] = "";

//Sensor Settings
char switchCO2Sensors_buff[8] = "";
char lowCO2_buff[8] = "";
char highCO2_buff[8] = "";
char lowRH_buff[8] = "";
char highRH_buff[8] = "";
char lowTemp_buff[8] = "";
char highTemp_buff[8] = "";
char lowBatt_buff[8] = "";
char sensorRate_buff[8] = "";
char compensateRate_buff[8] = "";
char dashboardRate_buff[8] = "";
char batteryRate_buff[8] = "";
// BME280 calibration
char bme280_offsetTemp_buff[8] = "";
char bme280_calibrateRH_buff[6] = "";
char bme280_high_reference_buff[8] = "";
char bme280_high_reading_buff[8] = "";
char bme280_low_reference_buff[8] = "";
char bme280_low_reading_buff[8] = "";
// SHTC3 calibration
char SHTC3_offsetTemp_buff[8] = "";
char SHTC3_calibrateRH_buff[6] = "";
char SHTC3_high_reference_buff[8] = "";
char SHTC3_high_reading_buff[8] = "";
char SHTC3_low_reference_buff[8] = "";
char SHTC3_low_reading_buff[8] = "";
// SCD41 calibration
char SCD41_offsetTemp_buff[8] = "";
char SCD41_calibrateRH_buff[6] = "";
char SCD41_high_reference_buff[8] = "";
char SCD41_high_reading_buff[8] = "";
char SCD41_low_reference_buff[8] = "";
char SCD41_low_reading_buff[8] = "";

//Additional
char pressure_buff[8] = "";
char baud_buff[8] = "";

// Adafruit IO variables
static uint8_t objStorage[sizeof(AdafruitIO_WiFi)];
AdafruitIO_WiFi *io;        // Adafruit IO pointer
AdafruitIO_Feed *CO2_Feed;  //CO2 data feed to IO dashboard
AdafruitIO_Feed *Temp_Feed; //Temp data feed to IO dashboard
AdafruitIO_Feed *RH_Feed;   //RH data feed to IO dashboard
AdafruitIO_Feed *roomTemp_Feed;   //room Temperature data feed to IO dashboard
AdafruitIO_Feed *roomRH_Feed;   //room RH data feed to IO dashboard
AdafruitIO_Feed *roomPress_Feed;   //room Pressure data feed to IO dashboard

// Create WiFiManager object and custom parameters to display on hotspot website
WiFiManager wifiManager;
WiFiManagerParameter custom_dashboard_title("<p>-------------------IO Dashboard Parameters-------------------</p>");
WiFiManagerParameter custom_IO_USERNAME("io_user", "Adafruit IO Username", IO_USERNAME_buff, 60);
WiFiManagerParameter custom_IO_KEY("io_key", "Adafruit IO Key", IO_KEY_buff, 60);
WiFiManagerParameter custom_IO_Dashboard("io_dashboard", "Adafruit IO Dashboard Short Name", IO_Dashboard_buff, 60);

// Optional WiFiManager parameters
WiFiManagerParameter custom_monitor_title("<p>-------------------Feature Toggles-------------------</p>");
WiFiManagerParameter custom_low_CO2_Monitor("low_CO2_Monitor", "low CO2 monitor (true/false)", low_CO2_Monitor_buff, 6);
WiFiManagerParameter custom_high_CO2_Monitor("high_CO2_Monitor", "high CO2 monitor (true/false)", high_CO2_Monitor_buff, 6);
WiFiManagerParameter custom_pressure_Monitor("pressure_Monitor", "pressure monitor (true/false)", pressure_Monitor_buff, 6);
WiFiManagerParameter custom_battery_Monitor("battery_Monitor", "battery monitor (true/false)", battery_Monitor_buff, 6);
WiFiManagerParameter custom_dashboard_Monitor("dashboard_Monitor", "dashboard monitor (true/false)", dashboard_Monitor_buff, 6);
WiFiManagerParameter custom_room_Monitor("room_Monitor", "Room monitor (true/false)", room_Monitor_buff, 6);

WiFiManagerParameter custom_sensor_title("<p>----------------------Sensor Parameters----------------------</p>");
WiFiManagerParameter custom_switch_CO2_Sensors("switch_CO2_Sensors", "switch between CO2 sensors (%)", switchCO2Sensors_buff, 8);
WiFiManagerParameter custom_lowCO2("low_CO2", "low CO2 alert (%)", lowCO2_buff, 8);
WiFiManagerParameter custom_highCO2("high_CO2", "high CO2 alert (%)", highCO2_buff, 8);
WiFiManagerParameter custom_lowRH("low_RH", "low RH alert (%)", lowRH_buff, 8);
WiFiManagerParameter custom_highRH("high_RH", "high RH alert (%)", highRH_buff, 8);
WiFiManagerParameter custom_lowTemp("low_Temp", "low Temp alert (C)", lowTemp_buff, 8);
WiFiManagerParameter custom_highTemp("high_Temp", "high Temp alert (C)", highTemp_buff, 8);
WiFiManagerParameter custom_lowBatt("low_Batt", "low Battery alert (%)", lowBatt_buff, 8);

WiFiManagerParameter custom_update_title("<p>-------------------- Update Rates ---------------------</p>");
WiFiManagerParameter custom_sensorRate("sensor_Rate", "sensor readout rate (s)", sensorRate_buff, 8);
WiFiManagerParameter custom_compensateRate("compensate_Rate", "compensate sensor Rate (s)", compensateRate_buff, 8);
WiFiManagerParameter custom_dashboardRate("dashboard_Rate", "dashboard update Rate (s)", dashboardRate_buff, 8);
WiFiManagerParameter custom_batteryRate("battery_Rate", "battery BMS readout rate (s)", batteryRate_buff, 8);

// BME280 WiFiManager Parameters
WiFiManagerParameter custom_bme280_title("<p>---------------------- BME280 Calibration ----------------------</p>");
WiFiManagerParameter custom_bme280_offsetTemp("bme280_offsetTemp", "Offset BME280 temperature to match other sensors", bme280_offsetTemp_buff, 8);
WiFiManagerParameter custom_bme280_calibrateRH("bme280_calibrateRH", "Apply RH calibration values below (true/false)", bme280_calibrateRH_buff, 6);
WiFiManagerParameter custom_bme280_high_reference("bme280_high_reference", "HIGH reference humidity chamber value (70-100%)", bme280_high_reference_buff, 8);
WiFiManagerParameter custom_bme280_high_reading("bme280_high_reading", "BME280 measured HIGH reference value", bme280_high_reading_buff, 8);
WiFiManagerParameter custom_bme280_low_reference("bme280_low_reference", "LOW reference humidity chamber value (10-30%)", bme280_low_reference_buff, 8);
WiFiManagerParameter custom_bme280_low_reading("bme280_low_reading", "BME280 measured LOW reference value", bme280_low_reading_buff, 8);

// SHTC3 WiFiManager Parameters
WiFiManagerParameter custom_SHTC3_title("<p>---------------------- SHTC3 Calibration ----------------------</p>");
WiFiManagerParameter custom_SHTC3_offsetTemp("SHTC3_offsetTemp", "Offset SHTC3 temperature to match other sensors", SHTC3_offsetTemp_buff, 8);
WiFiManagerParameter custom_SHTC3_calibrateRH("SHTC3_calibrateRH", "Apply RH calibration values below (true/false)", SHTC3_calibrateRH_buff, 6);
WiFiManagerParameter custom_SHTC3_high_reference("SHTC3_high_reference", "HIGH reference humidity chamber value (70-100%)", SHTC3_high_reference_buff, 8);
WiFiManagerParameter custom_SHTC3_high_reading("SHTC3_high_reading", "SHTC3 measured HIGH reference value", SHTC3_high_reading_buff, 8);
WiFiManagerParameter custom_SHTC3_low_reference("SHTC3_low_reference", "LOW reference humidity chamber value (10-30%)", SHTC3_low_reference_buff, 8);
WiFiManagerParameter custom_SHTC3_low_reading("SHTC3_low_reading", "SHTC3 measured LOW reference value", SHTC3_low_reading_buff, 8);

// SCD41 WiFiManager Parameters
WiFiManagerParameter custom_SCD41_title("<p>---------------------- SCD41 Calibration ----------------------</p>");
WiFiManagerParameter custom_SCD41_offsetTemp("SCD41_offsetTemp", "Offset SCD41 temperature to match other sensors", SCD41_offsetTemp_buff, 8);
WiFiManagerParameter custom_SCD41_calibrateRH("SCD41_calibrateRH", "Apply RH calibration values below (true/false)", SCD41_calibrateRH_buff, 6);
WiFiManagerParameter custom_SCD41_high_reference("SCD41_high_reference", "HIGH reference humidity chamber value (70-100%)", SCD41_high_reference_buff, 8);
WiFiManagerParameter custom_SCD41_high_reading("SCD41_high_reading", "SCD41 measured HIGH reference value", SCD41_high_reading_buff, 8);
WiFiManagerParameter custom_SCD41_low_reference("SCD41_low_reference", "LOW reference humidity chamber value (10-30%)", SCD41_low_reference_buff, 8);
WiFiManagerParameter custom_SCD41_low_reading("SCD41_low_reading", "SCD41 measured LOW reference value", SCD41_low_reading_buff, 8);

WiFiManagerParameter custom_Additional_title("<p>-------------------- Additional Parameters ---------------------</p>");
WiFiManagerParameter custom_pressure("pressure", "manual pressure value (mbar)", pressure_buff, 8);
WiFiManagerParameter custom_baud("baud", "USB baud rate (115200 default)", baud_buff, 8);

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
float stcCO2 = 0.00, stcTemp = 0.00, stcRH = 0.00, stcCRH = 0.00;
float scdCO2 = 0.00, scdTemp = 0.00, scdRH = 0.00, scdCRH = 0.00;
float bmeTemp = 0.00, bmeRH = 0.00, bmePress = 0.00, bmeCRH = 0.00;
float myCO2 = 0.00, myTemp = 0.00, myRH = 0.00, myPress = 0.00;
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
bool writeConfigValues = false;
bool shouldSaveConfig = false;
bool saveIO = false;
