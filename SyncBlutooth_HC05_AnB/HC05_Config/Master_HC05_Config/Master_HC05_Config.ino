#include <SoftwareSerial.h>

SoftwareSerial BTSerial(7, 8); // RX | TX

void setup()
{
  pinMode(14, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(14, HIGH);
  Serial.begin(19200);
  Serial.println("Configuring...");
  
  BTSerial.begin(38400);  // HC-05 default speed in AT command more
  
  sendcom("AT+ORGL");
  sendcom("AT+NAME=master");
  sendcom("AT+RMAAD");
  sendcom("AT+ROLE=1");
  sendcom("AT+RESET");
  sendcom("AT+CMODE=1");
  sendcom("AT+INQM=0,5,5");
  sendcom("AT+PSWD=1234");
  sendcom("AT+INIT");
  sendcom("AT+INQ");
  sendcom("AT+PAIR=2013,9,230265,500");
  sendcom("AT+BIND=2013,9,230265");
  sendcom("AT+CMODE=0");
  sendcom("AT+LINK=2013,9,230265");
  
  Serial.println("Enter AT commands:");
}

void loop()
{

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  if (BTSerial.available())
    Serial.write(BTSerial.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  if (Serial.available())
    BTSerial.write(Serial.read());
}

void sendcom(String cmd)
{
  Serial.println(cmd);
  BTSerial.println(cmd);
  delay(100);
  while (BTSerial.available())
  Serial.write(BTSerial.read());
}
