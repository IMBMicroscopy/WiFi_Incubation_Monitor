//IO related functions

void userActivatedHotspot(){
  //Check for user initiated Wifi hotspot request by button press
  pinMode(hotspotPin, INPUT);
  unsigned long timeNow = millis();
  int cursorCounter = 0;
  while (millis() - timeNow < (hotspotTimer*1000)) {
    Serial.print(F("."));
    cursorCounter = rotatingCursor(cursorCounter, cursorX, cursorY);

    if (digitalRead(hotspotPin) == HIGH) {
      Serial.println(F(" "));
      Serial.println(F("resetting WiFi settings"));
      initTFT();
      tft.println(F("Hotspot On"));
      delay(1000);
      startPortalFlag = true;
      break;
    }
    delay(500);
  }
}

void initWiFi(){
  if(dashboard_Monitor){
    WiFi.begin(); // Initialise WiFi
    delay(500);
    initTFT(); // Initialise TFT display
    Serial.print(F("Hold D2 button to reset parameters and start WiFi Manager"));
    cursorX = tft.getCursorX();
    cursorY = tft.getCursorY();
    tft.printf(" Config -> ", hotspotPin);

    userActivatedHotspot();
    Serial.printf("\n");
    initTFT();
    convertParamsToCharArray();

    if((IO_USERNAME_buff[0] == '\0') or (IO_KEY_buff[0] == '\0') or (IO_Dashboard_buff[0] == '\0')){
      Serial.printf("Adafruit IO values missing - reset WiFi Manager settings");
      startPortalFlag = true;
    }else{
      Serial.printf("loaded Adafruit IO values for Username: %s , Dashboard: %s.\n", IO_USERNAME_buff, IO_Dashboard_buff);
    }
  
    initWiFiManager();  // initialise wifi manager config and establish connection to Dashboard
    initDashboard();  //initialise dashboard parameters
  } 
}

void saveParams() {
  constrainRates();                 // Constrain user-defined readout rates to acceptable values
  //All Keys must be less than or equal to 15 characters long
  preferences.begin("custom", true); // Open Preferences in read mode
  int test = preferences.getInt("baud", -1);  //test if default values exist in flash, if they dont, set value to -1
  preferences.end();
  delay(20);

  preferences.begin("custom", false); // Open Preferences in write mode
  //save IO dashboard parameters if required
  if(saveIO){
    Serial.println("saving IO details to Flash");
    preferences.putString("IO_USERNAME", IO_USERNAME_buff); //may need to convert from character array to string first
    preferences.putString("IO_KEY", IO_KEY_buff);
    preferences.putString("IO_Dashboard", IO_Dashboard_buff);
  }

  //save default configuration parameter values
  if ((test == -1) || saveIO){
    if(test == -1){Serial.println("default parameters dont exist, write them to flash");}
    else{Serial.println("writing customised default parameters to flash");}
    //feature toggles
    // Feature toggles
    preferences.putBool("low_CO2_Mon", low_CO2_Monitor);
    preferences.putBool("hi_CO2_Mon", high_CO2_Monitor);
    preferences.putBool("press_Mon", pressure_Monitor);
    preferences.putBool("batt_Mon", battery_Monitor);
    preferences.putBool("dash_Mon", dashboard_Monitor);
    preferences.putBool("room_Mon", room_Monitor);
    // Sensor parameters
    preferences.putFloat("swCO2Sens", switchCO2Sensors);
    preferences.putFloat("lowCO2", lowCO2);
    preferences.putFloat("highCO2", highCO2);
    preferences.putFloat("lowRH", lowRH);
    preferences.putFloat("highRH", highRH);
    preferences.putFloat("lowTemp", lowTemp);
    preferences.putFloat("highTemp", highTemp);
    preferences.putFloat("lowBatt", lowBatt);
    preferences.putFloat("pressure", pressure);
    // update rates
    preferences.putInt("sensorRate", sensorRate);
    preferences.putInt("compRate", compensateRate);
    preferences.putInt("dashRate", dashboardRate);
    preferences.putInt("battRate", batteryRate);
    // BME280 calibration
    preferences.putFloat("bme_offsetT", bme280_offsetTemp);
    preferences.putBool("bme_calRH", bme280_calibrateRH);
    preferences.putFloat("bme_high_ref", bme280_high_reference);
    preferences.putFloat("bme_high_read", bme280_high_reading);
    preferences.putFloat("bme_low_ref", bme280_low_reference);
    preferences.putFloat("bme_low_read", bme280_low_reading);
    // SHTC3 calibration
    preferences.putFloat("sht_offsetT", SHTC3_offsetTemp);
    preferences.putBool("sht_calRH", SHTC3_calibrateRH);
    preferences.putFloat("sht_high_ref", SHTC3_high_reference);
    preferences.putFloat("sht_high_read", SHTC3_high_reading);
    preferences.putFloat("sht_low_ref", SHTC3_low_reference);
    preferences.putFloat("sht_low_read", SHTC3_low_reading);
    // SCD41 calibration
    preferences.putFloat("scd_offsetT", SCD41_offsetTemp);
    preferences.putBool("scd_calRH", SCD41_calibrateRH);
    preferences.putFloat("scd_high_ref", SCD41_high_reference);
    preferences.putFloat("scd_high_read", SCD41_high_reading);
    preferences.putFloat("scd_low_ref", SCD41_low_reference);
    preferences.putFloat("scd_low_read", SCD41_low_reading);
    //additional
    //preferences.putInt("baud", baud);

    Serial.println("writing to Flash complete, go back to the homepage and click Exit");
  }else{Serial.println("default Values exist and havent been customised, dont write new values to flash");}
  delay(20);
  preferences.end();
  delay(20);    
  saveIO = false;
}


