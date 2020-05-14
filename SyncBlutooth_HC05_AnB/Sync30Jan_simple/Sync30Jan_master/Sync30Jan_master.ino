#include <SoftwareSerial.h>
#include <TimerOne.h>


//Timers
//'sync_counter' is the variable upon which both devices get synced
//it increments after time 'ISR_timer'(a variable given below). which is 5ms. which means sync_counter will increment after 5ms
//sync_counter will increase until 't_limit' is reached. 't_limit' is micro seconds conversion of 'execute_code_after_time' which is in ms.
//after execute_code_after_time is reached, code in DoInSync will be excuted                    //t_limit = execute_code_after_time/(ISR_timer/1000)

//Example: lets says we want DoInSync to be executed in sync after each 5 seconds. 'execute_code_after_time' will be 5000ms, 't_limit'=1000,
//         sync_counter will increment after each 5ms, until sync_counter=1000. and 
//         if slave and master will have same sync_counter value at a moment then they will be in sync      
unsigned long sync_counter = 1;        
const unsigned long ISR_timer = 1000; //us  =5ms    This is a timer used for syncronization code
unsigned long t_limit = 100;    //us
//const unsigned long execute_code_after_time = 1000; //milliseconds, This is the time after which whole loop (Already written code) will be executed.
const unsigned long sync_interval = 1000;        //send sync commands to slave after this interval
unsigned long sync_interval_timer = 0;
const unsigned long accuracy_tuner_lite = 1;    //ms. increasing this will cause lag in master's time, and increase in slave's time.
//increase this if master is lagging


//Nodes
const int analogPin[]={
  A9, A8, A7, A6, A5};                                                    // the analogRead pins
const int digitalPin[]={
  1,2,3,4,5,6,7,8,9,15,16,17,18,19,20};                         // the digital pins
const int analogNum = 5;                                                                       // the number of analog pins
const int digitalNum = 15;
const int TR = 115200;                                                                          // rate of transmitted data
const int nodes = analogNum*digitalNum;                            // total number of nodes (digital pins * analog pins)
int readings[nodes];                                               // array with readings nodes
int i = 0;                                                         // initial value 
int j = 0;                                                         // initial value                
int k = 0;                                                         // initial value
int s = -1;                                                        // initial value
int t = 0;    
const byte sdChipSelect = 10;         //this is the CS pin number of SD.

String bdata = "";

//Other variables
String ISRreport = "";                          
int level = 0;
boolean synced = false;                          //This will tell if programmed is synced. Sensors will only be read when synced =true
unsigned long reading_no = 0;
unsigned long max_readings = 999;                //after getting sync. this is max number of readings master will take, after which it will again syncing
//                                then again start reading and will keep on doing this
String resp = "";
boolean uui = false;

unsigned long lastsend = 0;

//Libraries
SoftwareSerial BTSerial(7, 8); // RX | TX
SoftwareSerial BTPC(9, 10); // RX | TX

void setup()
{

  Serial.begin(115200);
  Serial.println("Configuring...");

  for (i = 0; i < digitalNum; i++)  
  {                              // loop over all the digital pins
    pinMode(digitalPin[i], OUTPUT);                               // set digital pins as OUTPUT
    digitalWrite(digitalPin[i], LOW);                             // set digital pins LOW
  }




  Serial.println("...");


  //t_limit = execute_code_after_time/(ISR_timer/1000);


  //This Timer1 library helps in running the code in timer_ISR() after time t_limit
  //t_limit is the time after which timer_ISR() will run
  //execute_code_after_time  is the time after which DoInSync() will be executed.

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timer_ISR);

  BTSerial.begin(115200); 
  BTPC.begin(115200);
  sendcom("AT");                                     //AT checks if in AT mode
  sendcom("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");      //This command tells slave that master has been restarted
  Serial.println("...");
  level=0;
}



void loop()
{
  //loop code is divided in 3 level 0,1,2. In each loop only one of the levels will run.

  //at start level is 0
  //At this level:
  //-Devices are not synced
  //-Master sends the slave sync_counter variable.   sync_counter is the number upon which both devices gets sync
  //-Master listens to the slave response to see if slave has been synced.
  
  
    int pl = 0;
    while(pl<5)
    {
      while(BTSerial.available())      //recieve data from slave
      {
        char c = BTSerial.read();
        if(c=='%')          //if slave's response include '%' it means slave is reset or is at level-0, so master is also set to level-0/
        {
          Serial.println("Slave Reset");
          synced=false;
          reading_no=0;
          level=0;
          resp = "";
        }
        else
        {
          if(c!='^')
          {
            resp += c;
            if(c=='b')
            {
               BTPC.print("[B]>" + resp + "<");
               //Serial.println("[B]>" + resp + "<");
               resp = "";
            }
          }
          else
          {
           resp = ""; 
          }
        }
      }
      delay(10);
      pl++;
    }
    //Serial.println("[B]>" + resp + "<");
    if(reading_no>=max_readings)    // will sync again when max_readings is reached
    {
      Serial.println("END");
      reading_no=0;
      synced=false;
      level=0;
    }
    if(uui)
    {
      Serial.println(reading_no);
     // Serial.print("\t");
      uui = false;
    }


  






  if(ISRreport.length()>0)
  {
    if(millis() - lastsend > 1000)
    {
      BTPC.print("\r\n[A]>"+ISRreport + "<");
      //write_to_sd("MASTER.CSV", ISRreport);    //writes master's data to sd card
      //Serial.print(ISRreport);
      ISRreport = "";
      lastsend = millis();
    }
  }


  /*
   while (Serial.available())
   {
   BTSerial.write(Serial.read());
   }
   */

}












void sendcom(String cmd)
{
  Serial.println("Sent:" + cmd);
  BTSerial.println(cmd);
}







void check_and_show_response()
{
  String resp = check_response(1000);
  if(resp.length()>0)
  {
    Serial.println("Recieved: " + resp);
  }
}

String check_response(int msecs)
{
  String resp = "";
  int delayer = 0;
  int dsecs = (msecs/10);
  while(delayer<dsecs)
  {
    while(BTSerial.available())
    {
      char coop = BTSerial.read();
      resp += coop;
    }
    delayer++;
    delay(10);
  }

  return resp;  
}













