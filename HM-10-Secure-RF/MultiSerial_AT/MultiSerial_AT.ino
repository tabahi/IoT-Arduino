#include <SoftwareSerial.h>


#define remote_ADDR "60D9A0326E4C"
#define pass_code "004483"  //6 characters only
#define ROLE 1 //0=Peripheral,  1=central
#define CONNECT_TIMEOUT 60000 //ms

//SoftwareSerial Serial1(10, 11); //for UNO, NANO, MICRO


const String errorstring = "ERROR";
boolean connected = false;

void setup()
{
  // initialize both serial ports:
  pinMode(INPUT, 14);
  pinMode(INPUT, 16);
  pinMode(OUTPUT, 13);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(500);
  Serial.println(F("Connecting BLE"));


  while (setup_HM10() == false)
  {
    delay(5000);
  }
  Serial.println("...");

}

String ss = "";
String senc = "";
unsigned long sstimeout = 0;
unsigned long cntimeout = 0;

void loop()
{
  while (Serial1.available())
  {
    char c = Serial1.read();
    ss += c;
    sstimeout = millis();
  }

  int sslen = ss.length();
  if ((((millis() - sstimeout) > 500) && (sslen > 0)) || (sslen>32))
  {
    Serial.println(ss);
    if (ss.indexOf("OK+LOST") != -1)
    {
      connected = false;
      Serial.println(F("Connection Lost"));
      digitalWrite(13, LOW);
      ss = "";
    }
    else if ((ss.indexOf("OK+CONNF") != -1) || (ss.indexOf("OK+CONNE") != -1))
    {
      connected = false;
      Serial.println(F("Couldn't connect"));
      digitalWrite(13, LOW);
      ss = "";
    }
    else if (ss.indexOf("OK+CONN") != -1)
    {
      connected = true;
      Serial.println(F("Connected"));
      digitalWrite(13, HIGH);
      ss = "";
    }
    
    if(connected==true)
    {
      //senc += ss;   //Encypted data recieved
      Serial.println("Encrypted data");
    }
    ss = "";
  }

  if ( (ROLE == 1) && (connected == false) && ((millis() - cntimeout) > 60000) )
  {
    Serial.println(F("Trying Again"));
    setup_HM10();
    cntimeout = millis();
  }




  while (Serial.available())
  {
    char c = Serial.read();
    Serial1.write(c);
  }

}