void loadParams() {
  Serial.println(F("Loading custom parameters from file"));
  //All Keys must be less than or equal to 15 characters long
  preferences.begin("custom", true); // Open Preferences in read mode
  //read parameters from preferences file, if they're not available then use the default values
  (preferences.getString("IO_USERNAME", "")).toCharArray(IO_USERNAME_buff, sizeof(IO_USERNAME_buff));  //may need to convert to character array from string first
  (preferences.getString("IO_KEY", "")).toCharArray(IO_KEY_buff, sizeof(IO_KEY_buff));
  (preferences.getString("IO_Dashboard","")).toCharArray(IO_Dashboard_buff, sizeof(IO_Dashboard_buff));
  //feature toggles
  low_CO2_Monitor = preferences.getBool("low_CO2_Mon", low_CO2_Monitor);
  high_CO2_Monitor = preferences.getBool("hi_CO2_Mon", high_CO2_Monitor);
  pressure_Monitor = preferences.getBool("press_Mon", pressure_Monitor);
  battery_Monitor = preferences.getBool("batt_Mon", battery_Monitor);
  dashboard_Monitor = preferences.getBool("dash_Mon", dashboard_Monitor);
  room_Monitor = preferences.getBool("room_Mon", room_Monitor);
  //sensor parameters
  switchCO2Sensors = preferences.getFloat("swCO2Sens", switchCO2Sensors);
  lowCO2 = preferences.getFloat("lowCO2", lowCO2);
  highCO2 = preferences.getFloat("highCO2", highCO2);
  lowRH = preferences.getFloat("lowRH", lowRH);
  highRH = preferences.getFloat("highRH", highRH);
  lowTemp = preferences.getFloat("lowTemp", lowTemp);
  highTemp = preferences.getFloat("highTemp", highTemp);
  lowBatt = preferences.getFloat("lowBatt", lowBatt);
  pressure = preferences.getFloat("pressure", pressure);
  //update rates
  sensorRate = preferences.getInt("sensorRate", sensorRate);
  compensateRate = preferences.getInt("compRate", compensateRate);
  dashboardRate = preferences.getInt("dashRate", dashboardRate);
  batteryRate = preferences.getInt("battRate", batteryRate);
  // BME280 calibration
  bme280_offsetTemp = preferences.getFloat("bme_offsetT", bme280_offsetTemp);
  bme280_calibrateRH = preferences.getBool("bme_calRH", bme280_calibrateRH);
  bme280_high_reference = preferences.getFloat("bme_high_ref", bme280_high_reference);
  bme280_high_reading = preferences.getFloat("bme_high_read", bme280_high_reading);
  bme280_low_reference = preferences.getFloat("bme_low_ref", bme280_low_reference);
  bme280_low_reading = preferences.getFloat("bme_low_read", bme280_low_reading);
  // SHTC3 calibration
  SHTC3_offsetTemp = preferences.getFloat("sht_offsetT", SHTC3_offsetTemp);
  SHTC3_calibrateRH = preferences.getBool("sht_calRH", SHTC3_calibrateRH);
  SHTC3_high_reference = preferences.getFloat("sht_high_ref", SHTC3_high_reference);
  SHTC3_high_reading = preferences.getFloat("sht_high_read", SHTC3_high_reading);
  SHTC3_low_reference = preferences.getFloat("sht_low_ref", SHTC3_low_reference);
  SHTC3_low_reading = preferences.getFloat("sht_low_read", SHTC3_low_reading);
  // SCD41 calibration
  SCD41_offsetTemp = preferences.getFloat("scd_offsetT", SCD41_offsetTemp);
  SCD41_calibrateRH = preferences.getBool("scd_calRH", SCD41_calibrateRH);
  SCD41_high_reference = preferences.getFloat("scd_high_ref", SCD41_high_reference);
  SCD41_high_reading = preferences.getFloat("scd_high_read", SCD41_high_reading);
  SCD41_low_reference = preferences.getFloat("scd_low_ref", SCD41_low_reference);
  SCD41_low_reading = preferences.getFloat("scd_low_read", SCD41_low_reading);  //additional
  //Additional
  //baud = preferences.getInt("baud", baud);

  preferences.end();
  constrainRates();                 // Constrain user-defined readout rates to acceptable values
}

