//general functions

//get default WiFi Mac Address
String getDefaultMacAddress() {
  String mac = "";
  unsigned char mac_base[6] = {0};

  if (esp_efuse_mac_get_default(mac_base) == ESP_OK) {
    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
    mac = buffer;
  }
  return mac;
}

//initialise timers at end of setup for code execution in loop
void initTimers(){
  //used to calculate loop timers
  setupTime = millis(); 
  lastCompensateTime = millis(); 
  lastSensorTime = millis();
  lastBatteryTime = millis();
  lastDashboardTime = millis();
}

void constrainRates(){
  //constrain refresh rates 
  sensorRate = constrain(sensorRate, 1, 32767);
  compensateRate = constrain(compensateRate, sensorRate, 32767);
  dashboardRate = constrain(dashboardRate, sensorRate, 32767);
  batteryRate = constrain(batteryRate, sensorRate, 32767);
}

//device startup text
void startupText(){
  Serial.println(F(" "));
  Serial.printf("Incubation Monitor, Ver: %s Initialising\n", SOFTWARE_VERSION);
  Serial.print(F("WiFi Mac Address: ")); Serial.println(getDefaultMacAddress());
  tft.setTextColor(ST77XX_WHITE);  //default text colour
  tft.setTextSize(3);  //1 (small) - 4(biggest) text size
  tft.printf("Incubation\n\nMonitor\n\nVer: %s\n\n", SOFTWARE_VERSION);
}

void infoPage(){
  //Check for user initiated info page request by button press
  if (digitalRead(infoPin) == HIGH){
    initTFT();
    tft.setTextColor(ST77XX_WHITE);  //default text colour

    Serial.println(F(" "));
    Serial.println(F("Info Page Initiated..."));

    startupText();  //Write startup text
    delay(3000);

    //Write Dashboard parameters
    Serial.printf("Adafruit IO values for Username: %s , Dashboard: %s.\n", IO_USERNAME_buff, IO_Dashboard_buff);
    initTFT();
    tft.setTextSize(3);  //1 (small) - 4(biggest) text size
    preferences.begin("custom", true); // Open Preferences in read mode
    tft.printf("User:\n"); tft.printf("%s\n", preferences.getString("IO_USERNAME", ""));
    tft.printf("\nFeed:\n"); tft.printf("%s\n", preferences.getString("IO_Dashboard", ""));
    delay(5000);

    //Write configuration parameters 
    Serial.printf("lowCO2:  %s\n", low_CO2_Monitor ? "true" : "false");
    Serial.printf("highCO2: %s\n", high_CO2_Monitor ? "true" : "false");
    Serial.printf("Press:   %s\n", pressure_Monitor ? "true" : "false");
    Serial.printf("Batt:    %s\n", battery_Monitor ? "true" : "false");
    Serial.printf("Dash:    %s\n", dashboard_Monitor ? "true" : "false");
    Serial.printf("Room:    %s\n", room_Monitor ? "true" : "false");
    Serial.printf("Sleep:   %s\n", sleep_WiFi ? "true" : "false");
    initTFT();
    tft.setTextSize(2);  //1 (small) - 4(biggest) text size
    tft.printf("lowCO2:  %s\n", low_CO2_Monitor ? "true" : "false");
    tft.printf("highCO2: %s\n", high_CO2_Monitor ? "true" : "false");
    tft.printf("Press:   %s\n", pressure_Monitor ? "true" : "false");
    tft.printf("Batt:    %s\n", battery_Monitor ? "true" : "false");
    tft.printf("Dash:    %s\n", dashboard_Monitor ? "true" : "false");
    tft.printf("Room:    %s\n", room_Monitor ? "true" : "false");
    tft.printf("Sleep:   %s\n", sleep_WiFi ? "true" : "false");

    preferences.end();
    tft.setTextSize(3);  //1 (small) - 4(biggest) text size
    delay(5000);
  }
}

//update current time for use in other functions to control code execution
void updateTime(){
  //format elapsed Time for printing
  now = millis() - setupTime;
  hour = (now / ms_per_hour);
  now -= (hour * ms_per_hour);
  minute = (now / ms_per_min);
  now -= (minute * ms_per_min);
  second = (now / ms_per_sec);
}

//create a rotating cursor for the tft display
int rotatingCursor(int counter, int cursorX, int cursorY){
  tft.setCursor(cursorX, cursorY);
  if(counter == 4){counter = 0;}
  String cursor[4] = {"|","/","-","\\"};
  tft.setTextColor(0xFFFF, 0x0000);
  tft.print(cursor[counter]);
  return counter + 1;
}

