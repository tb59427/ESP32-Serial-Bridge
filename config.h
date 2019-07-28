// config: ////////////////////////////////////////////////////////////
// 
//#define BLUETOOTH
//if used by XCSOAR clients, uncomment the next line
#define XCSOAR 

#define OTA_HANDLER 
#define MODE_AP // phone connects directly to ESP


#define PROTOCOL_TCP

//if you want iGlide support uncomment the following line
#define IGLIDE

bool debug = true;

#define VERSION "1.12"

// For AP mode:
#ifdef IGLIDE
// this may be obsolete - these settings are derived from iGlide manual 
const char *ssid = "Air ConnectTB";  // Per iGlide manual, this should be the SSID - not sure wether iGlide really needs this. Needs some testing
const char *pw = "Flightcomputer";   // and this is the password
IPAddress ip(192, 168, 1, 1);        // Per iGlide manual, iGlide seems to expect this IP as the Air Connect IP
#else
const char *ssid = "D-2402";  // You will connect your phone to this Access Point
const char *pw = "Flightcomputer"; // and this is the password
IPAddress ip(192, 168, 4, 1); // From RoboRemo app, connect to this IP
#endif
IPAddress netmask(255, 255, 255, 0);


// You must connect the phone to this AP, then:
// menu -> connect -> Internet(TCP) -> 192.168.4.1:8880  for UART0
//                                  -> 192.168.4.1:8881  for UART1
//                                  -> 192.168.4.1:8882  for UART2
// Port numbers for XCSOAR are a little awkward with 4352,4353 and 10110 respectively
// iGlide always expects data at port 2000


#define NUM_COM   3                 // total number of COM Ports
#define DEBUG_COM 0                 // debug output to COM0
/*************************  COM Port 0 *******************************/
#define UART_BAUD0 19200            // Baudrate UART0
#define SERIAL_PARAM0 SERIAL_8N1    // Data/Parity/Stop UART0
#define SERIAL0_RXPIN 21            // receive Pin UART0
#define SERIAL0_TXPIN 1             // transmit Pin UART0
#ifdef XCSOAR
#define SERIAL0_TCP_PORT 4352       // Wifi Port UART0 for XCSoar
#else
#define SERIAL0_TCP_PORT 8880       // Wifi Port UART0 for LK8000
#endif


/*************************  COM Port 1 *******************************/
#define UART_BAUD1 19200            // Baudrate UART1
#define SERIAL_PARAM1 SERIAL_8N1    // Data/Parity/Stop UART1
#define SERIAL1_RXPIN 16            // receive Pin UART1
#define SERIAL1_TXPIN 17            // transmit Pin UART1
#ifdef XCSOAR
#define SERIAL1_TCP_PORT 4353       // Wifi Port UART1 for XCSoar
#else
#define SERIAL1_TCP_PORT 8881       // Wifi Port UART1 for LK8000
#endif

/*************************  COM Port 2 *******************************/
#define UART_BAUD2 9600            // Baudrate UART2
#define SERIAL_PARAM2 SERIAL_8N1    // Data/Parity/Stop UART2
#define SERIAL2_RXPIN 15            // receive Pin UART2
#define SERIAL2_TXPIN 4             // transmit Pin UART2
#ifdef XCSOAR
#define SERIAL2_TCP_PORT 10110      // Wifi Port UART2 for XCSoar
#else
#define SERIAL2_TCP_PORT 8882       // Wifi Port UART2 for LK8000
#endif


/*************************  iGlide settings **************************/
#ifdef IGLIDE                       // Support for iGlide Connections
#define IGLIDE_PORT 2000            // iGlide connects at port 2000
#define IGLIDE_UART 0               // iGlide connections connect to GPIO port 0
#define MAX_IGLIDE_CLIENTS 2        // 2 Connections max - front seat + back seat
#define IGLIDE_PW_PHRASE "PASS?"   // this string is sent by Air Connect once connected
#define IGLIDE_OK_PHRASE "AOK"      // this string is sent by Air Connect once PW was sent by iGlide
#endif

#define bufferSize 1024


//////////////////////////////////////////////////////////////////////////
