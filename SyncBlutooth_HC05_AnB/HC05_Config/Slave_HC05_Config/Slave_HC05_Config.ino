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
  sendcom("AT+NAME=slave");
  sendcom("AT+RMAAD");
  sendcom("AT+PSWD=1234");
  sendcom("AT+ROLE=0");
  sendcom("AT+ADDR?");
  
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