//initialise the TFT display
void initTFT(){
  // turn on tft backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  // initialize TFT
  tft.init(135, 240); // Init ST7789 240x135
  tft.setRotation(1);  //3=rotate display 180 degrees
  tft.fillScreen(ST77XX_BLACK); //clear display
  tft.setTextWrap(false);  //disable text wrapping
  tft.setCursor(0, 0); //move cursor to top left of screen
}


void updateTFT(){
  elapsedTFTTime = (millis() - lastTFTTime)/1000;
  if(firstRun || (elapsedTFTTime >= sensorRate)) {
    lastTFTTime = millis(); //update timer

    //set TFT display
    tft.fillScreen(ST77XX_BLACK); //clear display
    tft.setCursor(0, 0); //position cursor at top left
    tft.setTextColor(ST77XX_WHITE); //set text colour
    tft.setTextSize(3);
      
    bool probeMonitor = low_CO2_Monitor || high_CO2_Monitor;
    bool roomOnly = room_Monitor && !probeMonitor;

    // CO2
    if(probeMonitor){
      //if (myCO2 >= 0) {
          tft.print("CO2:");
          // Set text color based on CO2 range
          tft.setTextColor((myCO2 < lowCO2 || myCO2 > highCO2) ? ST77XX_RED : ST77XX_GREEN);
          tft.print(myCO2, 1);
          tft.print("%");
          // Display '*' if compFlag is true
          tft.println(compFlag ? " *" : "");
      //} 
    }

    // Temperature
    if (probeMonitor || room_Monitor) {
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("");
      tft.setTextSize(3);
      tft.print("T:");


      if (probeMonitor) {
        // Probe monitor temperature range check
        tft.setTextColor((myTemp < lowTemp || myTemp > highTemp) ? ST77XX_RED : ST77XX_GREEN);
        tft.print(myTemp, 1);
        if (probeMonitor && room_Monitor){
          // Room monitor
          tft.setTextColor(ST77XX_WHITE);
          tft.setTextSize(2);
          tft.print(" / ");
          tft.setTextSize(3);
        }
        if (room_Monitor) {
            tft.print(bmeTemp, 1);
        }
      }else if (roomOnly) {
        tft.print(bmeTemp, 1);
      }
      tft.print("C");
      tft.println("");
    }

    // Relative Humidity
    if (probeMonitor || room_Monitor) {
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(1);
        tft.println("");
        tft.setTextSize(3);
        tft.print("RH:");

        bool probeMonitor = low_CO2_Monitor || high_CO2_Monitor;
        bool roomOnly = room_Monitor && !probeMonitor;

        if (probeMonitor) {
          // Probe monitor RH range check
          tft.setTextColor((myRH < lowRH || myRH > highRH) ? ST77XX_RED : ST77XX_GREEN);
          tft.print(myRH, 0);
          if (probeMonitor && room_Monitor){
            // Room monitor
            tft.setTextColor(ST77XX_WHITE);
            tft.setTextSize(2);
            tft.print(" / ");
            tft.setTextSize(3);
          }
          if (room_Monitor) {
            tft.print(bmeCRH, 0);
          }
        } else if (roomOnly) {
          tft.print(bmeCRH, 0);
        }
        tft.print("%");
        tft.println("");
    }
    
    // Relative Humidity
    if (roomOnly) {
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("");
      tft.setTextSize(3);
      tft.print("P:");
      tft.print(myPress, 0);
      tft.print("mBar");
      tft.println("");
    }

    compFlag = false;  //turn off compensation indicator on next tft update

    //if the battery parameters are in range it probably exists, so display values on display
    if (battExistsFlag) {
        // Battery SOC
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(1);
        tft.println("");
        tft.setTextSize(3);
        tft.print("B:");

        // Set text color based on battery percentage
        tft.setTextColor(battPercent < lowBatt ? ST77XX_RED : ST77XX_GREEN);
        tft.print(battPercent, 0);
        tft.print("% ");

        // Uncomment the following block if you want to display charge/discharge rate
        /*
        tft.setTextColor(battRate < 0 ? ST77XX_RED : ST77XX_GREEN);
        tft.print(" ");
        tft.print(battRate, 0);
        tft.println("%/h");
        */
    }

    // Display WiFi and IO indicators if Dashboard is enabled
    if (dashboard_Monitor) {
      if (!battExistsFlag) {
          tft.setTextSize(1);
          tft.println("");
      }

      // Set WiFi indicator color
      tft.setTextSize(3);
      tft.setTextColor(wifiConnectedFlag ? ST77XX_GREEN :
                      wifiAsleepFlag ? ST77XX_YELLOW :
                      !updatingDashboardFlag ? ST77XX_RED : ST77XX_BLUE);
      tft.print("WiFi ");

      // Display IO indicator
      if (!wifiAsleepFlag) {
          tft.setTextColor(updatingDashboardFlag ? ST77XX_GREEN : ST77XX_RED);
          tft.print("^");
          if (updatingDashboardFlag) updatingDashboardFlag = false;
      }
    }
  }
}

