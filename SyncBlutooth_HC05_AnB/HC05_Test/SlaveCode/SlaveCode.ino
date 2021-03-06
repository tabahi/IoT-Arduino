#include <SoftwareSerial.h>


int mode = 0;  //   1=master  0=slave


SoftwareSerial BTSerial(7, 8); // RX | TX



void setup() 
{

  pinMode(14, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin) HIGH to switch module to AT mode
  digitalWrite(14, HIGH);
  
  Serial.begin(19200);
  Serial.println("...");

  BTSerial.begin(38400);  // HC-05 default speed in AT command mode

  delay(100);


  send_command("AT");
  check_response(10);
  send_command("AT+NAME=BT1");
  check_response(20);
  send_command("AT+UART=19200,1,0 ");
  check_response(20);
  send_command("AT+ROLE=0");
  check_response(20);
  send_command("AT+ROLE?");
  
 
  send_command("AT+ADDR?");
  check_response(30);
  send_command("AT");
  check_response(30);

  delay(200);
  
  digitalWrite(14, LOW);

}





void loop() 
{
  send_command("Slave:Hello");
  delay(100);
  check_response(30);
  delay(5000);
}



void send_command(String command)
{
  Serial.println(command);
  BTSerial.println(command);
}

void check_response(int uuu)
{

  int wait = 0;
  String recieved = "";
  while((BTSerial.available())&&(recieved.length()<uuu))
  {
    char abc = BTSerial.read();
    recieved += abc;
  }
  if(recieved.length()>1)
  {
    Serial.println("Recieved:");
    Serial.println(recieved);
    recieved = "";
  }
  else
  {
    Serial.println("Nothing Recieved");
  }
}





