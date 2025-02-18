/*********************************************************************
 * User-Definable Settings for the Incubation Monitor
 * Adjust as required
 *********************************************************************/

// -------------------------------------------------------------------
// Feature Toggles
// Set to false if you haven't installed the hardware or don't require the feature.
// -------------------------------------------------------------------
bool low_CO2_Monitor    = true; // Optional SCD41 sensor for measuring low ambient levels of CO2 (<1%)
bool high_CO2_Monitor   = true; // STC31 sensor (0.7 - 100% CO2) attached?
bool pressure_Monitor   = true; // Optional BME280 sensor for measuring ambient pressure
bool battery_Monitor    = true; // Monitor battery charge if a battery is connected
bool dashboard_Monitor  = true; // Enable/disable online dashboard reporting (requires WiFi or Ethernet)

// -------------------------------------------------------------------
// CO2 Sensor Configuration
// -------------------------------------------------------------------
float switchCO2Sensor = 0.7; // Changeover from low-CO2 (SCD41) to high-CO2 (STC31) sensor. 
// If STC31 reading > 0.7%, use STC31; otherwise, use SCD41.

// -------------------------------------------------------------------
// Measurement Limits (Incubation Settings)
// Below or above these thresholds, an alarm condition (e.g., TFT color change) occurs
// -------------------------------------------------------------------
float lowCO2   = 4.1;   // CO2 % value BELOW this triggers an alarm
float highCO2  = 6;   // CO2 % value ABOVE this triggers an alarm
float lowRH    = 40;  // Relative humidity % BELOW this triggers an alarm
float highRH   = 100; // Relative humidity % ABOVE this triggers an alarm
float lowTemp  = 35;  // Temperature (°C) BELOW this triggers an alarm
float highTemp = 38;  // Temperature (°C) ABOVE this triggers an alarm

// -------------------------------------------------------------------
// Battery Settings
// -------------------------------------------------------------------
float lowBatt = 20; // Battery State of Charge (SOC) in % BELOW this triggers an alarm

// -------------------------------------------------------------------
// Measurement Rates (in seconds)
// -------------------------------------------------------------------
int sensorRate      = 15;  // Read sensors and report every X seconds (>=1)
int compensateRate  = 15;  // Recalibrate CO2 sensor (T, RH, Pressure) every X seconds (>= sensorRate)
int dashboardRate   = 60;  // Update the online dashboard every X seconds (>= sensorRate)
int batteryRate     = 300; // Check BMS hibernation status and wake if needed (>= sensorRate)

// -------------------------------------------------------------------
// Additional Configurations
// -------------------------------------------------------------------
int   baud      = 115200;  // Serial port baud rate
float pressure  = 1020.0;  // Ambient average pressure (mbar) used for CO2 calibration if no BME280

// -------------------------------------------------------------------
// Configuration Hotspot
// -------------------------------------------------------------------
char hotspotSSID[64] = "Incubation Setup";  // SSID for the configuration hotspot
char hotspotPWD[64]  = "setup123";         // Password (must be exactly 8 chars)
int  hotspotTimer    = 10;                 // Seconds to wait after reboot to allow a user to activate the config hotspot