void boolToCharArray(bool value, char* output) {
    strcpy(output, value ? "true" : "false");
}


bool charArrayToBool(const char* str) {
    if (strcasecmp(str, "true") == 0 || strcmp(str, "1") == 0) {
        return true;
    }
    return false;
}

//copy default config values to character arrays
void convertParamsToCharArray(){
  //Feature Toggles
  boolToCharArray(low_CO2_Monitor, low_CO2_Monitor_buff);
  boolToCharArray(high_CO2_Monitor, high_CO2_Monitor_buff);
  boolToCharArray(pressure_Monitor, pressure_Monitor_buff);
  boolToCharArray(battery_Monitor, battery_Monitor_buff);
  boolToCharArray(dashboard_Monitor, dashboard_Monitor_buff);
  boolToCharArray(room_Monitor, room_Monitor_buff);
  boolToCharArray(sleep_WiFi, sleep_WiFi_buff);
  //Sensor Parameters
  dtostrf(switchCO2Sensors, 4, 2, switchCO2Sensors_buff);
  dtostrf(lowCO2, 4, 2, lowCO2_buff);
  dtostrf(highCO2, 4, 2, highCO2_buff);
  dtostrf(lowRH, 4, 2, lowRH_buff);
  dtostrf(highRH, 4, 2, highRH_buff);
  dtostrf(lowTemp, 4, 2, lowTemp_buff);
  dtostrf(highTemp, 4, 2, highTemp_buff);
  dtostrf(lowBatt, 4, 2, lowBatt_buff);
  dtostrf(pressure, 4, 2, pressure_buff);
  //update rates
  dtostrf(sensorRate, 4, 0, sensorRate_buff);
  dtostrf(compensateRate, 4, 0, compensateRate_buff);
  dtostrf(dashboardRate, 4, 0, dashboardRate_buff);
  dtostrf(batteryRate, 4, 0, batteryRate_buff);
  // STC31 calibration
  dtostrf(stc31_offsetCO2, 4, 2, stc31_offsetCO2_buff);
  // BME280 calibration
  dtostrf(bme280_offsetTemp, 4, 2, bme280_offsetTemp_buff);
  boolToCharArray(bme280_calibrateRH, bme280_calibrateRH_buff);
  dtostrf(bme280_high_reference, 4, 2, bme280_high_reference_buff);
  dtostrf(bme280_high_reading, 4, 2, bme280_high_reading_buff);
  dtostrf(bme280_low_reference, 4, 2, bme280_low_reference_buff);
  dtostrf(bme280_low_reading, 4, 2, bme280_low_reading_buff);
  // sht calibration
  dtostrf(sht_offsetTemp, 4, 2, sht_offsetTemp_buff);
  boolToCharArray(sht_calibrateRH, sht_calibrateRH_buff);
  dtostrf(sht_high_reference, 4, 2, sht_high_reference_buff);
  dtostrf(sht_high_reading, 4, 2, sht_high_reading_buff);
  dtostrf(sht_low_reference, 4, 2, sht_low_reference_buff);
  dtostrf(sht_low_reading, 4, 2, sht_low_reading_buff);
  // SCD41 calibration
  dtostrf(SCD41_offsetTemp, 4, 2, SCD41_offsetTemp_buff);
  boolToCharArray(SCD41_calibrateRH, SCD41_calibrateRH_buff);
  dtostrf(SCD41_high_reference, 4, 2, SCD41_high_reference_buff);
  dtostrf(SCD41_high_reading, 4, 2, SCD41_high_reading_buff);
  dtostrf(SCD41_low_reference, 4, 2, SCD41_low_reference_buff);
  dtostrf(SCD41_low_reading, 4, 2, SCD41_low_reading_buff);
  //Additional
  //dtostrf(baud, 4, 0, baud_buff);
}