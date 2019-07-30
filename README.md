# ESP32-Serial-Bridge

Transparent WiFi (TCP) to all three UART Bridge, supports both AP and STATION WiFi modes. The .ino file is the code for the ESP32. Use Arduino IDE for ESP32 to compile and upload it to the ESP32.
I made this project in order to connect Flight equipment devices devices like (Radio, Vario FLARM), to a Flight Computer (Kobo, Smartphones etc.),  but it is not limited to that. You can use it wherever you want, but on your own risk. Read license file for more details.
Accesspoint                                                    
IPAdress: 192.168.4.1                                           
AP SSID: LK8000                                                   
AP Password: Flightcomputer                                       
Used Ports:                                                                                                          
192.168.4.1:8880  <-> COM0                                     
192.168.4.1:8881  <-> COM1                                     
192.168.4.1:8882  <-> COM2                                     

2019-07-28:
Additional support for XCSoar (funky port numbers) and iGlide (funky connection setup) added.

# For xcsoar port numbers are as follows:
Beware: in case you compile with XCSOAR support, XCSoar's strange port numbers also have to be used with lk8000
COM0 - 4352

COM1 - 4353

COM2 - 10110

IP Address remains the same as without xcsoar support

# iGlide support
For iGlide support (if compiled with it) ESP32-Serial-Bridge also listens on port 2000 and runs
iGlide's funny connection setup procedure. Beware that for iGlide you can use any passwort. It's not evaluated. At this time, iGlide is only connected read-only (e.g. data sent from UART to iGlide).
Beware: IP Address and SSID change for iGlide support. Not sure, whether iGlide really needs this. 

IPAdress: 192.168.1.1                                           
AP SSID: Air ConnectTB                                                   
AP Password: Flightcomputer

# Dynamic configuration
Working on integrating a dynamic configuration via telnet and a simple CLI. That way all devices could be built off of one
sketch and then dynamically configured on the spot. Config will end up in EEPROM.
For dynamic configuration I use DynamicCommandParser (see below). Download it and put it into the root folder of your
sketch in Arduino.

===============================================================

Used Libraries: (must be installed in the arduino IDE):

https://github.com/espressif/arduino-esp32

https://github.com/mdjarv/DynamicCommandParser.git

===============================================================

In some cases the memorylayout is to small for this scetch.
If you face this problem you can either disable Bluetooth by removing
#define BLUETOOTH
in config.h 
or change the partition size as described here:
https://desire.giesecke.tk/index.php/2018/04/20/change-partition-size-arduino-ide/

===============================================================

example usecases:

https://www.youtube.com/watch?v=K2Hia06IMtk

https://www.youtube.com/watch?v=GoSxlQvuAhg

# Hardware
here is the wiring diagram recomendation:
https://raw.githubusercontent.com/AlphaLima/ESP32-Serial-Bridge/master/ESP32-SerialBridge.jpg             
Pinning                                                                                     
COM0 Rx <-> GPIO21                                                                               
COM0 Tx <-> GPIO01                                                                                 
COM1 Rx <-> GPIO16                                                                               
COM1 Tx <-> GPIO17                                                                              
COM2 Rx <-> GPIO15                                                                               
COM2 Tx <-> GPIO04                                                                              

NOTE: The PIN assignment has changed and may not look straigt forward (other PINs are marke as Rx/Tx), but this assignment allows to flash via USB also with hooked MAX3232 serial drivers.

I recomend to start your project with a Node32s or compatible evaluation board. For a TTL to RS232 level conversion search google for "TTL RS3232 Converter"



https://tech.scargill.net/wp-content/uploads/2017/05/ESP326.jpg

A discussion incl. the similar ESP8266 projekt can be found here:

http://www.postfrontal.com/forum/topic.asp?TOPIC_ID=8467
