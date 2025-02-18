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

void userActivatedHotspot(){
  //Check for user initiated Wifi Parameter reset by button press
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
    Serial.printf("\n\n");
    initTFT();
    readParamsFromFS(); // get parameters from file system
    initWiFiManager();  // initialise wifi manager config and establish connection to Dashboard
    initDashboard();  //initialise dashboard parameters
  } 
}

void saveDefaultConfig(){
  //see if default config parameters exist in config.json file on FS
  if (LittleFS.begin()){
    if (LittleFS.exists("/config.json")){
      // file exists, reading and loading
      Serial.println(F("Reading config file for default values"));
      File configFile = LittleFS.open("/config.json", "r");
      if (configFile){
        Serial.println("config file found");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        Serial.print("config file output : "); Serial.println(Serial);
        configFile.close();

        if (!deserializeError){
          if (json.containsKey("param_lowCO2")){
            Serial.println("default Values exist, dont write new values");
          }else{
            Serial.println("default Values dont exist, write them to config file");

            //copy default config values to character arrays for writing to FS
            dtostrf(1,1,1, Default_Values);
            dtostrf(lowCO2, 4, 2, param_lowCO2);
            dtostrf(highCO2, 4, 2, param_highCO2);
            dtostrf(lowRH, 4, 2, param_lowRH);
            dtostrf(highRH, 4, 2, param_highRH);
            dtostrf(lowTemp, 4, 2, param_lowTemp);
            dtostrf(highTemp, 4, 2, param_highTemp);
            dtostrf(sensorRate, 4, 0, param_sensorRate);
            dtostrf(compensateRate, 4, 0, param_compensateRate);
            dtostrf(dashboardRate, 4, 0, param_dashboardRate);
            dtostrf(batteryRate, 4, 0, param_batteryRate);
            dtostrf(pressure, 4, 2, param_pressure);

            DynamicJsonDocument json(1024);  //initialise json variable

            //copy values into json variable
            json["Default_Values"] = "1";
            json["param_lowCO2"] = param_lowCO2;
            json["param_highCO2"] = param_highCO2;
            json["param_lowRH"] = param_lowRH;
            json["param_highRH"] = param_highRH;
            json["param_lowTemp"] = param_lowTemp;
            json["param_highTemp"] = param_highTemp;
            json["param_sensorRate"] = param_sensorRate;
            json["param_compensateRate"] = param_compensateRate;
            json["param_dashboardRate"] = param_dashboardRate;
            json["param_batteryRate"] = param_batteryRate;
            json["param_pressure"] = param_pressure;

            File configFile = LittleFS.open("/config.json", "w");
            if (!configFile){
              Serial.println(F("Failed to open config file for writing default Config"));
            }

            serializeJson(json, Serial);
            serializeJson(json, configFile);  //write json to file
            configFile.close();
          }
        }else {
          Serial.println(F("Failed to load json config"));
        }
        configFile.close();
      }
    }else{
      Serial.println(F("Failed to mount File System - please reinstall firmware"));
    }
  }else{
    Serial.println(F("Failed to Start LittleFS - please reinstall firmware"));
  }
}

