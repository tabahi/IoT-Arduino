#include <Wire.h>
#include <math.h>
#include "DHT.h"
#include <VirtualWire.h>
#include "Adafruit_TCS34725.h"

#define Register_ID 0
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23

#define addressHMC 0x1E //0011110b, I2C 7bit addressHMC of HMC5883

#define DHTTYPE DHT11
#define DHTPIN 2     // what pin we're connected to


#define BMP085_ADDRESS 0x77  // I2C address of BMP085

//Heartbeat sensor
int sensorPin = 0;
double alpha = 0.75;
const int no_of_samples = 10;
int HIGHbpm[no_of_samples];
int LOWbpm[no_of_samples];
int bpmindex = 0;

int highbeat = 0;
int lowbeat = 0;

//Humidity and Temp sensor
float h = 0;
// Read temperature as Celsius
float t = 0;



int red = 0;
int green = 0;
int blue = 0;

/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

DHT dht(DHTPIN, DHTTYPE);


unsigned long timer4 = 0;
unsigned long timer5 = 0;
unsigned long timer6 = 0;
unsigned long timer7 = 0;
unsigned long timer8 = 0;

void setup() 
{
  Serial.begin(9600);
  Wire.begin();

  delay(100);
  dht.begin();
  delay(100);
  tcs.begin();
  delay(100);

  for(int i=0; i<no_of_samples; i++)
  {
    HIGHbpm[i] = 0;
    LOWbpm[i] = 0;
  }
  
  //Tx setup
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(12);
  vw_setup(4000);// speed of data transfer Kbps
  delay(100);
  sendmsg("Hello");

  Serial.println("Start");
  
  //First Readings
  dht_read();
  readcolor();
}




void loop()
{
  heartbeat();
    
  if(millis()-timer5>5000)
  {
    dht_read();
    timer5 = millis();
  }
  if(millis()-timer6>1000)
  {
    serial_print_everything();
    timer6 = millis();
  }  
  if(millis()-timer8>1000)
  {
    readcolor();
    timer8 = millis();
  }
  
  
  
  if(millis()-timer7>500)
  {
    //Transmision
    senddata();
    timer7 = millis();
  }

  delay(10);
}













void serial_print_everything()
{

  //Serial printing every 1 second

 
  //Temp and Pressure
  Serial.print("T:");
  Serial.print(t);
  Serial.print("C\t");
  


  //DHT
  Serial.print("\tH:"); 
  Serial.print((int)h);
  Serial.print("%");
  //Serial.print("Heat index: ");
  //Serial.print(hi);
  //Serial.println(" *F");

 
  
  //RGB
  Serial.print("\tRGB(");
  Serial.print(red);
  Serial.print(",");
  Serial.print(green);
  Serial.print(",");
  Serial.print(blue);
  Serial.print(")");
  
  Serial.println(); 
  
  
}


void readcolor() 
{
  uint16_t r, g, b, c, colorTemp, lux;
  
  tcs.getRawData(&r, &g, &b, &c);

  lux = tcs.calculateLux(r, g, b);
  
  red = (int)(r/4);
  green = (int)(g/4);
  blue = (int)(b/4);
  /*
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  */
}

void heartbeat()
{
  readheartbeat();
  int hb = 0;
  int lb = 1024;
  for(int i=0; i<no_of_samples; i++)
  {
    // Serial.println(String(HIGHbpm[i]) + "\t" + String(LOWbpm[i]));
    if((HIGHbpm[i]>hb)&&(HIGHbpm[i]!=0))
    {
     hb =  HIGHbpm[i];
    }
    if((LOWbpm[i]<lb)&&(LOWbpm[i]!=0))
    {
     lb =  LOWbpm[i];
    }
  }
  if((lb<1024)&&(hb>0))
  {
    highbeat = hb;
    lowbeat = lb;
  }
}


void readheartbeat()
{
  double change = 0.0;
  double oldValue = 0;
  double oldChange = 0;

  int hbpm = 0;
  int lbpm = 1024;
  int m = 0;
  while (m < 10)
  {
    m++;
    int rawValue = analogRead(sensorPin);
    double dvalue = alpha * oldValue + (1 - alpha) * rawValue;
    unsigned int value = (int)(dvalue);
    
    //Serial.println(String(dvalue) +"\t" + String(value));
    
    if (value < lbpm)
    {      
      lbpm = value;
    }
    if (value >= hbpm)
    {
      hbpm = value;
    }
    oldValue = value;
    delay(10);
  }
  if (((lbpm < 1024) && (lbpm >= 0)) && ((hbpm < 1024) && (hbpm >= 0)))
  {
   
    HIGHbpm[bpmindex] = hbpm;
    LOWbpm[bpmindex] = lbpm;
    bpmindex++;

    if (bpmindex >= no_of_samples)
    {
      bpmindex = 0;
    }
  }
}

