// ESP32 WiFi <-> 3x UART Bridge
// by AlphaLima
// www.LK8000.com
// additional Code for XCSOAR and iGlide support by Torsten Beyer

// Disclaimer: Don't use  for life support systems
// or any other situations where system failure may affect
// user or environmental safety.

#include "config.h"
#include <esp_wifi.h>
#include <WiFi.h>




#ifdef BLUETOOTH
#include <BluetoothSerial.h>
BluetoothSerial SerialBT; 
#endif

#ifdef OTA_HANDLER  
#include <ArduinoOTA.h> 
#endif // OTA_HANDLER

HardwareSerial Serial_one(1);
HardwareSerial Serial_two(2);
HardwareSerial* COM[NUM_COM] = {&Serial, &Serial_one , &Serial_two};

#define MAX_NMEA_CLIENTS 4
#ifdef PROTOCOL_TCP
#include <WiFiClient.h>
WiFiServer server_0(SERIAL0_TCP_PORT);
WiFiServer server_1(SERIAL1_TCP_PORT);
WiFiServer server_2(SERIAL2_TCP_PORT);
WiFiServer *server[NUM_COM]={&server_0,&server_1,&server_2};
WiFiClient TCPClient[NUM_COM][MAX_NMEA_CLIENTS];
// Add special WiFiServer to handle iGlide connections - it will also receive data from COM[0]
#endif
#ifdef IGLIDE
WiFiServer server_iGlide(IGLIDE_PORT);
WiFiClient iGLIDEClient[MAX_IGLIDE_CLIENTS];
#endif


uint8_t buf1[NUM_COM][bufferSize];
uint16_t i1[NUM_COM]={0,0,0};

uint8_t buf2[NUM_COM][bufferSize];
uint16_t i2[NUM_COM]={0,0,0};

uint8_t BTbuf[bufferSize];
uint16_t iBT =0;


void setup() {

  delay(500);
  
  COM[0]->begin(UART_BAUD0, SERIAL_PARAM0, SERIAL0_RXPIN, SERIAL0_TXPIN);
  COM[1]->begin(UART_BAUD1, SERIAL_PARAM1, SERIAL1_RXPIN, SERIAL1_TXPIN);
  COM[2]->begin(UART_BAUD2, SERIAL_PARAM2, SERIAL2_RXPIN, SERIAL2_TXPIN);
  
  if(debug) COM[DEBUG_COM]->println("\n\nLK8000 WiFi serial bridge V1.00");
#ifdef MODE_AP 
   if(debug) COM[DEBUG_COM]->println("Open ESP Access Point mode");
  //AP mode (phone connects directly to ESP) (no router)
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, netmask); // configure ip address for softAP 
  WiFi.softAP(ssid, pw); // configure ssid and password for softAP
#endif


#ifdef MODE_STA
   if(debug) COM[DEBUG_COM]->println("Open ESP Station mode");
  // STATION mode (ESP connects to router and gets an IP)
  // Assuming phone is also connected to that router
  // from RoboRemo you must connect to the IP of the ESP
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pw);
  if(debug) COM[DEBUG_COM]->print("try to Connect to Wireless network: ");
  if(debug) COM[DEBUG_COM]->println(ssid);
  while (WiFi.status() != WL_CONNECTED) {   
    delay(500);
    if(debug) COM[DEBUG_COM]->print(".");
  }
  if(debug) COM[DEBUG_COM]->println("\nWiFi connected");
  
#endif
#ifdef BLUETOOTH
  if(debug) COM[DEBUG_COM]->println("Open Bluetooth Server");  
  SerialBT.begin(ssid); //Bluetooth device name
#endif
#ifdef OTA_HANDLER  
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request

  ArduinoOTA.begin();
#endif // OTA_HANDLER    

  #ifdef PROTOCOL_TCP
  COM[0]->println("Starting TCP Server 1");  
  if(debug) COM[DEBUG_COM]->println("Starting TCP Server 1");  
  server[0]->begin(); // start TCP server 
  server[0]->setNoDelay(true);
  COM[1]->println("Starting TCP Server 2");
  if(debug) COM[DEBUG_COM]->println("Starting TCP Server 2");  
  server[1]->begin(); // start TCP server 
  server[1]->setNoDelay(true);
  COM[2]->println("Starting TCP Server 3");
  if(debug) COM[DEBUG_COM]->println("Starting TCP Server 3");  
  server[2]->begin(); // start TCP server   
  server[2]->setNoDelay(true);
  #endif
  
  // start add'l server if iGlide is supported
  #ifdef IGLIDE
  COM[0]->println("Starting iGlide Server");  
  if(debug) COM[DEBUG_COM]->println("Starting iGlide Server");  
  server_iGlide.begin(); // start iGlide TCP Server
  server_iGlide.setNoDelay(true);
  #endif

  esp_err_t esp_wifi_set_max_tx_power(50);  //lower WiFi Power
}