void getHotspotValues(){
  //get values from hotspot website
  strcpy(IO_USERNAME_buff, custom_IO_USERNAME.getValue());
  strcpy(IO_KEY_buff, custom_IO_KEY.getValue());
  strcpy(IO_Dashboard_buff, custom_IO_Dashboard.getValue());
  //feature toggles
  strcpy(low_CO2_Monitor_buff, custom_low_CO2_Monitor.getValue());
  strcpy(high_CO2_Monitor_buff, custom_high_CO2_Monitor.getValue());
  strcpy(pressure_Monitor_buff, custom_pressure_Monitor.getValue());
  strcpy(battery_Monitor_buff, custom_battery_Monitor.getValue());
  strcpy(dashboard_Monitor_buff, custom_dashboard_Monitor.getValue());
  strcpy(room_Monitor_buff, custom_room_Monitor.getValue());
  //sensor parameters
  strcpy(switchCO2Sensors_buff, custom_switch_CO2_Sensors.getValue());
  strcpy(lowCO2_buff, custom_lowCO2.getValue());
  strcpy(highCO2_buff, custom_highCO2.getValue());
  strcpy(lowRH_buff, custom_lowRH.getValue());
  strcpy(highRH_buff, custom_highRH.getValue());
  strcpy(lowTemp_buff, custom_lowTemp.getValue());
  strcpy(highTemp_buff, custom_highTemp.getValue());
  strcpy(lowBatt_buff, custom_lowBatt.getValue());
  strcpy(pressure_buff, custom_pressure.getValue());
  //update rates
  strcpy(sensorRate_buff, custom_sensorRate.getValue());
  strcpy(compensateRate_buff, custom_compensateRate.getValue());
  strcpy(dashboardRate_buff, custom_dashboardRate.getValue());
  strcpy(batteryRate_buff, custom_batteryRate.getValue());
  // BME280 calibration
  strcpy(bme280_offsetTemp_buff, custom_bme280_offsetTemp.getValue());
  strcpy(bme280_calibrateRH_buff, custom_bme280_calibrateRH.getValue());
  strcpy(bme280_high_reference_buff, custom_bme280_high_reference.getValue());
  strcpy(bme280_high_reading_buff, custom_bme280_high_reading.getValue());
  strcpy(bme280_low_reference_buff, custom_bme280_low_reference.getValue());
  strcpy(bme280_low_reading_buff, custom_bme280_low_reading.getValue());
  // SHTC3 calibration
  strcpy(SHTC3_offsetTemp_buff, custom_SHTC3_offsetTemp.getValue());
  strcpy(SHTC3_calibrateRH_buff, custom_SHTC3_calibrateRH.getValue());
  strcpy(SHTC3_high_reference_buff, custom_SHTC3_high_reference.getValue());
  strcpy(SHTC3_high_reading_buff, custom_SHTC3_high_reading.getValue());
  strcpy(SHTC3_low_reference_buff, custom_SHTC3_low_reference.getValue());
  strcpy(SHTC3_low_reading_buff, custom_SHTC3_low_reading.getValue());
  // SCD41 calibration
  strcpy(SCD41_offsetTemp_buff, custom_SCD41_offsetTemp.getValue());
  strcpy(SCD41_calibrateRH_buff, custom_SCD41_calibrateRH.getValue());
  strcpy(SCD41_high_reference_buff, custom_SCD41_high_reference.getValue());
  strcpy(SCD41_high_reading_buff, custom_SCD41_high_reading.getValue());
  strcpy(SCD41_low_reference_buff, custom_SCD41_low_reference.getValue());
  strcpy(SCD41_low_reading_buff, custom_SCD41_low_reading.getValue());
  //additional
  //strcpy(baud_buff, custom_baud.getValue());

  //convert character arrays into values used in calculations
  //feature toggles
  low_CO2_Monitor = charArrayToBool(low_CO2_Monitor_buff);  
  high_CO2_Monitor = charArrayToBool(high_CO2_Monitor_buff);
  pressure_Monitor = charArrayToBool(pressure_Monitor_buff);
  battery_Monitor = charArrayToBool(battery_Monitor_buff);
  dashboard_Monitor = charArrayToBool(dashboard_Monitor_buff);
  room_Monitor = charArrayToBool(room_Monitor_buff);
  //sensor parameters
  switchCO2Sensors = atof(switchCO2Sensors_buff);
  lowCO2 = atof(lowCO2_buff);
  highCO2 = atof(highCO2_buff);
  lowRH = atof(lowRH_buff);
  highRH = atof(highRH_buff);
  lowTemp = atof(lowTemp_buff);
  highTemp = atof(highTemp_buff);
  lowBatt = atof(lowBatt_buff);
  pressure = atof(pressure_buff);
  //update rates
  sensorRate = atof(sensorRate_buff);
  compensateRate = atof(compensateRate_buff);
  dashboardRate = atof(dashboardRate_buff);
  batteryRate = atof(batteryRate_buff);
  // BME280 calibration
  bme280_offsetTemp = atof(bme280_offsetTemp_buff);
  bme280_calibrateRH = charArrayToBool(bme280_calibrateRH_buff);
  bme280_high_reference = atof(bme280_high_reference_buff);
  bme280_high_reading = atof(bme280_high_reading_buff);
  bme280_low_reference = atof(bme280_low_reference_buff);
  bme280_low_reading = atof(bme280_low_reading_buff);
  // SHTC3 calibration
  SHTC3_offsetTemp = atof(SHTC3_offsetTemp_buff);
  SHTC3_calibrateRH = charArrayToBool(SHTC3_calibrateRH_buff);
  SHTC3_high_reference = atof(SHTC3_high_reference_buff);
  SHTC3_high_reading = atof(SHTC3_high_reading_buff);
  SHTC3_low_reference = atof(SHTC3_low_reference_buff);
  SHTC3_low_reading = atof(SHTC3_low_reading_buff);
  // SCD41 calibration
  SCD41_offsetTemp = atof(SCD41_offsetTemp_buff);
  SCD41_calibrateRH = charArrayToBool(SCD41_calibrateRH_buff);
  SCD41_high_reference = atof(SCD41_high_reference_buff);
  SCD41_high_reading = atof(SCD41_high_reading_buff);
  SCD41_low_reference = atof(SCD41_low_reference_buff);
  SCD41_low_reading = atof(SCD41_low_reading_buff);
  //additional
  //baud = atof(baud_buff);
}

