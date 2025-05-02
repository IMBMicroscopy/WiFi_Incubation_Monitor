//sensor functions

void initSensors(){
  Serial.println(F("initialise sensors..."));
  initTFT();
  tft.setTextSize(3);  //1 (small) - 4(biggest) text size

  if(pressure_Monitor){
    for(int i=0;i <=5;i++) {  
      if(myBME280.begin(0x76, &Wire) || myBME280.begin(0x77, &Wire)){
        Serial.println(F("Found BME280 sensor."));
        tft.println(F("Found BME280"));
        bme280Exists = true;

        myBME280.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X4, // temperature
                    Adafruit_BME280::SAMPLING_X4, // pressure
                    Adafruit_BME280::SAMPLING_X4, // humidity
                    Adafruit_BME280::FILTER_X4   );

        myBME280.MODE_SLEEP;
        myBME280.setTemperatureCompensation(bme280_offsetTemp);

        break;
      }
      delay(200);
    }
    if(!bme280Exists){
      Serial.println(F("Couldn't find BME280 sensor!"));
      pressure_Monitor = false;
      bme280Exists = false;
      tft.println(F("Fail BME280"));
    }
  }
  delay(500);

  //find low pressure sensor if enabled in settings
  if(low_CO2_Monitor){
    for(int i=0; i<=5; i++){
      if(mySCD41.begin()) {
        Serial.println(F("Found SCD41 sensor."));
        tft.println(F("Found SCD41"));
        scd41Exists = true;
        //mySCD41.stopPeriodicMeasurement();
        mySCD41.startPeriodicMeasurement();
        mySCD41.setTemperatureOffset(SCD41_offsetTemp);
        break;
      }
      delay(200);
    }
    if(!scd41Exists){
      Serial.println(F("SCD41 sensor not detected."));
      low_CO2_Monitor = false;
    }
  }
  delay(500);

  //find high CO2 sensor if enabled in settings
  if(high_CO2_Monitor){
    for(int i=0; i<=5; i++){
      if (mySTC31.begin()){
        Serial.println(F("Found STC31 sensor."));
        tft.println(F("Found STC31"));
        stc31Exists = true;
        break;
      }
      delay(200);
    }
    if(!stc31Exists){
      Serial.println(F("STC31 sensor not detected."));
      tft.println(F("Fail STC31"));
      high_CO2_Monitor = false;
    }

    if(high_CO2_Monitor){
      // Try SHT4 sensor
      for (int i = 0; i <= 5; i++) {
        if (mySHT4.begin()) {
          Serial.println(F("Found SHT4 sensor."));
          tft.println(F("Found SHT4"));
          sht4Exists = true;
          break;
        }
        delay(500);
      }

      // If SHT4 not found, try SHTC3
      if (!sht4Exists) {
        for (int i = 0; i <= 5; i++) {
          if (mySHTC3.begin() == SHTC3_Status_Nominal) {
            Serial.println(F("Found SHTC3 sensor."));
            tft.println(F("Found SHTC3"));
            shtC3Exists = true;
            break;
          }
          delay(500);
        }
      }

      // If neither sensor is found
      if (!sht4Exists && !shtC3Exists) {
        Serial.println(F("SHT4/SHTC3 not detected."));
        tft.println(F("Fail SHT"));
        high_CO2_Monitor = false;
      }    
    }

    if(high_CO2_Monitor){
      //We need to tell the STC3x what binary gas and full range we are using
      //Possible values are:
      //  STC3X_BINARY_GAS_CO2_N2_100   : Set binary gas to CO2 in N2.  Range: 0 to 100 vol%
      //  STC3X_BINARY_GAS_CO2_AIR_100  : Set binary gas to CO2 in Air. Range: 0 to 100 vol%
      //  STC3X_BINARY_GAS_CO2_N2_25    : Set binary gas to CO2 in N2.  Range: 0 to 25 vol%
      //  STC3X_BINARY_GAS_CO2_AIR_25   : Set binary gas to CO2 in Air. Range: 0 to 25 vol%

      for(int i=0; i<=5; i++){
        if (mySTC31.setBinaryGas(STC3X_BINARY_GAS_CO2_AIR_25) == true){
          Serial.println(F("Binary gas set"));
          gasExists = true;
          break;
        }
        delay(200);
      }
      if(!gasExists){
        Serial.println(F("Could not set the binary gas!"));
        high_CO2_Monitor = false;
      }
    }
  }
  Serial.println(F(""));
}