// callback notifying us of the need to save config
void saveConfigCallback() {
  Serial.println("Saving new config");
  shouldSaveConfig = true;

  //get values from hotspot website
  strcpy(IO_USERNAME, custom_IO_USERNAME.getValue());
  strcpy(IO_KEY, custom_IO_KEY.getValue());
  strcpy(IO_Dashboard, custom_IO_Dashboard.getValue());
  strcpy(Default_Values, "1");
  strcpy(param_lowCO2, custom_lowCO2.getValue());
  strcpy(param_highCO2, custom_highCO2.getValue());
  strcpy(param_lowRH, custom_lowRH.getValue());
  strcpy(param_highRH, custom_highRH.getValue());
  strcpy(param_lowTemp, custom_lowTemp.getValue());
  strcpy(param_highTemp, custom_highTemp.getValue());
  strcpy(param_sensorRate, custom_sensorRate.getValue());
  strcpy(param_compensateRate, custom_compensateRate.getValue());
  strcpy(param_dashboardRate, custom_dashboardRate.getValue());
  strcpy(param_batteryRate, custom_batteryRate.getValue());
  strcpy(param_pressure, custom_pressure.getValue());

  //convert character arrays stored in FS into float values used in calculations
  lowCO2 = atof(param_lowCO2);
  highCO2 = atof(param_highCO2);
  lowRH = atof(param_lowRH);
  highRH = atof(param_highRH);
  lowTemp = atof(param_lowTemp);
  highTemp = atof(param_highTemp);
  sensorRate = atof(param_sensorRate);
  compensateRate = atof(param_compensateRate);
  dashboardRate = atof(param_dashboardRate);
  batteryRate = atof(param_batteryRate);
  pressure = atof(param_pressure);

  Serial.print("saveConfigCallBack lowCO2 = "); Serial.println(lowCO2);

  DynamicJsonDocument json(1024);

  //copy values into json variable
  json["IO_KEY"] = IO_KEY;
  json["IO_USERNAME"] = IO_USERNAME;
  json["IO_Dashboard"] = IO_Dashboard;
  json["Default_Values"] = Default_Values;
  json["param_lowCO2"] = param_lowCO2;
  json["param_highCO2"] = param_highCO2;
  json["param_lowRH"] = param_lowRH;
  json["param_highRH"] = param_highRH;
  json["param_lowTemp"] = param_lowTemp;
  json["param_highTemp"] = param_highTemp;
  json["param_sensorRate"] = param_sensorRate;
  json["param_compensateRate"] = param_compensateRate;
  json["param_dashboardRate"] = param_dashboardRate;
  json["param_batteryRate"] = param_batteryRate;
  json["param_pressure"] = param_pressure;
  
  File configFile = LittleFS.open("/config.json", "w");
  if (!configFile){
    Serial.println(F("Failed to open config file for writing"));
  }

  serializeJson(json, Serial);
  serializeJson(json, configFile);  //write json to file
  configFile.close();
} // end save