// callback to save custom parameters on config hotspot
void saveParamsCallback() {
  shouldSaveConfig = true;
  getHotspotValues();  //read hotspot values and convert as required
  saveIO = true;  //flag to activate save
  saveParams(); //save files to preferences
} // end save

//configure WiFi Manager parameters and either autoconnect to known WiFi or launch a hotspot to configure
void initWiFiManager() {
  Serial.println(F("\nstarting Wifi Manager"));

  //configure portal
  wifiManager.setClass("invert");          // enable "dark mode" for the config portal
  wifiManager.setConfigPortalTimeout(30); // sets timeout before AP,webserver loop ends and exits even if there has been no setup.
  wifiManager.setAPClientCheck(true);      // avoid timeout if client connected to hotspot
  wifiManager.setWiFiAutoReconnect(true);  // enable the device to auto connect to Wifi if it drops out
  wifiManager.setConnectRetries(15);  //sets number of retries for autoconnect, force retry after wait failure exit
  wifiManager.setSaveConnectTimeout(10);  //sets timeout for which to attempt connecting on saves, useful if there are bugs in esp waitforconnectloop
  wifiManager.setSaveConnect(true);  //lets you disable automatically connecting after save from webportal
  wifiManager.setShowPassword(false);  //hide the saved wifi password
  wifiManager.setTitle("WiFi Incubation Monitor");   //title of config portal webpage 
  wifiManager.setBreakAfterConfig(true);  //if this is set, it will exit after config, even if connection is unsuccessful.
  wifiManager.setParamsPage(true);  //create the Setup page for additional parameters

  //custom dashboard parameters
  wifiManager.addParameter(&custom_dashboard_title);
  wifiManager.addParameter(&custom_IO_USERNAME); 
  wifiManager.addParameter(&custom_IO_KEY);      
  wifiManager.addParameter(&custom_IO_Dashboard);  
    //custom Feature toggles parameters
  wifiManager.addParameter(&custom_monitor_title);
  wifiManager.addParameter(&custom_low_CO2_Monitor); 
  wifiManager.addParameter(&custom_high_CO2_Monitor);      
  wifiManager.addParameter(&custom_pressure_Monitor);  
  wifiManager.addParameter(&custom_battery_Monitor);  
  wifiManager.addParameter(&custom_dashboard_Monitor);  
  wifiManager.addParameter(&custom_room_Monitor);
  //custom parameters for sensors
  wifiManager.addParameter(&custom_sensor_title);
  wifiManager.addParameter(&custom_switch_CO2_Sensors);
  wifiManager.addParameter(&custom_lowCO2);
  wifiManager.addParameter(&custom_highCO2);
  wifiManager.addParameter(&custom_lowRH);
  wifiManager.addParameter(&custom_highRH);
  wifiManager.addParameter(&custom_lowTemp);
  wifiManager.addParameter(&custom_highTemp);
  wifiManager.addParameter(&custom_lowBatt);
  wifiManager.addParameter(&custom_pressure);
  //update rates
  wifiManager.addParameter(&custom_update_title);
  wifiManager.addParameter(&custom_sensorRate);
  wifiManager.addParameter(&custom_compensateRate);
  wifiManager.addParameter(&custom_dashboardRate);
  wifiManager.addParameter(&custom_batteryRate);
  // BME280 calibration
  wifiManager.addParameter(&custom_bme280_title);
  wifiManager.addParameter(&custom_bme280_offsetTemp);
  wifiManager.addParameter(&custom_bme280_calibrateRH);
  wifiManager.addParameter(&custom_bme280_high_reference);
  wifiManager.addParameter(&custom_bme280_high_reading);
  wifiManager.addParameter(&custom_bme280_low_reference);
  wifiManager.addParameter(&custom_bme280_low_reading);
  // SHTC3 calibration
  wifiManager.addParameter(&custom_SHTC3_title);
  wifiManager.addParameter(&custom_SHTC3_offsetTemp);
  wifiManager.addParameter(&custom_SHTC3_calibrateRH);
  wifiManager.addParameter(&custom_SHTC3_high_reference);
  wifiManager.addParameter(&custom_SHTC3_high_reading);
  wifiManager.addParameter(&custom_SHTC3_low_reference);
  wifiManager.addParameter(&custom_SHTC3_low_reading);
  // SCD41 calibration
  wifiManager.addParameter(&custom_SCD41_title);
  wifiManager.addParameter(&custom_SCD41_offsetTemp);
  wifiManager.addParameter(&custom_SCD41_calibrateRH);
  wifiManager.addParameter(&custom_SCD41_high_reference);
  wifiManager.addParameter(&custom_SCD41_high_reading);
  wifiManager.addParameter(&custom_SCD41_low_reference);
  wifiManager.addParameter(&custom_SCD41_low_reading);
  //additional
  //wifiManager.addParameter(&custom_Additional_title);
  //wifiManager.addParameter(&custom_baud);
  
  //apply values to hotspot
  custom_IO_USERNAME.setValue(IO_USERNAME_buff, 64); // set custom parameter value
  custom_IO_KEY.setValue(IO_KEY_buff, 64);           // set custom parameter value
  custom_IO_Dashboard.setValue(IO_Dashboard_buff, 64); // set custom parameter value
  //feature toggles
  custom_low_CO2_Monitor.setValue(low_CO2_Monitor_buff, 6);
  custom_high_CO2_Monitor.setValue(high_CO2_Monitor_buff, 6);
  custom_pressure_Monitor.setValue(pressure_Monitor_buff, 6);
  custom_battery_Monitor.setValue(battery_Monitor_buff, 6);
  custom_dashboard_Monitor.setValue(dashboard_Monitor_buff, 6);
  custom_room_Monitor.setValue(room_Monitor_buff, 6);
  //sensor parameters
  custom_switch_CO2_Sensors.setValue(switchCO2Sensors_buff, 8);
  custom_lowCO2.setValue(lowCO2_buff, 8);
  custom_highCO2.setValue(highCO2_buff, 8);
  custom_lowRH.setValue(lowRH_buff, 8);
  custom_highRH.setValue(highRH_buff, 8);
  custom_lowTemp.setValue(lowTemp_buff, 8);
  custom_highTemp.setValue(highTemp_buff, 8);
  custom_lowBatt.setValue(lowBatt_buff, 8);
  custom_pressure.setValue(pressure_buff, 8);
  //update rates
  custom_sensorRate.setValue(sensorRate_buff, 8);
  custom_compensateRate.setValue(compensateRate_buff, 8);
  custom_dashboardRate.setValue(dashboardRate_buff, 8);
  custom_batteryRate.setValue(batteryRate_buff, 8);
  // BME280 calibration
  custom_bme280_offsetTemp.setValue(bme280_offsetTemp_buff, 8);
  custom_bme280_calibrateRH.setValue(bme280_calibrateRH_buff, 6);
  custom_bme280_high_reference.setValue(bme280_high_reference_buff, 8);
  custom_bme280_high_reading.setValue(bme280_high_reading_buff, 8);
  custom_bme280_low_reference.setValue(bme280_low_reference_buff, 8);
  custom_bme280_low_reading.setValue(bme280_low_reading_buff, 8);
  // SHTC3 calibration
  custom_SHTC3_offsetTemp.setValue(SHTC3_offsetTemp_buff, 8);
  custom_SHTC3_calibrateRH.setValue(SHTC3_calibrateRH_buff, 6);
  custom_SHTC3_high_reference.setValue(SHTC3_high_reference_buff, 8);
  custom_SHTC3_high_reading.setValue(SHTC3_high_reading_buff, 8);
  custom_SHTC3_low_reference.setValue(SHTC3_low_reference_buff, 8);
  custom_SHTC3_low_reading.setValue(SHTC3_low_reading_buff, 8);
  // SCD41 calibration
  custom_SCD41_offsetTemp.setValue(SCD41_offsetTemp_buff, 8);
  custom_SCD41_calibrateRH.setValue(SCD41_calibrateRH_buff, 6);
  custom_SCD41_high_reference.setValue(SCD41_high_reference_buff, 8);
  custom_SCD41_high_reading.setValue(SCD41_high_reading_buff, 8);
  custom_SCD41_low_reference.setValue(SCD41_low_reference_buff, 8);
  custom_SCD41_low_reading.setValue(SCD41_low_reading_buff, 8);
  //additional
  //custom_baud.setValue(baud_buff, 8);

  initTFT();  //reset TFT display
  tft.println(F("Hotspot On"));
  
  wifiManager.setSaveParamsCallback(saveParamsCallback);  //called when saving either WiFi or Setup params page

  if(startPortalFlag){
    wifiManager.startConfigPortal(hotspotSSID, hotspotPWD);  //start hotspot if user pressed button
  }else{
    if (!wifiManager.autoConnect(hotspotSSID, hotspotPWD)) // connect to wifi with existing setting or start config hotspot if wifi not found
    {
      Serial.println(F("User failed to connect to hotspot"));
      tft.println(F("WiFi Failed") );
    }
    else
    {
      Serial.println(F("Connected to WiFi."));
      tft.println(F("WiFi Success"));
    }
  }

  delay(1000);

  if(WiFi.status() == WL_CONNECTED) {
    wifiConnectedFlag = true;
  }else{
    wifiConnectedFlag = false;
  }
}

