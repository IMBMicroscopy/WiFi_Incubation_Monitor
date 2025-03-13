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
  Serial.println(F("Incubation Monitor Initialising"));
  Serial.print(F("WiFi Mac Address: ")); Serial.println(getDefaultMacAddress());
  tft.setTextColor(ST77XX_WHITE);  //default text colour
  tft.setTextSize(3);  //1 (small) - 4(biggest) text size
  tft.printf("Incubation\n\nMonitor\n\nInitialising\n");
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

    if(myCO2 > 0){
      tft.print("CO2:");
      if(myCO2 < lowCO2){
        tft.setTextColor(ST77XX_RED);  //values out of range in RED
      }else if(myCO2 > highCO2) {
        tft.setTextColor(ST77XX_RED); //values out of range in RED       
      }else{
        tft.setTextColor(ST77XX_GREEN);  //values in range in GREEN
      }
      tft.print(myCO2, 2); tft.print("%");
      if(compFlag){tft.println(" *");}else{tft.println("");}
    }
    
    //Temperature
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.println("");
    tft.setTextSize(3);
    tft.print("T:"); 
    if(myTemp < lowTemp){
      tft.setTextColor(ST77XX_RED);  //values out of range in RED
    }else if(myTemp > highTemp) {
      tft.setTextColor(ST77XX_RED);  //values out of range in RED      
    }else{
      tft.setTextColor(ST77XX_GREEN);  //values in range in GREEN
    }
    tft.print(myTemp, 1);// tft.print("C");
    if(room_Monitor){
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.print(" / "); 
      tft.setTextSize(3);
      tft.print(bmeTemp, 1); 
    }
    tft.print("C"); tft.println("");

    //Relative Humidity
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.println("");
    tft.setTextSize(3);
    tft.print("RH:"); 
    if(myRH < lowRH){
      tft.setTextColor(ST77XX_RED);  //values out of range in RED
    }else if(myRH > highRH) {
      tft.setTextColor(ST77XX_RED); //values out of range in RED       
    }else{
      tft.setTextColor(ST77XX_GREEN);  //values in range in GREEN
    }
    tft.print(myRH, 0); //tft.print("%");
    if(room_Monitor){
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(2);
      tft.print(" / "); 
      tft.setTextSize(3);
      tft.print(bmeCRH, 0); 
    }
    tft.print("%"); tft.println("");
    
    compFlag = false;

    //if the battery parameters are in range it probably exists, so display values on display
    if(battExistsFlag){
      //Battery SOC and charge rate
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("");
      tft.setTextSize(3);
      tft.print("B:"); 
      if(battPercent < lowBatt){
        tft.setTextColor(ST77XX_RED);  //values out of range in RED
      }else{
        tft.setTextColor(ST77XX_GREEN);  //values in range in GREEN
      }
      tft.setTextSize(3);
      tft.print(battPercent, 0); tft.print("% ");
      tft.setTextSize(3);

      //Battery charge/discharge rate
      /*if(battRate < 0){
        tft.setTextColor(ST77XX_RED); //values out of range in RED
      }else{
        tft.setTextColor(ST77XX_GREEN);  //values in range in GREEN
      }
      tft.print(" "); tft.print(battRate, 0); tft.println("%/h");
      */
    }

    //display Wifi and IO indicators if Dashboard is enabled
    if(dashboard_Monitor){
      if(!battExistsFlag){
        tft.setTextSize(1);
        tft.println("");
      }

      //display WiFi indicator
      tft.setTextSize(3);
      if(wifiConnectedFlag){
        tft.setTextColor(ST77XX_GREEN);
      }else{
        tft.setTextColor(ST77XX_RED);  
      }
      tft.print("WiFi ");
  
      //display IO indicator 
      if(updatingDashboardFlag){
        tft.setTextColor(ST77XX_GREEN);
        tft.setTextSize(3);
        tft.print("^");
        updatingDashboardFlag = false;
      }else if(!ioConnectedFlag){
        tft.setTextColor(ST77XX_RED);
        tft.setTextSize(3);
        tft.print("^");
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
  // BME280 calibration
  dtostrf(bme280_offsetTemp, 4, 2, bme280_offsetTemp_buff);
  boolToCharArray(bme280_calibrateRH, bme280_calibrateRH_buff);
  dtostrf(bme280_high_reference, 4, 2, bme280_high_reference_buff);
  dtostrf(bme280_high_reading, 4, 2, bme280_high_reading_buff);
  dtostrf(bme280_low_reference, 4, 2, bme280_low_reference_buff);
  dtostrf(bme280_low_reading, 4, 2, bme280_low_reading_buff);
  // SHTC3 calibration
  dtostrf(SHTC3_offsetTemp, 4, 2, SHTC3_offsetTemp_buff);
  boolToCharArray(SHTC3_calibrateRH, SHTC3_calibrateRH_buff);
  dtostrf(SHTC3_high_reference, 4, 2, SHTC3_high_reference_buff);
  dtostrf(SHTC3_high_reading, 4, 2, SHTC3_high_reading_buff);
  dtostrf(SHTC3_low_reference, 4, 2, SHTC3_low_reference_buff);
  dtostrf(SHTC3_low_reading, 4, 2, SHTC3_low_reading_buff);
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