#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <SD.h>
#include <SPI.h>


//Timers
//'sync_counter' is the variable upon which both devices get synced
//it increments after time 'ISR_timer'(a variable given below). which is 5ms. which means sync_counter will increment after 5ms
//sync_counter will increase until 't_limit' is reached. 't_limit' is micro seconds conversion of 'execute_code_after_time' which is in ms.
//after execute_code_after_time is reached, code in DoInSync will be excuted                    //t_limit = execute_code_after_time/(ISR_timer/1000)

//Example: lets says we want DoInSync to be executed in sync after each 5 seconds. 'execute_code_after_time' will be 5000ms, 't_limit'=1000,
//         sync_counter will increment after each 5ms, until sync_counter=1000. and 
//         if slave and master will have same sync_counter value at a moment then they will be in sync      
unsigned long sync_counter = 0;        
const unsigned long ISR_timer = 5000; //us  =5ms    This is a timer used for syncronization code
unsigned long t_limit = 1000;    //us
const unsigned long execute_code_after_time = 1000; //milliseconds, This is the time after which whole loop (Already written code) will be executed.

const unsigned long sync_interval = 2000;        //send sync commands to slave after this interval
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
const int TR = 19200;                                                                          // rate of transmitted data
const int nodes = analogNum*digitalNum;                            // total number of nodes (digital pins * analog pins)
int readings[nodes];                                               // array with readings nodes
int i = 0;                                                         // initial value 
int j = 0;                                                         // initial value                
int k = 0;                                                         // initial value
int s = -1;                                                        // initial value
int t = 0;    
const byte sdChipSelect = 10;         //this is the CS pin number of SD.



//Other variables
String ISRreport = "";                          
int level = 0;
boolean synced = false;                          //This will tell if programmed is synced. Sensors will only be read when synced =true
unsigned long reading_no = 0;
unsigned long max_readings = 999;                //after getting sync. this is max number of readings master will take, after which it will again syncing
//                                then again start reading and will keep on doing this
String resp = "";
boolean uui = false;


//Libraries
SoftwareSerial BTSerial(7, 8); // RX | TX

void setup()
{

  Serial.begin(19200);
  Serial.println("Configuring...");

  for (i = 0; i < digitalNum; i++)  
  {                              // loop over all the digital pins
    pinMode(digitalPin[i], OUTPUT);                               // set digital pins as OUTPUT
    digitalWrite(digitalPin[i], LOW);                             // set digital pins LOW
  }

  pinMode(sdChipSelect, OUTPUT);
  digitalWrite(sdChipSelect, HIGH);      //sd is selected ad slave device in SPI when this pin is LOW 


  //Initialise the SD Card
  Serial.print(F("Initializing SD card..."));
  delay(100);
  // check if the card is present and can be initialized:
  if (!SD.begin(sdChipSelect)) 
  {
    int yil = 0;
    while(yil<50)
    {
      delay(100); 
      yil++;
    }
    if(yil>=50)
    {
      if (!SD.begin(sdChipSelect))
        Serial.println(F("Error: Something is wrong with SD card.")); 
    }
  }
  else
  {
    Serial.println(F("card ok"));  
  }
  delay(100);

  Serial.println("...");


  t_limit = execute_code_after_time/(ISR_timer/1000);


  //This Timer1 library helps in running the code in timer_ISR() after time t_limit
  //t_limit is the time after which timer_ISR() will run
  //execute_code_after_time  is the time after which DoInSync() will be executed.

  Timer1.initialize(ISR_timer);
  Timer1.attachInterrupt(timer_ISR);

  BTSerial.begin(38400); 
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

  if(level==0)      // not synced. Send sync command to slave and check response
  {

    if(millis() - sync_interval_timer > sync_interval)
    {
      Serial.println("This is Master");
      sendcom("S#" + String(sync_counter) +";");    //Sends sync_counter to slave
      sendcom("%%%%%%%%%%%%%%%%");              //This command tells slave that master is at level 0. character '%' means level-0. '&' means level-1. '^' means slave is synced.
      sync_interval_timer = millis(); 
    }

    while(BTSerial.available())
    {
      char c = BTSerial.read();
      //Serial.write(c);
      resp += c;
    }

    if(resp.length()>4)
    {
      Serial.println("Slave: " + resp);

      if(resp.indexOf('^')!=-1) // receievd '^' will means slave have been synced
      {
        Serial.println("Slave is synced");
        delay(accuracy_tuner_lite);
        level=1;
      }
      resp = "";
    }

  }
  else if(level==1)    //level-1 : After slave is synced, master gets at level 1, at this level it sends start reading sensors command to slave.
  {
    reading_no=0;
    sendcom("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");    //  '&' character tells the slave to start reading sensors now
    sendcom("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    resp = "";
    level=2;
    delay(accuracy_tuner_lite);
  }
  else if(level==2)    //level-2 : starts reading sensors and receives sensors data from slave. and write it all to sd card
  {
    synced=true;
    if(millis() - sync_interval_timer > sync_interval)
    {
      //Serial.println("#" + String(reading_no));
      sync_interval_timer = millis(); 

    }
    while(BTSerial.available())      //recieve data from slave
    {
      char c = BTSerial.read();
      resp += c;
    }
    if(resp.length()>100)
    {
      if(resp.indexOf('%')!=-1)          //if slave's response include '%' it means slave is reset or is at level-0, so master is also set to level-0/
      {
        Serial.println("Slave Reset");
        synced=false;
        reading_no=0;
        level=0;
      }
      else
      {
        write_to_sd("SLAVE.CSV", resp);    //writes slave's data to sd card
      }
      resp = "";
    }
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
      uui = false;
    }


  }






  if(ISRreport.length()>1)
  {
    write_to_sd("MASTER.CSV", ISRreport);    //writes master's data to sd card
    ISRreport = "";
  }


  /*
   while (Serial.available())
   {
   BTSerial.write(Serial.read());
   }
   */

}











void write_to_sd(char* filename, String data)
{
  File dataFile;
  dataFile = SD.open(filename, FILE_WRITE);  
  if (dataFile) 
  {
    dataFile.print(data);
  }
  else 
  {
    Serial.println(F("Error writing to SD."));
  }
  dataFile.close();
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