//generate feednames for Adafruit IO dashboard and connect
void initDashboard() {
  if(dashboard_Monitor){
    if(wifiConnectedFlag){
      //create feed names
      strcpy (CO2_FeedName_buff, IO_Dashboard_buff);
      strcat (CO2_FeedName_buff, "_probe_CO2");  //create string identifier for data feed
      String CO2String((char*)CO2_FeedName_buff);
      strlwr (CO2_FeedName_buff);  //make lower case

      strcpy(Temp_FeedName_buff, IO_Dashboard_buff);
      strcat(Temp_FeedName_buff, "_probe_TEMP");  //create string identifier for data feed
      String TempString((char*)Temp_FeedName_buff);
      strlwr(Temp_FeedName_buff);  //make lower case

      strcpy(RH_FeedName_buff, IO_Dashboard_buff);
      strcat(RH_FeedName_buff, "_probe_RH");  //create string identifier for data feed
      String RHString((char*)RH_FeedName_buff);
      strlwr(RH_FeedName_buff);  //make lower case

      Serial.printf("\nCO2 data feed name: %s\n", CO2_FeedName_buff);
      Serial.printf("Temperature data feed name: %s\n", Temp_FeedName_buff);
      Serial.printf("Relative Humidity data feed name: %s\n", RH_FeedName_buff);
      Serial.println("");

      if(room_Monitor){
        //create feed names for the room monitor
        strcpy(roomTemp_FeedName_buff, IO_Dashboard_buff);
        strcat(roomTemp_FeedName_buff, "_room_TEMP");  //create string identifier for data feed
        String roomTempString((char*)roomTemp_FeedName_buff);
        strlwr(roomTemp_FeedName_buff);  //make lower case
      
        strcpy(roomRH_FeedName_buff, IO_Dashboard_buff);
        strcat(roomRH_FeedName_buff, "_room_RH");  //create string identifier for data feed
        String roomRHString((char*)roomRH_FeedName_buff);
        strlwr(roomRH_FeedName_buff);  //make lower case

        strcpy(roomPress_FeedName_buff, IO_Dashboard_buff);
        strcat(roomPress_FeedName_buff, "_room_Press");  //create string identifier for data feed
        String roomPressString((char*)roomPress_FeedName_buff);
        strlwr(roomPress_FeedName_buff);  //make lower case

        Serial.printf("Temperature data feed name: %s\n", roomTemp_FeedName_buff);
        Serial.printf("Relative Humidity data feed name: %s\n", roomRH_FeedName_buff);
        Serial.printf("Pressure data feed name: %s\n", roomPress_FeedName_buff);
        Serial.println("");
      }

      //if dashboad feed name was created, then connect to dashboard, otherwise dont
      if((CO2String != "_CO2") && (TempString != "_TEMP") && (RHString != "_RH")){
        io = new (objStorage) AdafruitIO_WiFi(IO_USERNAME_buff, IO_KEY_buff, "", ""); //create IO object with user details for subsequent connection
        Serial.printf("Connecting to Adafruit IO with User: %s, Dashboard: %s.\n", IO_USERNAME_buff, IO_Dashboard_buff);

        io->connect(); //initiate IO connection for data feeds

        if(firstRun){
          cursorX0 = tft.getCursorX();
          cursorY0 = tft.getCursorY();
          tft.print(F("IO Attempt "));
          cursorX = tft.getCursorX();
          cursorY = tft.getCursorY();
        }

        // wait for a connection
        int cursorCounter = 0;
        while ((io->status() < AIO_CONNECTED)) //|| (io.status() != AIO_NET_CONNECTED))
        {
          Serial.print(".");
          cursorCounter = rotatingCursor(cursorCounter, cursorX, cursorY);
          delay(500);
        }

        //Configure data feeds for each parameter to log
        CO2_Feed = io->feed(CO2_FeedName_buff);
        Temp_Feed = io->feed(Temp_FeedName_buff);
        RH_Feed = io->feed(RH_FeedName_buff);

        if(room_Monitor){
          roomPress_Feed = io->feed(roomPress_FeedName_buff);
          roomTemp_Feed = io->feed(roomTemp_FeedName_buff);
          roomRH_Feed = io->feed(roomRH_FeedName_buff);
        }

        Serial.println(F("Adafruit IO connection established\n"));
        if(firstRun){
          tft.setCursor(cursorX0, cursorY0);
          tft.println(F("IO Success"));
          delay(1000);
        }
        ioConnectedFlag = true;          // successful connection
      }else{
        Serial.println(F("Adafruit IO values missing - disable dashboard upload"));
        Serial.print(F("IO_USERNAME: "));Serial.println(IO_USERNAME_buff);
        Serial.print(F("IO_Dashboard: "));Serial.println(IO_Dashboard_buff);
        if(firstRun){
          tft.setCursor(cursorX0, cursorY0);
          tft.println(F("IO Failed"));
          delay(1000);
        }
        ioConnectedFlag = false;
      }
    }else{Serial.println(F("Wifi not connected"));}
  }else{Serial.println(F("Dashboard disabled in config"));}
}