#ifdef IGLIDE
// handle startup for iGlide connection 
bool start_iGlide_connection (int clientno) {
  uint8_t buf[bufferSize];
  int i;
               
  if(iGLIDEClient[clientno]) 
  {
    // check if iGlide sends "Hello"
    i = 0;
    while(iGLIDEClient[clientno].available())
    {
      buf[i] = iGLIDEClient[clientno].read(); // read char from client (iGlide)
      if(i<bufferSize-1) i++;
    } 
    // we don't care what iGlide sends and forget buf content hence i=0
    i= 0;
    
    // send PASS?*
    iGLIDEClient[clientno].write (IGLIDE_PW_PHRASE);
    
    // read password from iGlide
    while(iGLIDEClient[clientno].available())
    {
      buf[i] = iGLIDEClient[clientno].read(); // read char from client (iGlide)
      if(i<bufferSize-1) i++;
    }

    // we don't care WHAT iGlide sends as long as it sends SOMETHING - meaning, it's alive and loves us
    if (i>0) {
      // send AOK
      iGLIDEClient[clientno].write (IGLIDE_OK_PHRASE);
      return true;
    }
    else {
      // Hmm, iGlide fell asleep - so no connection in place, return false
      return false;
    }
  }
  return false;
}
#endif

void loop() 
{  
#ifdef OTA_HANDLER  
  ArduinoOTA.handle();
#endif // OTA_HANDLER
  
#ifdef BLUETOOTH
  // receive from Bluetooth:
  if(SerialBT.hasClient()) 
  {
    while(SerialBT.available())
    {
      BTbuf[iBT] = SerialBT.read(); // read char from client (LK8000 app)
      if(iBT <bufferSize-1) iBT++;
    }          
    for(int num= 0; num < NUM_COM ; num++)
      COM[num]->write(BTbuf,iBT); // now send to UART(num):          
    iBT = 0;
  }  
#endif  
#ifdef PROTOCOL_TCP
  for(int num= 0; num < NUM_COM ; num++)
  {
    if (server[num]->hasClient())
    {
      for(byte i = 0; i < MAX_NMEA_CLIENTS; i++){
        //find free/disconnected spot
        if (!TCPClient[num][i] || !TCPClient[num][i].connected()){
          if(TCPClient[num][i]) TCPClient[num][i].stop();
          TCPClient[num][i] = server[num]->available();
          if(debug) COM[DEBUG_COM]->print("New client for COM"); 
          if(debug) COM[DEBUG_COM]->print(num); 
          if(debug) COM[DEBUG_COM]->println(i);
          continue;
        }
      }
      //no free/disconnected spot so reject
      WiFiClient TmpserverClient = server[num]->available();
      TmpserverClient.stop();
    }
  }

  // check if iGlide connection is coming in
#ifdef IGLIDE
  if (server_iGlide.hasClient()) {
    bool connected = false;
    bool startup_iglide = false;
    
    for(byte i = 0; i < MAX_IGLIDE_CLIENTS; i++){
        //find free/disconnected spot
        if (!iGLIDEClient[i] || !iGLIDEClient[i].connected()){
          if(iGLIDEClient[i]) iGLIDEClient[i].stop();
          iGLIDEClient[i] = server_iGlide.available();
          if(debug) COM[DEBUG_COM]->print("New client for iGlide"); 
          if(debug) COM[DEBUG_COM]->print("0"); 
          if(debug) COM[DEBUG_COM]->println(i);

          connected = true;
          
          // HANDLE IGLIDE CONNECTION PROTOCOL
          startup_iglide = start_iGlide_connection(i);
          if (!startup_iglide) {
            iGLIDEClient[i].stop();
          }
        }
    }
    if (!connected) {
      WiFiClient TmpserverClient = server_iGlide.available();
      TmpserverClient.stop();
    }
  }
#endif // IGLIDE
#endif // PROTOCOL_TCP
 
  for(int num= 0; num < NUM_COM ; num++)
  {
    if(COM[num] != NULL)          
    {
      for(byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++)
      {               
        if(TCPClient[num][cln]) 
        {
          while(TCPClient[num][cln].available())
          {
            buf1[num][i1[num]] = TCPClient[num][cln].read(); // read char from client (LK8000 app)
            if(i1[num]<bufferSize-1) i1[num]++;
          } 

          COM[num]->write(buf1[num], i1[num]); // now send to UART(num):
          i1[num] = 0;

          // add code for iGlide
        }
      }
  
      if(COM[num]->available())
      {
        while(COM[num]->available())
        {     
          buf2[num][i2[num]] = COM[num]->read(); // read char from UART(num)
          if(i2[num]<bufferSize-1) i2[num]++;
        }
        // now send to WiFi:
        for(byte cln = 0; cln < MAX_NMEA_CLIENTS; cln++)
        {   
          if(TCPClient[num][cln])                     
            TCPClient[num][cln].write(buf2[num], i2[num]);
        }
        // if iGlide supported AND num == IGLIDE_UART send data to iGlide devices, too
#ifdef IGLIDE
        if (num == IGLIDE_UART)
        {
          for(byte cln = 0; cln < MAX_IGLIDE_CLIENTS; cln++)
          {   
            if(iGLIDEClient[cln])                     
              iGLIDEClient[cln].write(buf2[num], i2[num]);
          }
        }
#endif
#ifdef BLUETOOTH        
        // now send to Bluetooth:
        if(SerialBT.hasClient())      
          SerialBT.write(buf2[num], i2[num]);               
#endif  
        i2[num] = 0;
      }
    }    
  }
}