void recalSensor() {
  //calculate elapsed timers used below
  elapsedCompensateTime = (millis() - lastCompensateTime)/1000;
  
  //recalibrate CO2 sensor if required
  if(firstRun || (elapsedCompensateTime >= compensateRate)){
    lastCompensateTime = millis();

    //report elapsed time
    Serial.println(F("")); Serial.print(F("Elapsed Time = ")); Serial.print(hour); Serial.print(F("h:")); Serial.print(minute); Serial.print(F("m")); Serial.print(second); Serial.println(F("s"));
    
    compensate(); //compensate CO2 measurement for Temp, RH and Pressure
  }
}

float BME280_correctedRH(float RH) {
  if (bme280_calibrateRH) {
    // Function to calibrate the BME280 humidity against a known standard (or the STC31)
    // Calculate correction factor (slope) and offset
    float a = (bme280_high_reference - bme280_low_reference) / (bme280_high_reading - bme280_low_reading);
    float b = bme280_high_reference - (a * bme280_high_reading);

    // Apply correction 
    float cRH = (a * RH) + b;
    return cRH;
  } else {
    return RH;
  }
}

float SHT_correctedRH(float RH) {
  if (sht_calibrateRH) {
    // Function to calibrate the SHT humidity against a known standard
    float a = (sht_high_reference - sht_low_reference) / (sht_high_reading - sht_low_reading);
    float b = sht_high_reference - (a * sht_high_reading);

    // Apply correction 
    float cRH = (a * RH) + b;
    return cRH;
  } else {
    return RH;
  }
}

float SCD41_correctedRH(float RH) {
  if (SCD41_calibrateRH) {
    // Function to calibrate the SCD41 humidity against a known standard
    float a = (SCD41_high_reference - SCD41_low_reference) / (SCD41_high_reading - SCD41_low_reading);
    float b = SCD41_high_reference - (a * SCD41_high_reading);

    // Apply correction 
    float cRH = (a * RH) + b;
    return cRH;
  } else {
    return RH;
  }
}

void readBME() {
  for(int i=0;i <=20;i++) {  
    if(myBME280.takeForcedMeasurement()){
      delay(50);
      bmeTemp = myBME280.readTemperature();
      bmePress = myBME280.readPressure();
      bmeRH = myBME280.readHumidity();
      bmeCRH = BME280_correctedRH(bmeRH);
      
      if(!compSerialFlag){
        Serial.print(F("BME280: "));
        Serial.print(F("Temperature: ")); Serial.print(bmeTemp,1); Serial.print('C');
        Serial.print(F("\tHumidity: ")); Serial.print(bmeCRH, 0); Serial.print(F("%"));
        Serial.print(F("\t\t\tPressure: ")); Serial.print(bmePress*.01, 1); Serial.println(F("mBar"));
      }
      break;
    }
    if(i == 10){Serial.println(F("bme280 read failed, use last values"));}
    delay(500);
  }
}

void readSCD41(){
  //mySCD41.measureSingleShot();
  //delay(5000);  //required for low power single shot mode
  delay(20);
  for(int i=0;i <=20;i++) {  
    if(mySCD41.readMeasurement()) {// readMeasurement will return true when fresh data is available
      delay(20);
      scdTemp = mySCD41.getTemperature();
      scdCO2 = mySCD41.getCO2()*0.0001;
      scdRH = mySCD41.getHumidity();
      scdCRH = SCD41_correctedRH(scdRH);
      
      if(!compSerialFlag){
        Serial.print(F("SCD41: "));
        Serial.print(F("\tTemperature: ")); Serial.print(scdTemp, 1); Serial.print(F("C"));
        Serial.print(F("\tHumidity: ")); Serial.print(scdCRH, 0); Serial.print(F("%"));
        Serial.print(F("\tCO2: ")); Serial.print(scdCO2, 2); Serial.print(F("%"));
        Serial.println();
      }
      break;
    }
    if(i == 10){Serial.println(F("SCD41 read failed, use last values"));}
    delay(500);
  }
}