void ioStatus(){
    elapsedIOTime = (millis() - lastIOTime)/1000;
    if(dashboard_Monitor && (firstRun || (elapsedIOTime >= dashboardRate))){
      lastIOTime = millis(); //update timer

    //auto reconnect to wifi if connection is lost
    if(WiFi.status() != WL_CONNECTED) {
      wifiConnectedFlag = false;
      Serial.println(F("\nReconnecting to WiFi..."));
      wifiManager.setEnableConfigPortal(false);
      wifiManager.disconnect();
      WiFi.disconnect();
      delay(1000);
      WiFi.begin();
      Serial.print("reconnect to wifi SSID: "); Serial.println(WiFi.SSID());
      wifiManager.autoConnect((WiFi.SSID()).c_str(),(WiFi.psk()).c_str());
      delay(1000);
    }else{
      wifiConnectedFlag = true;
    }
  
    //stay connected to adafruit io
    if(wifiConnectedFlag){ 
      if(io->status() >= AIO_CONNECTED) {
        io->run();
        ioConnectedFlag = true;
      }else{
        Serial.println("Attempt to reconnect to IO!\n\n");
        initDashboard();      
      }
    } else{ioConnectedFlag = false;}
  }
}

void updateDashboard(){
  if(ioConnectedFlag){
    elapsedDashboardTime = (millis() - lastDashboardTime)/1000;
    //report to online dashboard
    if(dashboard_Monitor && (firstRun || elapsedDashboardTime >= dashboardRate)){
      lastDashboardTime = millis(); //update timer
      updatingDashboardFlag = true;
      //report elapsed time
      Serial.println(F("")); Serial.print(F("Elapsed Time = ")); Serial.print(hour); Serial.print("h:"); Serial.print(minute); Serial.print("m"); Serial.print(second); Serial.println("s");
      
      Serial.println(F("Updating Dashboard"));
      
      //update online dashboard values
      if(myCO2 >0){CO2_Feed->save(myCO2);}
      Temp_Feed->save(myTemp);
      RH_Feed->save(myRH);

      if(room_Monitor){
        roomTemp_Feed->save(bmeTemp);
        roomRH_Feed->save(bmeRH);
        roomPress_Feed->save(bmePress);
      }
    }
  }
}