//read WiFi and IO parameters from file stored in LittleFS filesystem on device
void readParamsFromFS() {
  Serial.println(F("Read Parameters from File"));

  if (LittleFS.begin())
  {
    if (LittleFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println(F("Reading config file"));

      File configFile = LittleFS.open("/config.json", "r");
      if (configFile)
      {
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        Serial.println();
        if (!deserializeError)
        {
          //copy values from json to variables for use later
          if (json.containsKey("IO_USERNAME"))
            strcpy(IO_USERNAME, json["IO_USERNAME"]);

          if (json.containsKey("IO_KEY"))
            strcpy(IO_KEY, json["IO_KEY"]);

          if (json.containsKey("IO_Dashboard"))
            strcpy(IO_Dashboard, json["IO_Dashboard"]);

          if (json.containsKey("param_lowCO2"))
            strcpy(param_lowCO2, json["param_lowCO2"]); 

          if (json.containsKey("param_highCO2"))
            strcpy(param_highCO2, json["param_highCO2"]);  

          if (json.containsKey("param_lowRH"))
            strcpy(param_lowRH, json["param_lowRH"]);  

          if (json.containsKey("param_highRH"))
            strcpy(param_highRH, json["param_highRH"]);  

          if (json.containsKey("param_lowTemp"))
            strcpy(param_lowTemp, json["param_lowTemp"]);  

          if (json.containsKey("param_highTemp"))
            strcpy(param_highTemp, json["param_highTemp"]);  

          if (json.containsKey("param_sensorRate"))
            strcpy(param_sensorRate, json["param_sensorRate"]);  

          if (json.containsKey("param_compensateRate"))
            strcpy(param_compensateRate, json["param_compensateRate"]);  

          if (json.containsKey("param_dashboardRate"))
            strcpy(param_dashboardRate, json["param_dashboardRate"]);  

          if (json.containsKey("param_batteryRate"))
            strcpy(param_batteryRate, json["param_batteryRate"]);  

          if (json.containsKey("param_pressure"))
            strcpy(param_pressure, json["param_pressure"]);  

          Serial.print("FS lowCO2 = "); Serial.println(param_lowCO2);

          //convert character array variables into float values used in calculations
          lowCO2 = atof(param_lowCO2);
          highCO2 = atof(param_highCO2);
          lowRH = atof(param_lowRH);
          highRH = atof(param_highRH);
          lowTemp = atof(param_lowTemp);
          highTemp = atof(param_highTemp);
          sensorRate = atof(param_sensorRate);
          compensateRate = atof(param_compensateRate);
          dashboardRate = atof(param_dashboardRate);
          batteryRate = atof(param_batteryRate);
          pressure = atof(param_pressure);

          //if IO dashboard configuration parameters dont exist, start the portal
          if((IO_USERNAME[0] == '\0') or (IO_KEY[0] == '\0') or (IO_Dashboard[0] == '\0')){
            Serial.printf("Adafruit IO values missing - reset WiFi Manager settings");
            startPortalFlag = true;
          }else{
            Serial.print("Username=");Serial.println(IO_USERNAME);
            Serial.printf("loaded Adafruit IO values for Username: %s , Dashboard: %s.\n", IO_USERNAME, IO_Dashboard);}
        }
        else
        {
          Serial.println(F("Failed to load json config - reset WiFi Manager settings"));
          startPortalFlag = true;
        }
        configFile.close();
      }
    }
    else
    {
      Serial.println(F("Failed to mount File System - please reinstall firmware"));
      wifiConnectedFlag = false;
      dashboard_Monitor = false;
    }
  }else{
    Serial.println(F("Failed to Start LittleFS - please reinstall firmware"));
    wifiConnectedFlag = false; 
    dashboard_Monitor = false; 
  }
}

