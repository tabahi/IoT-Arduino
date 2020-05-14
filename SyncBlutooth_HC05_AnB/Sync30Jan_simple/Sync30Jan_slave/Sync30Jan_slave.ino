#include <SoftwareSerial.h>
#include <TimerOne.h>



//Timers
unsigned long sync_counter = 1;
const unsigned long ISR_timer = 1000; //us  5ms    This is a timer used for syncronization code
unsigned long t_limit = 100;
const unsigned long sync_interval = 1000;        //send sync commands to slave after this interval
unsigned long sync_interval_timer = 0;
//const unsigned long execute_code_after_time = 1000; //milliseconds, This is the time after which whole loop (Already written code) will be executed.
const unsigned long accuracy_tuner = 1;               //ms. Increasing this will cause increase in slave's time.

unsigned long reading_sent = 0;

//Nodes
const int analogPin[]={A9, A8, A7, A6, A5};                                                    // the analogRead pins
const int digitalPin[]={1,2,3,4,5,6,7,8,9,10,11,12,14,15,16};                                  // the digital pins
const int analogNum = 5;                                                                       // the number of analog pins
const int digitalNum = 15;
const int TR = 19200;                                                                          // rate of transmitted data
const int nodes = analogNum*digitalNum;                            // total number of nodes (digital pins * analog pins)
int readings[nodes];                                               // array with readings nodes
int i = 0;                                                         // initial value 
int j = 0;                                                         // initial value                
int k = 0;                                                         // initial value
int s = -1;                                                        // initial value
int t = 0;    


//Other variables
String ISRreport = "";
int level = 0;
boolean synced = false;                          //This will tell if programmed is synced
unsigned long reading_no = 0;
const unsigned long max_readings = 999;
String resp = "";
volatile boolean uui = false;


//Libraries
SoftwareSerial BTSerial(7, 8); // RX | TX







void setup()
{
  Serial.begin(115200);

  for (i = 0; i < digitalNum; i++)  
  {                              // loop over all the digital pins
    pinMode(digitalPin[i], OUTPUT);                               // set digital pins as OUTPUT
    digitalWrite(digitalPin[i], LOW);                             // set digital pins LOW
  }

  Serial.println("Configuring...");

  Serial.println("...");

 //t_limit = execute_code_after_time/(ISR_timer/1000);

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer_ISR);

  BTSerial.begin(115200);  // HC-05 default speed in AT command more
  
  sendcom("AT");
  delay(100);
  sendcom("%%%%%%%%%%%%%%%%%%%");  
  

  Serial.println("...");
  level=0;
}



void loop()
{
  
  


    synced=true;
    if(reading_no>reading_sent)
    {
      if(ISRreport.length()>0)
      {
        String hold = ISRreport;
        ISRreport = ""; 
        Serial.println(hold);
        BTSerial.print(hold);      //send sensor reading to master
        reading_sent++;
      }
    }
    if(reading_no>=max_readings)
    {
      Serial.println("END");
      reading_no=0;
      reading_sent=0;
      synced=false;
      level=0;
    }
    if(uui)
    {
      Serial.println(reading_no);
      uui = false;
    }

  

  
  /*
  while (Serial.available())
   {
   BTSerial.write(Serial.read());
   }
   */
}





void parse_sync(String oop)
{
  unsigned long gc = oop.substring(oop.indexOf('#') + 1, oop.indexOf(';')).toInt();
  Serial.println(gc);
  sync_counter = gc + accuracy_tuner;
}



void sendcom(String cmd)
{
  Serial.println("Sent:" + cmd);
  BTSerial.println(cmd);
}






