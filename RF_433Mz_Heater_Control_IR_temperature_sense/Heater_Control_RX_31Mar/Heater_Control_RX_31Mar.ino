#include <VirtualWire.h>


#define RED_temperature 60

#define RF_Data_pin 12
#define RED_pin 5


//Temperature control hysteresis
const float temperature_upper_limit = 80.0;   //if higher than this value, heater will be switched OFF
const float temperature_lower_limit = 78.0;   //if below this value, heater will be switched ON

const int relay_pin = 8;
float temp = 0.0;
float last_temp = 0.0;
String rec = "";
boolean heater_state = LOW;
unsigned long lastupdate = 0;

boolean reset_wait = false;

void setup()
{

  temp = (temperature_lower_limit + temperature_upper_limit) / 2;
  last_temp = temp;
  reset_wait = false;


  pinMode(RED_pin, OUTPUT);
  pinMode(relay_pin, OUTPUT);
  digitalWrite(RED_pin, LOW);
  digitalWrite(relay_pin, LOW);
  Serial.begin(9600);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(RF_Data_pin);
  vw_setup(4000);  // Bits per sec\

  vw_rx_start();       // Start the receiver PLL running
  Serial.println("Listening...");

}



void loop()
{
  rx_signal();

  if (reset_wait == false)
  {
    if ( (heater_state == LOW) && (temp < temperature_lower_limit))
    {
      heater_state = HIGH;
      digitalWrite(RED_pin, LOW);
      digitalWrite(relay_pin, HIGH);
      Serial.println("Heater ON");
    }
    else if ( (heater_state == HIGH) && (temp >= temperature_upper_limit))
    {
      heater_state = LOW;
      digitalWrite(RED_pin, HIGH);
      digitalWrite(relay_pin, LOW);
      reset_wait = true;
      Serial.println("Heater OFF");
    }
  }

  
  if (last_temp != temp)
  {
    Serial.println("T:\t" + String(temp) + " C");
    last_temp = temp;
  }




  if (millis() - lastupdate > 30000)
  {
    Serial.println("No Signal");
    reset_all();
  }

}



//433Mhz Reciever part

unsigned long rx_signal()
{

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;

  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    for (int i = 0; i < buflen; i++)
    {
      char chr = buf[i];
      rec += chr;
      if (rec.length() > 9)
      {
        rec = rec.substring(2);
      }
    }
    if (rec.length() > 0)
    {
      if (rec.indexOf('@') > -1)
      {
        reset_all();
        reset_wait = false;
      }
      else
      {
        int j5 = rec.indexOf('#');
        if (j5 != -1)
        {
          rec = rec.substring(j5);
          int j6 = rec.indexOf('$');
          if (j6 > 0)
          {
            String valueS = rec.substring(1, j6 - 1); //#8.88484$
            temp = valueS.toFloat();
            rec = "";
          }
        }
      }
      lastupdate = millis();
    }
  }
  return 0;
}


void reset_all()
{
  lastupdate = 0;
  digitalWrite(RED_pin, LOW);
  digitalWrite(relay_pin, LOW);
  temp = (temperature_lower_limit + temperature_upper_limit) / 2;
  last_temp = temp;
  rec = "";
  heater_state = LOW;
  Serial.println("RESET");
}