//configure WiFi Manager parameters and either autoconnect to known WiFi or launch a hotspot to configure
void initWiFiManager() {
  Serial.println(F("\nstarting Wifi Manager"));

  //configure portal
  wifiManager.setClass("invert");          // enable "dark mode" for the config portal
  wifiManager.setConfigPortalTimeout(60); // auto close configportal after n seconds
  wifiManager.setConnectRetries(15);
  wifiManager.setAPClientCheck(true);      // avoid timeout if client connected to hotspot
  wifiManager.setWiFiAutoReconnect(true);
  wifiManager.setSaveConnectTimeout(5);
  wifiManager.setSaveConfigCallback(saveConfigCallback); // set config save notify callback
  wifiManager.setSaveConnect(true);
  wifiManager.setShowPassword(false);
  wifiManager.setTitle("WiFi Incubation Monitor");
  wifiManager.setParamsPage(true);
  wifiManager.setBreakAfterConfig(true);

  //custom dashboard parameters
  wifiManager.addParameter(&custom_dashboard_title);
  wifiManager.addParameter(&custom_IO_USERNAME); // set custom paraeter for IO username
  wifiManager.addParameter(&custom_IO_KEY);      // set custom parameter for IO key
  wifiManager.addParameter(&custom_IO_Dashboard);  // set custom parameter for CO2 feed
  
  //custom parameters for sensors
  wifiManager.addParameter(&custom_sensor_title);
  wifiManager.addParameter(&custom_lowCO2);
  wifiManager.addParameter(&custom_highCO2);
  wifiManager.addParameter(&custom_lowRH);
  wifiManager.addParameter(&custom_highRH);
  wifiManager.addParameter(&custom_lowTemp);
  wifiManager.addParameter(&custom_highTemp);
  wifiManager.addParameter(&custom_sensorRate);
  wifiManager.addParameter(&custom_compensateRate);
  wifiManager.addParameter(&custom_dashboardRate);
  wifiManager.addParameter(&custom_batteryRate);
  wifiManager.addParameter(&custom_pressure);
  
  //apply values to hotspot
  custom_IO_USERNAME.setValue(IO_USERNAME, 64); // set custom parameter value
  custom_IO_KEY.setValue(IO_KEY, 64);           // set custom parameter value
  custom_IO_Dashboard.setValue(IO_Dashboard, 64); // set custom parameter value

  custom_lowCO2.setValue(param_lowCO2, 64);
  custom_highCO2.setValue(param_highCO2, 64);
  custom_lowRH.setValue(param_lowRH, 64);
  custom_highRH.setValue(param_highRH, 64);
  custom_lowTemp.setValue(param_lowTemp, 64);
  custom_highTemp.setValue(param_highTemp, 64);
  custom_sensorRate.setValue(param_sensorRate, 64);
  custom_compensateRate.setValue(param_compensateRate, 64);
  custom_dashboardRate.setValue(param_dashboardRate, 64);
  custom_batteryRate.setValue(param_batteryRate, 64);
  custom_pressure.setValue(param_pressure, 64);

  initTFT();  //reset TFT display
  tft.println(F("Hotspot On"));

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
      strcpy (CO2_FeedName, "CO2_");
      strcat (CO2_FeedName, IO_Dashboard);  //create string identifier for data feed
      String CO2String((char*)CO2_FeedName);
      strlwr (CO2_FeedName);  //make lower case

      strcpy (Temp_FeedName, "TEMP_");
      strcat (Temp_FeedName, IO_Dashboard);  //create string identifier for data feed
      String TempString((char*)Temp_FeedName);
      strlwr (Temp_FeedName); //make lower case

      strcpy (RH_FeedName, "RH_");
      strcat (RH_FeedName, IO_Dashboard);  //create string identifier for data feed
      String RHString((char*)RH_FeedName);
      strlwr (RH_FeedName); //make lower case

      Serial.printf("\nCO2 data feed name: %s\n", CO2_FeedName);
      Serial.printf("Temperature data feed name: %s\n", Temp_FeedName);
      Serial.printf("Relative Humidity data feed name: %s\n", RH_FeedName);
      Serial.println("");

      Serial.print("CO2string: ");Serial.print(CO2String);Serial.print(" : ");Serial.print("String isnt empty: ");Serial.println(CO2String != "CO2_");
      Serial.print("TempString: ");Serial.print(TempString);Serial.print(" : ");Serial.print("String isnt empty: ");Serial.println(TempString != "TEMP_");
      Serial.print("RHString: ");Serial.print(RHString);Serial.print(" : ");Serial.print("String isnt empty: ");Serial.println(RHString != "RH_");

      //if dashboad feed name was created, then connect to dashboard, otherwise dont
      if((CO2String != "CO2_") && (TempString != "TEMP_") && (RHString != "RH_")){
        io = new (objStorage) AdafruitIO_WiFi(IO_USERNAME, IO_KEY, "", ""); //create IO object with user details for subsequent connection
        Serial.printf("\nConnecting to Adafruit IO with User: %s Key: %s Dashboard: %s.\n", IO_USERNAME, IO_KEY, IO_Dashboard);

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
        CO2_Feed = io->feed(CO2_FeedName);
        Temp_Feed = io->feed(Temp_FeedName);
        RH_Feed = io->feed(RH_FeedName);

        Serial.println(F("Adafruit IO connection established\n"));
        if(firstRun){
          tft.setCursor(cursorX0, cursorY0);
          tft.println(F("IO Success"));
          delay(1000);
        }
        ioConnectedFlag = true;          // successful connection
      }else{
        Serial.println(F("Adafruit IO values missing - disable dashboard upload"));
        Serial.print(F("IO_USERNAME: "));Serial.println(IO_USERNAME);
        Serial.print(F("IO_KEY: "));Serial.println(IO_KEY);
        Serial.print(F("IO_Dashboard: "));Serial.println(IO_Dashboard);
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
  /*else{
    wifiConnectedFlag = false;
    ioConnectedFlag = false;
  }
  */
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
    }
  }
}