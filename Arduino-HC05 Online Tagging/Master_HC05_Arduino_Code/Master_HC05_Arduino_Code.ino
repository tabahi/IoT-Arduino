/*
Arduino used HC05 to search for nearby bluetooth and uploads their Mac adresses to a PHP server using Ethernet shield.

*/

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>



unsigned long range = 1000; //increasing this number will increase the range significantly
unsigned long time_to_remember = 120000;
const int bluetooths_to_buffer = 12;   //  more ram will be utilized for more events to buffer which may cause ram failure.  If it is less, event may get lost in case of connection failure.

char server[] = "server_adress.com";  //a php based webserver address
char server_directory[] = "/HC05test/post.php";  //address pointer for server code



const unsigned long serverupdateinterval = 20000; //ms
unsigned long updatetime = 0; //ms

struct logs
{
  String ID;
  unsigned long stime;
  unsigned long ltime;
  unsigned long rssi;
  boolean inrange;
  boolean sent1;
};

struct logs eventlog[bluetooths_to_buffer + 1];
int clog = 0;


byte mac[] = {0xAC, 0xAC, 0x88, 0x88, 0x00, 0x21 };


//defining Ethernet client name
EthernetClient client;
SoftwareSerial BTSerial(6, 7); // RX | TX


void setup()
{
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(9, HIGH);
  pinMode(4, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(4, HIGH);

  Serial.begin(9600);
  Serial.println("Starting...");
  
  
  if (Ethernet.begin(mac) == 0) 
  {
    Serial.println("DHCP Failed. Trying again...");
    delay(2000);
    //Retry connecting to network after 2 seconds using manual IP assignning
    if (Ethernet.begin(mac) == 0) 
    {
      Serial.println("DHCP Failed Again. Using static IP");
      
      delay(2000);
      IPAddress ip(192,168,1,111);
      Ethernet.begin(mac, ip);
    }
  }
  Serial.println("Ethernet:ok");
  Serial.println(Ethernet.localIP());
  

  

  for (int k = 0; k < bluetooths_to_buffer; k++)
  {
    eventlog[k].ID = "";
    eventlog[k].stime = 0;
    eventlog[k].ltime = 0;
    eventlog[k].rssi = 0;
    eventlog[k].inrange = false;
    eventlog[k].sent1 = false;
  }

  BTSerial.begin(38400);  // HC-05 default speed in AT command more
  sendcom("AT", 100);
  //sendcom("at+rname?E4D5,3D,E8E322", 5000);
  //sendcom("at+rname?60D9,A0,326E4C", 5000);
  sendcom("AT+ORGL", 5000);
  sendcom("AT+NAME=master", 5000);
  sendcom("AT+RMAAD", 5000);
  sendcom("AT+ROLE=1", 5000);
  sendcom("AT+PSWD=1234", 5000);

  Serial.println("AT mode");
}



void loop()
{

  //Bluetooths that are gone out of range
  for (int k = 0; k < bluetooths_to_buffer; k++)
  {
    unsigned long cmills = millis();
    if (eventlog[k].rssi > 0)
    {
      if ((cmills - eventlog[k].ltime) > time_to_remember)
      {
        unsigned long duration = eventlog[k].ltime - eventlog[k].stime + 1;
        Serial.println("Out of range: " + eventlog[k].ID + "\t Duration: " + String(duration));
        eventlog[k].inrange = false;
      }
    }
  }


  //Check the bluetooths in range
  sendcom("AT+RESET", 10000);
  delay(1000);
  sendcom("AT+CMODE=1", 1000);
  sendcom("AT+CLASS=000000", 10000);
  sendcom("AT+INQM=1,9,5", 5000);
  sendcom("AT+INIT", 20000);
  sendINQ("AT+INQ", 10000);
  Serial.println();
  Serial.println("Scanning Complete");
  
  
  delay(500);  
  
  
  if((millis()-updatetime)>serverupdateinterval)
  {
    updatetime = millis();
    senddata();
  }
  
  delay(500);


}
