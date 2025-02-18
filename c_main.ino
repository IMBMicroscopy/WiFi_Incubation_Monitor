// Incubation Monitor Main Code

void setup() {
  Serial.begin(baud);               // Initialize serial port for debugging
  delay(2000);

  Wire.begin();                     // Start I2C for sensor readings
  initTFT();                        // Initialize the TFT display
  startupText();                    // Display startup text
  saveDefaultConfig();              // save default Config parameters to FS if they dont exist already
  delay(2000);

  initWiFi();                       // Configure WiFi if Dashboard is enabled
  initSensors();                    // Initialize the incubation sensors
  compensate();                     // Perform initial sensor compensation
  initBattery();                    // Initialize the battery monitor
  initTimers();                     // Get the current time for loop operations
  constrainRates();                 // Constrain user-defined readout rates to acceptable values

  Serial.print("use lowCO2 = "); Serial.println(lowCO2);
  Serial.println(F("\nFinished setup\n"));
} // setup()

void loop() {
  updateTime();                     // Update current time values for functions
  recalSensor();                    // Recalibrate sensor if required
  readSensors();                    // Read sensor values and display them on the TFT
  batteryMonitoring();              // Monitor battery values and display them on the TFT

  // Online updates
  ioStatus();                       // Check and reconnect to WiFi and IO dashboard if needed
  updateDashboard();                // Update the online dashboard

  updateTFT();                      // Refresh the TFT display

  firstRun = false;                 // Reset first-run flag to trigger events at the required rate
  delay(100);                       // Short delay to prevent excessive loop frequency
} // loop()