void readSTC31() {
  for(int i=0;i <=20;i++) {  
    if(sht4Exists){
      if(mySHT4.getTemperatureSensor()) {break;}
    }else{
      if(mySHTC3.update() == SHTC3_Status_Nominal) {break;} //request a measurement
    }
    if(i == 10){Serial.println(F("SHT read failed, use last values"));}
    delay(500);
  }
  
  for(int i=0;i <= 20;i++) {  
    if (mySTC31.measureGasConcentration()) { // measureGasConcentration will return true when fresh data is available
      delay(50);
      stcCO2 = roundf((mySTC31.getCO2() + stc31_offsetCO2) * 10) / 10.0; //get CO2 reading and apply offset value and round to 1 decimal place
      stcCO2 = constrain(stcCO2, 0, 100); //constrain value within real reading possibilities

      if(sht4Exists){
        sensors_event_t humidity, temp;
        mySHT4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
        stcTemp = temp.temperature;
        stcRH = humidity.relative_humidity;
      }else{
        stcTemp = mySHTC3.toDegC();
        stcRH = mySHTC3.toPercent();
      }
      stcTemp = stcTemp + sht_offsetTemp;
      stcCRH = SHT_correctedRH(stcRH);

      if(!compSerialFlag){
        Serial.print(F("STC31: "));
        Serial.print(F("\tTemperature: ")); Serial.print(stcTemp, 1); Serial.print(F("C"));
        Serial.print(F("\tHumidity: ")); Serial.print(stcCRH, 0); Serial.print(F("%"));
        Serial.print(F("\tCO2: ")); Serial.print(stcCO2, 2); Serial.print(F("%"));
        Serial.println(F(""));
      }
      break;
    }
    if(i == 10){Serial.println(F("STC31 read failed, use last values"));}
    delay(500);
  }
}

void readSensors() {
  elapsedSensorTime = (millis() - lastSensorTime)/1000;
  //take sensor readings on first run and then at sensorRate
  if(firstRun || (elapsedSensorTime >= sensorRate)) {
    lastSensorTime = millis(); //update timer

    //report elapsed time
    Serial.println(F("")); Serial.print(F("Elapsed Time = ")); Serial.print(hour); Serial.print(F("h:")); Serial.print(minute); Serial.print(F("m")); Serial.print(second); Serial.println(F("s"));
    

    if(pressure_Monitor){
      //can use a live measurement if you include a pressure sensor, or define average value at beginning of code
      readBME();
    }

    if(low_CO2_Monitor){
      //read values from SCD41 CO2 board
      readSCD41();
    }

    if(high_CO2_Monitor){
      //read values from STC31 CO2 board
      readSTC31();
    }
    
    //use the SCD41 CO2 sensor for low CO2 values if available
    if (low_CO2_Monitor && scdCO2 >= 0 && ((scdCO2 <= switchCO2Sensors) || !high_CO2_Monitor)) {
      myCO2 = scdCO2;
    } else {
      myCO2 = stcCO2;
    }
    //if the stc31 high CO2 sensor doesnt exist use the scd41 low CO2 sensor for readings
    if(low_CO2_Monitor && !high_CO2_Monitor){
      myTemp = scdTemp;
      myRH = scdCRH;
    }else{
      myTemp = stcTemp;
      myRH = stcCRH;
    }

    //if neither co2 sensor exists use the bme280 sensor
    if(pressure_Monitor && !low_CO2_Monitor && !high_CO2_Monitor){
      myCO2 = 0.0;
      myTemp = bmeTemp;
      myRH = bmeCRH;
    }

    if(pressure_Monitor){
      myPress = bmePress *0.01;
    }else{myPress = pressure;}

    Serial.print(F("Report: "));
    Serial.print(F("Temperature: ")); Serial.print(myTemp, 1); Serial.print(F("C"));
    Serial.print(F("\tHumidity: ")); Serial.print(myRH, 0); Serial.print(F("%"));
    Serial.print(F("\tCO2: ")); Serial.print(myCO2, 2); Serial.print(F("%"));
    Serial.print(F("\tPressure: ")); Serial.print(myPress, 1); Serial.print(F("mBar"));
    Serial.println(F(""));
  }
}

