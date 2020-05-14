#include <TimerOne.h>
#include <math.h>
#include <stdio.h>
#include <SoftwareSerial.h>


int mode = 0;  //   1=master  0=slave
unsigned long sync_counter = 0;
const unsigned long ISR_timer = 5000; //us  5ms    This is a timer used for syncronization code
unsigned long t_limit = 1000;
String ISRreport = "";

const unsigned long execute_code_after_time = 500; //milliseconds, This is the time after which whole loop (Already written code) will be executed.
//In your code there was delay(500) so I assumed it will be 500ms

unsigned long accuracy_tuner = 200;               //In case if there is always a 200ms out of sync delay due to communication delay, then we can tune it here.
//This happens because of time taken while sending data from bluetooth

boolean synced = false;                          //This will tell if programmed is synced


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
int t = 0;                                                         // initial value

SoftwareSerial BTSerial(7, 8); // RX | TX

void setup() 
{
  Serial.begin(TR);

  pinMode(14, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(14, HIGH);

  BTSerial.begin(38400);  // HC-05 default speed in AT command more


  for (i = 0; i < digitalNum; i++)  
  {                              // loop over all the digital pins
    pinMode(digitalPin[i], OUTPUT);                               // set digital pins as OUTPUT
    digitalWrite(digitalPin[i], LOW);                             // set digital pins LOW
  }


  Serial.println("...");

  t_limit = ((ISR_timer/5)*execute_code_after_time);

  Timer1.initialize(execute_code_after_time);
  Timer1.attachInterrupt(timer_ISR);


}





void loop() 
{
  if(synced==false)
  {
    if(mode==1)
    {
      bt_master();
    }
    else
    {
      bt_slave();
    }

    if(ISRreport.length()>1)
    {
      Serial.println(ISRreport);
      ISRreport = "";
    }
    delay(3000);
  }
  else
  {
    delay(1000);
  }

  while (Serial.available())
  {
    BTSerial.write(Serial.read());
  }
}











void bt_master()
{
  BTSerial.println("#" + String(sync_counter) + "*");

  String recieved = "";

  while(BTSerial.available())
  {
    recieved += BTSerial.read();
  }

  Serial.println("Slave to Master: " + recieved);
}




void bt_slave()
{
  String recieved = "";

  while(BTSerial.available())
  {
    recieved += BTSerial.read();
  }

  Serial.println("Mater to slave: " + recieved);
}


