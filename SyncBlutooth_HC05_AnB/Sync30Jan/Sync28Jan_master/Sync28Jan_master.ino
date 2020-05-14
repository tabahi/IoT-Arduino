#include <SoftwareSerial.h>
#include <TimerOne.h>
#include <SD.h>
#include <SPI.h>

//Timers
unsigned long sync_counter = 0;
const unsigned long ISR_timer = 5000; //us  5ms    This is a timer used for syncronization code
unsigned long t_limit = 1000;
const unsigned long sync_interval = 2000;        //send sync commands to slave after this interval
unsigned long sync_interval_timer = 0;
const unsigned long execute_code_after_time = 500; //milliseconds, This is the time after which whole loop (Already written code) will be executed.
unsigned long accuracy_tuner = 50;               //In case if there is always a 200ms out of sync delay due to communication delay, then we can tune it here.
const unsigned long accuracy_tuner_lite = 50;


//Nodes
const int analogPin[]={ 
  A9, A8, A7, A6, A5};                                                    // the analogRead pins
const int digitalPin[]={ 
  1,2,3,4,5,6,7,8,9,15,16};                         // the digital pins
const int analogNum = 5;                                                                       // the number of analog pins
const int digitalNum = 11;
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
boolean synced = false;                          //This will tell if programmed is synced
unsigned long reading_no = 0;
unsigned long max_readings = 99999;
String resp = "";

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
  digitalWrite(sdChipSelect, HIGH);


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


  t_limit = ((ISR_timer/5)*execute_code_after_time)/1000;

  Timer1.initialize(execute_code_after_time);
  Timer1.attachInterrupt(timer_ISR);

  BTSerial.begin(38400);  // HC-05 default speed in AT command more
  sendcom("AT");
  sendcom("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
  Serial.println("...");
}



void loop()
{

  if(level==0)      // not synced. Send sync command to slave and wait for response
  {

    if(millis() - sync_interval_timer > sync_interval)
    {
      sendcom("S#" + String(sync_counter) +";");
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
      Serial.println("-Slave: " + resp);

      if(resp.indexOf('^')!=-1) // receievd SYNCED will means slave have been synced
      {
        Serial.println("0-Slave is synced");
        delay(accuracy_tuner_lite);
        level=1;
      }
      resp = "";
    }

  }
  else if(level==1)    //recieved sync ok then start start reading command
  {
    reading_no=0;
    sendcom("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    sendcom("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&");
    resp = "";
    level=2;
    delay(accuracy_tuner_lite);
  }
  else if(level==2)    //synced and start reading
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
    }
    while(BTSerial.available())      //recieve data from slave
    {
      char c = BTSerial.read();
      resp += c;
    }
    if(resp.length()>10)
    {
      Serial.println("2-Slave: " + resp);
      if(resp.indexOf('%')!=-1)
      {
        synced=false;
        level=0;
      }
      else
      {
        //write slaves data to SD card
        write_to_sd("SLAVE.CSV", resp);
      }
      resp = "";
    }


  }






  if(ISRreport.length()>1)
  {
    Serial.println("ISR: "  +  ISRreport);
    write_to_sd("MASTER.CSV", ISRreport);
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








