//general functions

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
    tft.print(myTemp, 1); tft.print("C");
    if(compFlag){tft.println(" *");}else{tft.println("");}

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
    tft.print(myRH, 0); tft.print("%");

    if(compFlag){tft.println(" *");}else{tft.println("");}
    
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

    //display Wifi text if required
    if(!battExistsFlag){
      tft.setTextSize(1);
      tft.println("");
    }
    tft.setTextSize(3);
    if(wifiConnectedFlag){
      tft.setTextColor(ST77XX_GREEN);
    }else{
      tft.setTextColor(ST77XX_RED);  
    }
    tft.print("WiFi ");
  
    //display IO text if required
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
