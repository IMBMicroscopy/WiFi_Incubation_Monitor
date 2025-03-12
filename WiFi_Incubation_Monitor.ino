/*********************************************************************
 * User-Definable default Settings for the Incubation Monitor
 * These settings are the default settings installed onto the ESP32 device at first compile
 * and used on first load of the configuration hotspot, but may be altered and saved to flash from the hotspot
 * Adjust parameters as required
 *********************************************************************/

// -------------------------------------------------------------------
// Feature Toggles
// Set to false if you haven't installed the hardware or don't require the feature.
// -------------------------------------------------------------------
bool low_CO2_Monitor    = true; // Optional SCD41 sensor for measuring low ambient levels of CO2 (<1%)
bool high_CO2_Monitor   = true; // STC31 sensor (0.7 - 100% CO2) attached?
bool pressure_Monitor   = true; // Optional BME280 sensor for measuring ambient pressure and room conditions (T and RH)
bool battery_Monitor    = true; // Monitor battery charge if a battery is connected
bool dashboard_Monitor  = true; // Enable/disable online dashboard reporting (requires WiFi or Ethernet)
bool room_Monitor       = true; // if pressure_Monitor is true, Enable/disable reporting of T, RH and Pressure values from bme280 installed in the display case
// -------------------------------------------------------------------
// CO2 Sensor Configuration
// -------------------------------------------------------------------
float switchCO2Sensors = 0.7; // Changeover from low-CO2 (SCD41) to high-CO2 (STC31) sensor. 
// If STC31 reading > 0.7%, use STC31; otherwise, use SCD41.
// the STC31 doesnt record below ~. 0.7% CO2
// ignored if low_CO2_Monitor = false

// -------------------------------------------------------------------
// bme280 calibration
// -------------------------------------------------------------------
float offsetTemp = -1;  //offset bme280 temperature to match other sensors

bool calibrateRH = true;  //Optionally: Use a humidity controlled environment to calibrate the bme280 Humidity Sensor
//Calibration standards such as salt media (NaCL and MgCL) or an electronically controlled humidity chamber can be used
//Place saturated salt solution (salt + a little water) in an airtight container.
//75% RH (NaCl) and 33% RH (MgCl₂)
//Place sensor in chamber, wait 24hrs or until value plateaus before recording measurement
float high_Standard = 76;   // High reference humidity chamber value (70-100%) 
float high_bme280 = 64;     // BME280 measured value when sensor is measuring in high_Standard value chamber
float low_Standard = 14;    // Low reference humidity chamber value (10-30%) 
float low_bme280 = 10;      // BME280 measured value when sensor is measuring in low_Standard value chamber

// -------------------------------------------------------------------
// Measurement Limits (Incubation Settings)
// Below or above these thresholds, an alarm condition (e.g., TFT color change) occurs
// -------------------------------------------------------------------
float lowCO2   = 4.0;   // CO2 % value BELOW this triggers an alarm
float highCO2  = 6.0;   // CO2 % value ABOVE this triggers an alarm
float lowRH    = 40;  // Relative humidity % BELOW this triggers an alarm
float highRH   = 100; // Relative humidity % ABOVE this triggers an alarm
float lowTemp  = 35;  // Temperature (°C) BELOW this triggers an alarm
float highTemp = 38;  // Temperature (°C) ABOVE this triggers an alarm

// -------------------------------------------------------------------
// Battery Settings
// -------------------------------------------------------------------
float lowBatt = 20; // Battery State of Charge (SOC) in % BELOW this triggers an alarm (changes colour on the display)

// -------------------------------------------------------------------
// Measurement Rates (in seconds) (values will be constrained automatically in code as per requirements below)
// -------------------------------------------------------------------
int sensorRate      = 15;  // Read sensors and update TFT every X seconds (>=1)
int compensateRate  = 15;  // Recalibrate CO2 sensor (T, RH, Pressure) every X seconds (>= sensorRate)
int dashboardRate   = 60;  // Update the online dashboard every X seconds (>= sensorRate)
int batteryRate     = 300; // Check BMS hibernation status and wake if needed (>= sensorRate)

