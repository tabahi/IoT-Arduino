#include <SPI.h>
#include <Ethernet.h>


IPAddress server_ip(1, 1, 1, 1);
const int server_port = 6789;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress arduino_manual_ip(192, 168, 1, 177);



EthernetClient client;
int flag = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Connecting to Ethernet..."));

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println(F("Failed to configure Ethernet using DHCP"));
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, arduino_manual_ip);
  }

  delay(1000);
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());

  //client.setTimeout(30);



}

String ss = "";
char read_or_write = 'N';

void loop()
{

  if (flag == 0)
  {
    Serial.println(F("Enter 'r' to read or 'w' to write SQL data"));
    flag = 1;
    read_or_write = 0;
    ss = "";
  }
  else if (flag == 1)
  {
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      if ((c == 'r') || (c == 'R'))
      {
        read_or_write = 'R';
        flag = 2;
      }
      else if ((c == 'w') || (c == 'W'))
      {
        read_or_write = 'W';
        flag = 2;
      }
      else
      {
        flag = 0;
      }
    }
  }
  else if (flag == 2)
  {
    Serial.println("Now enter SQL command");
    ss = "";
    delay(100);
    while (Serial.available() > 0)
    {
      char c = Serial.read();
    }
    Serial.flush();
    flag = 3;
  }
  else if (flag == 3)
  {
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      if ((c == '\n') || (c == '\r'))
      {
        if (ss.length() > 3)
        {
          Serial.println(F("Sending SQL querry: "));
          Serial.println(ss);
          String results = send_sql_command(">>" + String(read_or_write) + "##(" + ss + ")##<<\n");

          Serial.println(results);  //print results to serial monitor
          ss = "";
          
        }

        flag = 0; //reset client
      }
      else
      {
        ss += c;
        if (ss.length() > 70)
        {
          ss = ss.substring(10);
        }
      }
    }
  }



}


String send_sql_command(String cmd)
{
  Serial.println(F("Connecting to server..."));
  if (client.connect(server_ip, server_port))
  {
    Serial.println("connected");
  }
  else
  {
    //Serial.println(F("connection failed"));
    return "CONNECTION ERROR";
  }
  if (client.connected())
  {
    client.print(cmd);
  }
  else
  {
    //Serial.println(F("connection failed"));
    return "CONNECTION ERROR";
  }

  String results = "";
  unsigned long startTime = millis();

  while ((millis() - startTime) < 10000)
  {
    while (client.available())
    {
      char c = client.read();
      results += c;
    }
    if (results.indexOf("END") >= 0)
    {
      break;
    }
  }


  if (!client.connected())
  {
    Serial.println();
    Serial.println(F("disconnecting."));
    client.stop();
  }

  return results;
}




