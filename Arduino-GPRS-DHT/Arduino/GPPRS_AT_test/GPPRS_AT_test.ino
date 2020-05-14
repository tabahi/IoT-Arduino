#include <SoftwareSerial.h>
SoftwareSerial GPRSS(2, 3); // Rx, Tx

boolean gprsok = false;


unsigned long delayer = 0;

void setup()
{
  pinMode(8, OUTPUT);
  Serial.begin(9600);
  GPRSS.begin(4800);
  Serial.println("Initializing GPRS...");
}

void loop()
{

  while (GPRSS.available())
  {
    char c = GPRSS.read();
    Serial.write(c);
  }

  while (Serial.available())
  {
    char c = Serial.read();
    GPRSS.write(c);
  }

  if ((millis() - delayer) > 5000)
  {
    Serial.println("\r\n...........................................");
    powerSwitch();
    
    
    
    if (check_power_on(10000))
    {
      Serial.println("Power is ON");
    }
    else
    {
      Serial.println("Power is OFF");
    }
    delayer = millis();
  }

}


void powerSwitch()
{
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  delay(1200);
  digitalWrite(8, LOW);
  delay(2000);
  digitalWrite(8, HIGH);
  delay(3500);
  sendcom("AT");
  delay(1000);
}



void sendcom(String com)
{
  Serial.println("Command: " + com);
  GPRSS.println(com);
  delay(10);
}


String check_response(unsigned long wait, boolean ok)
{
  String s = "";
  unsigned long waiter = 0;
  gprsok = false;
  while (waiter <= wait)
  {
    while (GPRSS.available())
    {
      char c = GPRSS.read();
      Serial.write(c);
      s += c;
    }
    waiter = waiter + 10;
    if (ok == true)
    {
      if (s.indexOf("OK") != -1)
      {
        break;
      }
    }
    delay(10);
  }
  if (s.length() > 0)
  {
    //Serial.println("GPRS: " + s);
  }
  if (s.indexOf("OK") != -1)
  {
    gprsok = true;
  }
  return s;
}

boolean check_power_on(unsigned long wait)
{
  String s = "";
  unsigned long waiter = 0;
  boolean poweron = false;
  check_response(1000, false);
  sendcom("AT+CIPSTATUS");
  while (waiter <= wait)
  {
    while (GPRSS.available())
    {
      char c = GPRSS.read();
      Serial.write(c);
      s += c;
    }
    waiter = waiter + 10;
    if ((s.indexOf("OK") != -1) && (s.indexOf("INITIAL") != -1))
    {
      poweron = true;
    }
    if ((s.indexOf("NORMAL") != -1) || (s.indexOf("DOWN") != -1))
    {
      poweron = false;
    }

    delay(10);
  }
  return poweron;
}
