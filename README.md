# WiFi enabled Incubation Monitor

![updated incubation monitor](https://github.com/user-attachments/assets/96d33d65-d92c-4610-86c6-9e21cd06d4ff)

![Image](https://github.com/user-attachments/assets/02aba95f-da6f-4bfc-8eb2-71706fd93b8e)

![Image](https://github.com/user-attachments/assets/93f48372-cce1-4dd0-94da-fd26b47a517c)

## Monitor environmental conditions for live samples using an ESP32 device with STC31 CO2 sensor and SHTC3 temperature and relative humidity sensors

- Calibrate the CO2 sensor using Temperature, Humidity and Pressure
- Monitor LiPo battery SOC and discharge rate
- Report values to the TFT display, serial port and online dashboard
- **Each device will record the conditions for ONE incubator**
- **A free Adafruit IO account is required for logging of incubation conditions over time to the online dashboard**
  - https://accounts.adafruit.com/users/sign_in
  - You will then need the Username and Active Key values to load into the ESP32 device to enable data streaming.
  - **This must be done before configuring the hardware below**

**Required hardware:**

- Arduino compatible hardware such as: the following tested hardware
- Adafruit Feather ESP32-S3 reverse TFT mainboard such as https://learn.adafruit.com/esp32-s3-reverse-tft-feather (the code for the TFT has been optimised for this display size/resolution and will require modification for other displays)
- STC31 CO2 sensor board such as [https://www.sparkfun.com/products/18385](https://www.sparkfun.com/products/18385) or https://sensirion.com/products/catalog/SEK-STC31-C 
- Sparkfun Qwiic connector 500mm (or longer) cable: [https://www.sparkfun.com/products/17257](https://www.sparkfun.com/products/17257) (or you may manually solder the cables)
- This minimum configuration requires no soldering or electronics knowledge

**Required software libraries:**

- "SPI.h" built in library for SPI comms to tft display

- "WiFi.h" library for inbuit wifi comms to online dashboard

- "Preferences.h" for saving parameters to Flash for reuse later

- "SparkFun_STC3x_Arduino_Library.h" CO2 sensor library [http://librarymanager/All#SparkFun_STC3x](http://librarymanager/All#SparkFun_STC3x)

- "SparkFun_SHTC3.h" Temperature and Relative Humidity sensor library [http://librarymanager/All#SparkFun_SHTC3](http://librarymanager/All#SparkFun_SHTC3)

- "Adafruit_SHT4x.h" //SHT4x Temp/RH sensor library for use with the SEK-STC31-C development kit http://librarymanager/All#Adafruit SHT4x

- "Adafruit_GFX.h" Core graphics library for TFT display [http://librarymanager/All#Adafruit_GFX_Library](http://librarymanager/All#Adafruit_GFX_Library)

- "Adafruit_ST7789.h" Hardware-specific library for ST7789 controller [http://librarymanager/All#Adafruit_ST7789](http://librarymanager/All#Adafruit_ST7789)

- Adafruit IO Arduino Library [http://librarymanager/All#Adafruit_IO_Arduino](http://librarymanager/All#Adafruit_IO_Arduino)

  - "AdafruitIO_WiFi.h" for data feed functionality
  - "WiFiManager.h" for configuration of the sensors and WiFi

**Optional:**

- 3D printed case: AutoDesk Fusion https://a360.co/4j8FKbL or see included .3mf file
- Atmospheric pressure BME280 sensor [https://www.sparkfun.com/products/15440](https://www.sparkfun.com/products/15440) to measure atmospheric pressure for CO2 sensor calibration (also used for room T and RH monitoring)
- Adafruit BME280 Library for Environmental sensor [http://librarymanager/All#BME280](http://librarymanager/All#BME280)
- Low concentration CO2 sensor SCD41 [https://www.sparkfun.com/sparkfun-co-humidity-and-temperature-sensor-scd41-qwiic.html](https://www.sparkfun.com/sparkfun-co-humidity-and-temperature-sensor-scd41-qwiic.html)
- "SparkFun_SCD4x_Arduino_Library.h"  SCD41 CO2 sensor library [http://librarymanager/All#SparkFun_SCD4x](http://librarymanager/All#SparkFun_SCD4x)
- LiPo Battery for 3.7V - 4.2V such as 603450 with JST 2pin connector https://core-electronics.com.au/polymer-lithium-ion-battery-1000mah-38458.html
- "Adafruit_MAX1704X.h" library for lipo battery measurements [http://librarymanager/All#Adafruit_MAX1704X](http://librarymanager/All#Adafruit_MAX1704X)
- Additional short and long STEMMA/QWIIC connector I2C cables for the BME280 and SCD41 sensor boards as required (or just makeup and solder your own)

<br />
<br />

**Incubation Monitor Setup Instructions:**

**Configure Arduino IDE**

- Download and Install Arduino IDE: [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)
- Follow the instructions here: [https://learn.adafruit.com/esp32-s3-reverse-tft-feather/arduino-ide-setup-2](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/arduino-ide-setup-2), to configure the Arduino IDE and to install the required drivers for the ESP32 board
- Using the library manager, search for and install required plugins for ESP32 and sensors as listed above
- Restart the Arduino IDE

**Configure and upload code**

- Within Arduino IDE, open the WiFi_Incubation_Monitor.ino file and wait for the associated files to load.
- Connect the sensors to the ESP32 using the I2C cable
- Connect the ESP32 to the computer via USB cable
- Go to "Tools" menu and select "Board" and select "ESP32", then select the "Adafruit Feather ESP-32 S3 Reverse TFT" board
- Go to "Tools" menu and select "Port" and then select the device that contains the "Adafruit Feather"
- IF you have the SEK-STC31-C development sensor board for CO2 measurement, then you need to uncomment "#define SEK" in the initialise.ino file, else it should be commented out with "//".
- Upload "Incubation_Monitor" sketch to ESP32 hardware (use the arrow button in the top left corner of the IDE), the device will restart when the upload is complete (if it doesnt, press the RST button on the ESP32)
- Click on the Arduino IDE serial monitor (top right corner of IDE) and ensure it is set to "Both NL & CR" and "115200 baud" to monitor the device setup progress
- The Mac address for the device will be displayed in the serial monitor, you may need to register this with your IT department to gain WiFi access to hidden WiFi networks
- For new hardware installs, the ESP32 will automatically create a hotspot as configured in the WiFi_Incubation_Monitor.ino file, default is "Incubation_Setup"

**Configure Hardware (ESP32)**
<br />

- On your PC/Phone/Tablet select the new temporary WiFi network "Incubation Setup", enter the password as per WiFi_Incubation_Monitor.ino and a popup window should appear after a few seconds

<br />
  
- **Dashboard users**
  - First, if you intend to use the Adafruit IO Dashboard, record your IO username and key, and decide on your dashboard name (name of incubator/microscope etc)
  - It's critical that you are within coverage range of your nearest WiFi access point in order to connect to the AP and then the IO Dashboard.
  - This is important, First, Click the "Setup" button to configure hardware parameters:
    - Enter the IO Dashboard values (**Username, Key and Dashboard name**)
    - Ensure "dashboard monitor" feature is toggled to "true"
    - Toggle the required installed sensors and their sensor parameters if they differ from the default settings in WiFi_Incubation_Monitor.ino file.
    - For further information about these parameters, consult the WiFi_Incubation_Monitor.ino file.
  - Click "Save", then click the "back arrow" button twice to go back to the home page.
  - Now, Click "Configure WiFi" to enter the SSID name and password, then click "Save".
  - The device will now attempt to connect to WiFi and if successful will close the popup window 
  - The device will then attempt to connect to the IO dashboard, before initialising the sensors and begin acquiring data.

<br />

- **Non Dashboard users**   
  - Click the "Setup" button to configure hardware parameters:
    - Ensure "dashboard monitor" feature is toggled to "false"
    - Toggle the required installed sensors and their sensor parameters if they differ from the default settings in WiFi_Incubation_Monitor.ino file.
    - For further information about these parameters, consult the WiFi_Incubation_Monitor.ino file.
  - Click "Save", then click the "back arrow" button twice to go back to the home page.
  - Now, Click "Exit" and the hotspot will close after about 10 seconds, the device will now initialise the sensors and begin acquiring data
  
<br />

**Adjust Parameters**

- To adjust the Wifi and/or Adafruit parameters after initial setup and at any time, press the Reset button on the ESP32 device,
- Then when "Config -&gt;" appears on the display, press and hold the top right D0 button for one second,
- The "Incubation Setup" Wifi access point will appear on your network after a few seconds.
- Enter the hotspot password configured in the WiFi_Incubation_Monitor.ino file
- Then adjust the parameters in the "setup" and "Wifi setup" pages as required, then click Save
- To edit only the custom parameters on the setup page, you will first modify the parameters, click save, then go back to the homepage and click exit.

**Configure Adafruit IO Dashboard**

- If you havent already, you will need to create an account: https://accounts.adafruit.com/users/sign_in
- You will then need the Username and Active Key values to load into the ESP32 device to enable data streaming.
- Each incubation monitor will generally require a uniquely named dashboard to display its data feeds, these feeds are configured on the device after following the Incubation Monitor Setup Instructions steps above.
- You will need to create a unique data feed name prefix for each ESP32 based Incubation Monitor device, the device will then automatically create data feeds for each parameter: CO2, Temperature and Relative Humidity
- Once the ESP32 device is running and has started transmitting the data feeds, they should appear in the Data Feeds section of Adafruit IO
- Then you will need to create an Adafruit IO dashboard for each incubator with a graph to log the CO2, Temperature and Humidity values from the data feeds
- The CO2, Temperature and RH values must be assigned to a unique data feed name in the Adafruit IO dashboard
- ie: the Dashboard for the incubator would be the name of the microscope, ie: "Live Imager 1" and the data feed name prefix will be something like LI1 and will automatically create "LI1_probe_CO2", "LI1_probe_Temp" and "LI1_probe_RH"
- These will be the CO2, Temp and RH data feeds for the STC31 sensor on the long cable which is placed in the imaging chamber
- You can also enable "room_monitoring" in the configuration hotspot settings to create additional feeds from the optional bme280 sensor mounted in the 3D printed case, ie: "LI1_room_Temp", "LI1_room_RH" and "LI1_room_press"
- This can be used to monitor the environmental conditions in the microscope room, where the display unit is mounted, rather than in the imaging chamber
- Once the Dashboard is created, Create a new block such as a line graph and link them to the relevant datafeeds to display your data
- Edit the layout, to resize and position the blocks, then save your layout
- Adafruit IO allows you to publish read only versions of your dashboards to other websites for monitoring incubation conditions as well as configuring actions to send alert emails when conditions exceed limits

![Image](https://github.com/user-attachments/assets/7b07f74f-5639-4913-8092-9b42df199ed7)

**Issues/Notes**

- Installation has been tested on Mac OS 15.3.1 with Arduino IDE 2.3.4
- The Arduino IDE Serial monitor can be used to monitor and debug operation of the unit if required (ensure "Both NL & CR" and the correct baud rate (115200 default))
- The BME280 may cause an I2C address conflict by default and you will need to change the address, either by setting the dipswitch to ON or installing a surface mount resistor to the address pad as per the board manufacturers instructions
- If you use a board other than the ESP32 reverse TFT, you may need to change the configuration hotspot trigger button assignment by altering "#define hotspotPin 2  // Pushbutton pin to activate hotspot for WiFi configuration" in initilise.ino 
  
![Display](https://github.com/user-attachments/assets/ea00b058-6d7c-4633-9912-bed9eeb6a5f1)

- Due to the limited size of the included TFT display, some creativity was required to squish everything onto the display.
  - If "room_monitoring is enabled, additional Temperature and Relative Humidity values will be displayed in white after the probe values
  - "*" indicates the system is calibrating the CO2 sensor (rate is set in the code or via the configuration hotspot)
  - "WIFI" in green text indicates connected wifi, Yellow text indicates the wifi is asleep to save power, Red text indicates the WiFi connection has failed.
  - "^" indicates the system is uploading data to the dashboard (rate is set in the code or via the configuration hotspot)
  - the current CO2 reading reported on the TFT is selected by the "switchCO2Sensors" value, below which the lowCO2 sensor is used if active.
  - the battery discharge rate information doesnt currently fit on the display due to WiFi and IO indicators, so has been commented out in the code
- Additional sensors such as the BME280 (Environmental Sensor including Pressure) and SCD41 (low CO2) can be daisy chained in any order with the STC31 (high CO2 sensor) to the ESP32 using addtional I2C cables using the STEMMA/QWIIC connector or via solder joints  
- 3D Printed Case:
  -   The included 3D printed case file allows for a BME280 breakout board to be installed as well as a lithium ion battery inside the case (enable "Room Monitoring" in the configuration hotspot)
  -   However you may also mount the BME280 breakout board outside of the case in a location of your choosing such as the main incubation chamber of the microscope, to monitor ambient incubation conditions, and daisy chain on the SCD41 (low CO2) and STC31 breakout board (high CO2)   with another I2C cable to install inside the imaging chamber near the sample.
  -   In order to assemble the components within the 3d printed case, insert the ESP32 into the front case, gently spread the vertical panels and push down on the ESP32 until it seats under the latch,
  -   Install the cables onto the ESP32, then install the heatshields, then the bme280, route the battery and cabling, then install the battery support by pushing down until it clicks lightly.
  -   Place the battery on top of the support, pass the long sensor cable out through the hole in the back case, then install the case by sliding it over all of the components and pushing down until it clicks.
- Sensor Calibration:
  - The configuration hotspot contains settings for each sensor to offset the Temperature or CO2 values as well as perform a 2 point calbration for the Relative Humidity
  - Once built and configured you should ideally run the sensors for atleast 30mins to warm up and then place them in a known condition such as T, RH or CO2 and measure the offset and correct using the parameters available in the configuration hotspot
  - RH can be calibrated by using two known conditions of high and low RH at a measured temperature and applying these settings for each sensor
 
    
- Library versions tested
  - "SparkFun_STC3x_Arduino_Library.h" 1.0.0
  - "SparkFun_SHTC3.h" 1.1.4
  - "Adafruit_SHT4x.h" 1.0.5
  - "Adafruit_GFX.h" 1.12.0
  - "Adafruit_ST7789.h" 1.11.0
  - "Adafruit IO Arduino Library" 4.3.0
  - "WiFiManager" 2.0.17
  - "Adafruit BME280 Library" 2.2.4
  - "SparkFun_SCD4x_Arduino_Library.h" 1.1.2
  - "Adafruit_MAX1704X.h" 1.0.3

    
**Acknowledgements and License**

The author wishes to acknowledge the contributions from all of the hardware and software library creators that are featured in this code, without whom this tool would not be possible.  
Please see individual license files of each library for terms and conditions specific to that library.

    
MIT License
Copyright (c) \[2025\] \[James Springfield, The University of Queensland\]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