// -------------------------------------------------------------------
// bme280 calibration (in case for room monitoring)
// -------------------------------------------------------------------
float bme280_offsetTemp = 0;  //offset bme280 temperature to match other sensors

bool bme280_calibrateRH = false;  //Optionally: Use a humidity controlled environment to calibrate the bme280 Humidity Sensor
//Calibration standards such as salt media (NaCL and MgCL) or an electronically controlled humidity chamber can be used
//Place saturated salt solution (salt + a little water) in an airtight container.
//75% RH (NaCl) and 33% RH (MgCl₂)
//Place sensor in chamber, wait 24hrs or until value plateaus before recording measurement
float bme280_high_reference = 76;   // High reference humidity chamber value (70-100%) or validated sensor reading 
float bme280_high_reading = 64;     // BME280 measured value when sensor is measuring in high_reference value chamber
float bme280_low_reference = 14;    // Low reference humidity chamber value (10-30%) or validated sensor reading
float bme280_low_reading = 10;      // BME280 measured value when sensor is measuring in low_reference value chamber

// -------------------------------------------------------------------
// SHTC3 calibration (on High CO2 STC31 sensor board)
// -------------------------------------------------------------------
float SHTC3_offsetTemp = 0;  // Offset SHTC3 temperature to match other sensors

bool SHTC3_calibrateRH = false;  // Optionally: Use a humidity-controlled environment to calibrate the SHTC3 Humidity Sensor
// Calibration standards such as salt media (NaCl and MgCl₂) or an electronically controlled humidity chamber can be used
// Place saturated salt solution (salt + a little water) in an airtight container.
// 75% RH (NaCl) and 33% RH (MgCl₂)
// Place sensor in chamber, wait 24hrs or until value plateaus before recording measurement
float SHTC3_high_reference = 75;    // High reference humidity chamber value (70-100%) or validated sensor reading
float SHTC3_high_reading = 75;      // SHTC3 measured value when sensor is measuring in high_reference value chamber
float SHTC3_low_reference = 33;     // Low reference humidity chamber value (10-30%) or validated sensor reading
float SHTC3_low_reading = 33;       // SHTC3 measured value when sensor is measuring in low_reference value chamber

// -------------------------------------------------------------------
// SCD41 calibration (on Low CO2 sensor board)
// -------------------------------------------------------------------
float SCD41_offsetTemp = 0;  // Offset SCD41 temperature to match other sensors

bool SCD41_calibrateRH = false;  // Optionally: Use a humidity-controlled environment to calibrate the SCD41 Humidity Sensor
// Calibration standards such as salt media (NaCl and MgCl₂) or an electronically controlled humidity chamber can be used
// Place saturated salt solution (salt + a little water) in an airtight container.
// 75% RH (NaCl) and 33% RH (MgCl₂)
// Place sensor in chamber, wait 24hrs or until value plateaus before recording measurement
float SCD41_high_reference = 75;    // High reference humidity chamber value (70-100%) or validated sensor reading
float SCD41_high_reading = 75;      // SCD41 measured value when sensor is measuring in high_reference value chamber
float SCD41_low_reference = 33;     // Low reference humidity chamber value (10-30%) or validated sensor reading
float SCD41_low_reading = 33;       // SCD41 measured value when sensor is measuring in low_reference value chamber

// -------------------------------------------------------------------
// Additional Parameters
// -------------------------------------------------------------------
int   baud      = 115200;  // Serial port baud rate
float pressure  = 1020.0;  // Ambient average pressure (mbar) used for CO2 calibration if no BME280 pressure sensor is installed

// -------------------------------------------------------------------
// Configuration Hotspot (values below are configurable in hotspot)
// -------------------------------------------------------------------
char hotspotSSID[64] = "Incubation Setup";  // SSID for the configuration hotspot
char hotspotPWD[64]  = "setup123";         // Password (must be exactly 8 chars)
int  hotspotTimer    = 10;                 // Seconds to wait after reboot to allow a user to activate the config hotspot