//Compensate the CO2 reading for temperature and relative humidity using the readings from the SHT as well as a static pressure measurement
void compensate() {
  Serial.println(F("Running compensation routine..."));
  compFlag = true;  //flag to indicate calibration has been performed on TFT
  compSerialFlag = true;

  //take sensor readings, pressure required for CO2 compensation
  if(pressure_Monitor){
    //read values from BME280 board
    readBME();  
  }

  if(high_CO2_Monitor){
    readSTC31();

    //include Temperature in CO2 calculation
    Serial.print(F("Setting STC3x temperature to ")); Serial.print(stcTemp, 2); Serial.print(F("C was "));
    if(mySTC31.setTemperature(stcTemp) == false){Serial.print(F("not "));}
    Serial.println(F("successful"));

    //include Relative Humidity in CO2 calculation
    Serial.print(F("Setting STC3x Humidity to ")); Serial.print(stcCRH, 2); Serial.print(F("% was "));
    if (mySTC31.setRelativeHumidity(stcCRH) == false){Serial.print(F("not "));}
    Serial.println(F("successful"));

    //include Pressure measurement in CO2 calculation.
    //can use a live measurement if you include a pressure sensor, or define average value at beginning of code
    if(bme280Exists && (bmePress*0.01 > 600) && (bmePress*.01 < 1200)) {pressure = bmePress*0.01;} //use actual pressure from BME280 vs estimated pressure value in mbar
    Serial.print(F("Setting STC3x pressure to ")); Serial.print(pressure); Serial.print(F("mBar was "));
    if (mySTC31.setPressure(pressure) == false){Serial.print(F("not "));}
    Serial.println(F("successful"));
  }

  //update SCD41 readings using calibrated pressure in pascals
  if(low_CO2_Monitor){
    readSCD41();

    if(bme280Exists && (bmePress > 60000) && (bmePress < 120000)) {pascals = bmePress;}else{pascals = pressure*100;} //use actual pressure from BME280 vs estimated pressure value
    Serial.print(F("Setting SCD41 pressure to ")); Serial.print(pascals); Serial.print(F(" Pa"));
    if (mySCD41.setAmbientPressure(pascals) == false){Serial.print(F(" not"));}
    Serial.println(F(" successful"));

  }
  compSerialFlag = false;
  delay(2000);
}

void initBattery() {
  const int maxAttempts = 5; // Max retries for initialization
  const int retryDelayMs = 1000; // Delay between retries in milliseconds

  if (!battery_Monitor) {
    return; // Skip initialization if monitoring is disabled
  }

  for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
    if (maxlipo.begin()) {
      Serial.print(F("Found MAX17048 with Chip ID: 0x"));
      Serial.println(maxlipo.getChipID(), HEX);
      tft.println(F("Found BMS"));
      return; // Exit early on success
    }

    Serial.println(F("Couldn't find Adafruit MAX17048. Make sure a battery is plugged in!"));

    if (attempt == maxAttempts) {
      battery_Monitor = false;
      tft.println(F("Fail BMS"));
    }

    delay(retryDelayMs);
  }
}

void batteryMonitoring() {
  elapsedBatteryTime = (millis() - lastBatteryTime)/1000;
  //measure battery
  if(battery_Monitor && (firstRun || (elapsedBatteryTime >= batteryRate))){
    lastBatteryTime = millis(); //update timer

    //report elapsed time
    Serial.println(F("")); Serial.print(F("Elapsed Time = ")); Serial.print(hour); Serial.print(F("h:")); Serial.print(minute); Serial.print(F("m")); Serial.print(second); Serial.println(F("s"));
    

    //If required wake the Battery Monitor from Hibernation
        if (maxlipo.isHibernating()) {
      Serial.println(F("Waking Battery Monitor"));
      maxlipo.wake(); //wake up the battery monitor
    }

    //read Battery monitoring values
    battVoltage = maxlipo.cellVoltage();
    battPercent = maxlipo.cellPercent();
    battRate = maxlipo.chargeRate();

    //Check to see if a battery is connected
    if ((battVoltage >= 4.2) || isnan(battVoltage) || (battPercent >= 102) || ((battVoltage >= 4.1) && (battPercent >= 101)) || (battRate > 55) || (battVoltage == 0.0 && battPercent == 0.0 && battRate == 0.0)) {
      Serial.println(F("Battery is either Full or not connected"));
      battExistsFlag = false;
    }else{battExistsFlag = true;}


    //if a battery is connected, report values on serial port
    if(battExistsFlag){
      Serial.print(F("Batt Volts: ")); Serial.print(battVoltage, 1); Serial.print(F("V"));
      Serial.print(F("\tBatt %: ")); Serial.print(battPercent, 1); Serial.print(F("%"));
      Serial.print(F("\tCharge rate : ")); Serial.print(battRate, 1); Serial.println(F("%/hr"));
    }
    maxlipo.hibernate();
  }
}
