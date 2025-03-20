// Incubation Monitor Main Code

void setup() {
  Serial.begin(baud);               // Initialize serial port for debugging
  delay(3000);

  Wire.begin();                     // Start I2C for sensor readings
  initTFT();                        // Initialize the TFT display
  startupText();                    // Display startup text
  delay(2000);
  saveParams();                     // save default Config parameters to flash if they dont exist already
  loadParams();                     // read config parameters from flash

  initWiFi();                       // Configure WiFi if Dashboard is enabled
  initSensors();                    // Initialize the incubation sensors
  initBattery();                    // Initialize the battery monitor
  initTimers();                     // Get the current time for loop operations
  Serial.println(F("Finished setup"));

  sleep_WiFi = false; //disable this feature for now due to instability

} // setup()

void loop() {
  updateTime();                     // Update current time values for functions
  recalSensor();                    // Recalibrate sensor if required
  readSensors();                    // Read sensor values and display them on the TFT
  batteryMonitoring();              // Monitor battery values and display them on the TFT
  
  ioStatus();                       // Check and reconnect to WiFi and IO dashboard if needed
  updateDashboard();                // Update the online dashboard
  updateTFT();                      // Refresh the TFT display
  sleepWiFi();                      // put WiFi to sleep until next dashboard update
  
  firstRun = false;                 // Reset first-run flag to trigger events at the required rate
  delay(100);                       // Short delay to prevent excessive loop frequency
} // loop()

