#include <SoftwareSerial.h>
#include <TimerOne.h>



//Timers
unsigned long sync_counter = 0;
const unsigned long ISR_timer = 5000; //us  5ms    This is a timer used for syncronization code
unsigned long t_limit = 1000;
const unsigned long sync_interval = 2000;        //send sync commands to slave after this interval
unsigned long sync_interval_timer = 0;
const unsigned long execute_code_after_time = 500; //milliseconds, This is the time after which whole loop (Already written code) will be executed.
const unsigned long accuracy_tuner = 50;               //In case if there is always a 200ms out of sync delay due to communication delay, then we can tune it here.



//Nodes
const int analogPin[]={
  A9, A8, A7, A6, A5};                                                    // the analogRead pins
const int digitalPin[]={
  1,2,3,4,5,6,7,8,9,10,11,12,13,15,16};                         // the digital pins
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
const unsigned long max_readings = 99999;
String resp = "";

//Libraries
SoftwareSerial BTSerial(7, 8); // RX | TX







void setup()
{
  Serial.begin(19200);

  for (i = 0; i < digitalNum; i++)  
  {                              // loop over all the digital pins
    pinMode(digitalPin[i], OUTPUT);                               // set digital pins as OUTPUT
    digitalWrite(digitalPin[i], LOW);                             // set digital pins LOW
  }

  Serial.println("Configuring...");

  Serial.println("...");

  t_limit = ((ISR_timer/5)*execute_code_after_time)/1000;

  Timer1.initialize(execute_code_after_time);
  Timer1.attachInterrupt(timer_ISR);

  BTSerial.begin(38400);  // HC-05 default speed in AT command more
  sendcom("AT");
  sendcom("RESTART");
  delay(100);
  sendcom("%%%%%%%%%%%%%%%%%%%");

  Serial.println("...");
}



void loop()
{

  if(level==0)
  {
    if(millis() - sync_interval_timer > sync_interval)
    {
      sendcom("%%%");
      sync_interval_timer = millis(); 
    }
    while(BTSerial.available())
    {
      char c = BTSerial.read();
      //Serial.write(c);
      resp += c;
    }
    if(resp.length()>5)
    {
      Serial.println("0-Master: " + resp);
      if((resp.indexOf(';')>resp.indexOf('#')) && (resp.indexOf('#')>=0) )  //recieved sync_counter
      {
        parse_sync(resp);
        level=1;
        Serial.println("Synced!")
      }
      resp = "";
    }
    
  }
  else if(level==1)
  {
    sendcom("^");
    while(BTSerial.available())
    {
      char c = BTSerial.read();
      resp += c;
    }
    if(resp.length()>4)
    {
      Serial.println("1-Master: " + resp);
      if(resp.indexOf('&')!=-1)  //"StartStartStartStart"
      {
        Serial.println("Start!")
        synced=true;
        level=2;
      }
      resp = "";
    }
  }
  else if(level==2)          //readings
  {
    synced=true;
    if(millis() - sync_interval_timer > sync_interval)
    {
      Serial.println("2-#" + String(reading_no));
      sync_interval_timer = millis(); 
      if(reading_no>=max_readings)
      {
        reading_no=0;
        synced=false;
        level=0;
      }
      while(BTSerial.available())      //recieve data from slave
      {
        char c = BTSerial.read();
        resp += c;
      }
      Serial.println("2-Master: " + resp);
      if(resp.indexOf('%')!=-1)
      {
        synced=false;
        level=0;
      }
       resp = "";
    }
    if(ISRreport.length()>10)
    {
      Serial.println("2-ISR: "  +  ISRreport);   
      BTSerial.print(ISRreport);            //send reading data to master
      ISRreport = "";
    }
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



