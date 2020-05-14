#include <VirtualWire.h>
#include <i2cmaster.h>


#define RED_temperature 60
#define GREEN_temperature 45
#define BLUE_temperature 25



#define RF_Data_pin 12
#define RED_pin 5
#define GREEN_pin 6
#define BLUE_pin 7


float temp = 0;
boolean reset_wait = false;


void setup()
{
  pinMode(RED_pin, OUTPUT);
  pinMode(GREEN_pin, OUTPUT);
  pinMode(BLUE_pin, OUTPUT);
  digitalWrite(RED_pin, HIGH);
  digitalWrite(GREEN_pin, HIGH);
  digitalWrite(BLUE_pin, HIGH);
  Serial.begin(9600);

  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);//enable pullups

  //Tx setup
  vw_set_ptt_inverted(true); //
  vw_set_tx_pin(RF_Data_pin);
  vw_setup(4000);// speed of data transfer Kbps
  delay(500);


  digitalWrite(RED_pin, LOW);
  digitalWrite(GREEN_pin, LOW);
  digitalWrite(BLUE_pin, LOW);

  for (int jks = 0; jks < 10; jks++)
  {
    sendmsg("@@@");
    delay(100);
  }
  reset_wait = false;
  sendmsg("Hello");
}



void loop()
{
  read_temperature();


  if ((temp > RED_temperature) || (reset_wait == true))
  {
    digitalWrite(RED_pin, HIGH);
    digitalWrite(GREEN_pin, LOW);
    digitalWrite(BLUE_pin, LOW);
    reset_wait = true;
  }
  else if (temp > GREEN_temperature)
  {
    digitalWrite(RED_pin, LOW);
    digitalWrite(GREEN_pin, HIGH);
    digitalWrite(BLUE_pin, LOW);
  }
  else if (temp >= BLUE_temperature)
  {
    digitalWrite(RED_pin, LOW);
    digitalWrite(GREEN_pin, LOW);
    digitalWrite(BLUE_pin, HIGH);
  }
  else
  {
    digitalWrite(RED_pin, LOW);
    digitalWrite(GREEN_pin, LOW);
    digitalWrite(BLUE_pin, LOW);
  }

  senddata();

  delay(500);
  //delay(1000);
}



//433Mhz Transmission part


void senddata()
{
  String txdata = "";
  txdata += "#";
  txdata += String(temp);
  txdata += "$";
  sendmsg(txdata);
}


void sendmsg(String smsg)
{
  unsigned int len = smsg.length() + 1;
  char msg[len];
  smsg.toCharArray(msg, len);
  Serial.print("Sending: ");
  Serial.print(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  Serial.print("..");
  vw_wait_tx(); // Wait until the whole message is gone
  Serial.println(".!");
}



void read_temperature()
{
  int dev = 0x5A << 1;
  int data_low = 0;
  int data_high = 0;
  int pec = 0;

  i2c_start_wait(dev + I2C_WRITE);
  i2c_write(0x07);

  // read
  i2c_rep_start(dev + I2C_READ);
  data_low = i2c_readAck(); //Read 1 byte and then send ack
  data_high = i2c_readAck(); //Read 1 byte and then send ack
  pec = i2c_readNak();
  i2c_stop();

  //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
  double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
  double tempData = 0x0000; // zero out the data
  int frac; // data past the decimal point

  // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
  tempData = (double)(((data_high & 0x007F) << 8) + data_low);
  tempData = (tempData * tempFactor) - 0.01;

  float celcius = tempData - 273.15;
  float fahrenheit = (celcius * 1.8) + 32;

  temp = celcius;
  Serial.print("Deg C: ");
  Serial.print(celcius);

  Serial.print("\tDeg F: ");
  Serial.println(fahrenheit);
}

