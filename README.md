
  Monitor environmental conditions for live samples using and ESP32 device with STC31 CO2 sensor and SHTC3 temperature and relative humidity sensors
  Calibrate the CO2 sensor using Temperature, Humidity and Pressure
  Monitor LiPo battery SOC and discharge rate
  Report values to TFT display, serial port and online dashboard

  An Adafruit IO account is required for logging of incubation conditions over time: https://accounts.adafruit.com/users/sign_in
  Each device will record the conditions for ONE incubator
  First you will need to create a unique data feed for each parameter: CO2, Temperature and Relative Humidity
  Then create a dashboard for each incubator with a graph to log the CO2, Temperature and Humidity values from the feeds
  The CO2, Temperature and RH values must be assigned to a unique data feed name in the Adafruit IO dashboard
  ie: the Dashboard for the incubator would be the name of the microscope, ie: "Live Imager 1" and the CO2 data feed should be called "CO2_Live_Imager_1"
  ie: the Dashboard for "Live Imager 2" and the CO2 data feed should be called "CO2_Live_Imager_2", or something similar

  Adafruit IO allows you to publish read only versions of your dashboards to other websites for monitoring incubation conditions as well as configuring actions to send alert emails when conditions exceed limits

  

  Required hardware:
  Arduino compatible hardware such as: the following tested hardware
  Adafruit Feather ESP32-S3TFT mainboard such as https://learn.adafruit.com/adafruit-esp32-s3-tft-feather/overview
  STC31 CO2 sensor board such as https://www.sparkfun.com/products/18385
  Sparkfun Qwiic connector 500mm (or longer) cable: https://www.sparkfun.com/products/17257
  This minimum configuration requires no soldering or electronics knowledge

  Required software libraries:
  "SparkFun_STC3x_Arduino_Library.h" CO2 sensor library http://librarymanager/All#SparkFun_STC3x
  "SparkFun_SHTC3.h" Temperature and Relative Humidity sensor library http://librarymanager/All#SparkFun_SHTC3
  "Adafruit_GFX.h" Core graphics library for TFT display http://librarymanager/All#Adafruit_GFX_Library
  "Adafruit_ST7789.h" Hardware-specific library for ST7789 controller http://librarymanager/All#Adafruit_ST7789
  "SPI.h" built in library for SPI comms to tft display

  Optional:
  3D printed case: https://www.printables.com/model/386400-adafruit-esp32-s2-reverse-tft-feather-case
  
  LiPo Battery for 3.7V - 4.2V such as 603450 with JST 2pin connector
  "Adafruit_MAX1704X.h" library for lipo battery measurements http://librarymanager/All#Adafruit_MAX1704X

  "WiFi.h" library for inbuit wifi comms to online dashboard

  Ethernet featherwing such as https://www.adafruit.com/product/3201
  Ethernet Library http://librarymanager/All#Ethernet

  Atmospheric pressure sensor such as a BME280 sensor https://www.sparkfun.com/products/15440
  Adafruit BME280 Library for Environmental sensor http://librarymanager/All#BME280
  

  Incubation Monitor Setup Instructions

  Download and Install Arduino IDE: https://www.arduino.cc/en/software
  Follow the instructions here: https://learn.adafruit.com/esp32-s3-reverse-tft-feather/arduino-ide-setup-2
  To configure the Arduino IDE and to install the required drivers for the ESP32 board

  Install required plugins for ESP32 and sensors:

  Required software libraries:
  Adafruit IO Arduino Library http://librarymanager/All#Adafruit_IO_Arduino
  "SparkFun_STC3x_Arduino_Library.h" CO2 sensor library http://librarymanager/All#SparkFun_STC3x
  "SparkFun_SHTC3.h" Temperature and Relative Humidity sensor library http://librarymanager/All#SparkFun_SHTC3
  "Adafruit_GFX.h" Core graphics library for TFT display http://librarymanager/All#Adafruit_GFX_Library
  "Adafruit_ST7789.h" Hardware-specific library for ST7789 controller http://librarymanager/All#Adafruit_ST7789
  "SPI.h" built in library for SPI comms to tft display

  Optional plugins:
  "Adafruit_MAX1704X.h" library for lipo battery measurements http://librarymanager/All#Adafruit_MAX1704X
  SparkFun SCD41 library http://librarymanager/All#SparkFun_SCD4x
  "WiFi.h" library for inbuit wifi comms to online dashboard
  Ethernet Library http://librarymanager/All#Ethernet
  Adafruit BME280 Library for Environmental sensor http://librarymanager/All#BME280
  "Adafruit_MAX1704X.h" library for lipo battery measurements http://librarymanager/All#Adafruit_MAX1704X



  Create a SPIFFS/LittleFS partition for storing wifi credentials: https://randomnerdtutorials.com/arduino-ide-2-install-esp32-littlefs/
    Copy the VSIX file to the appropriate windows or Mac/Linux folder: https://github.com/earlephilhower/arduino-littlefs-upload
    Restart the Arduino IDE
    Open the "Incubation_Monitor" sketch
    From the Sketch Menu, Click "Show Sketch Folder" to open the folder containing the "incubation monitor" sketch
    Ensure there is subfolder called "data" which contains an empty file "config.json", if it doesnt, then create it.
    Connect the ESP32 device and sensors via USB to the PC
    In Arduino IDE, ensure the "Adafruit Feaher Reverse TFT S3" board is selected and the correct port is selected
    From the Tools Menu, select Partition Scheme: "Minimal SPIFFS - 1.9MB APP with OTA"
    From the Tools Menu, select "Erase all Flash before Sketch Upload - Enabled"
    Ensure there is no Serial Monitor interface open in the Arduino IDE
    Press (Windows [Ctrl] + [Shift] + [P]) or (Mac [⌘] + [Shift] + [P]) to open the command palette. 
    An instruction called ‘Upload Little FS to Pico/ESP8266/ESP32‘ should be there (just scroll down or search for the name of the instruction). 
    Click ‘Upload Little FS to Pico/ESP8266/ESP32‘ to format the ESP32 SPIFFS partition to enable storing of data.
    You may have to try a few times to successfully upload the littleFS filesystem, and you may have to disconnect/reconnect the USB cable and restart the Arduino IDE and upload again, if the uploader fails to connect
    From the Tools Menu, select "Erase all Flash before Sketch Upload - Disabled"

  Configure the "config.h" file parameters as required for your measurement requirements
  
  Upload "Incubation_Monitor" sketch to ESP32 hardware and restart the device when the upload is complete
    Click on the Arduino IDE serial monitor and ensure it is set to "Both NL & CR" and "115200 baud" to monitor the device setup progress
    
    On your PC/Phone/Tablet select the new temporary WiFi network "Incubation Setup"
    A popup window should appear, click "WiFi Setup"
    Complete the required fields for Wifi and Adafruit IO

  The device should now connect to Adafruit IO and begin logging incubation conditions

To reset the Wifi and/or Adafruit parameters, press the Reset button on the ESP32 device, then press and hold the D0 button until the "Incubation Setup" Wifi access point appears on your network.
Then reenter the parameters in the "Wifi setup" popup window.



