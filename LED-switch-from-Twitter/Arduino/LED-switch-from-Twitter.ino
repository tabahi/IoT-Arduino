/*
Read tweets from twitter(screen_name is used) to switch led on and off

Tweets to control led:
#switchledON
#switchledOFF

*/


#include <SPI.h>
#include <WiFi.h>



//Dont forget to set these:

String led_on_tag = "#switchledON";
String led_off_tag = "#switchledOFF";

char screen_name[] = "testwonrobo";    //twitter screen name to read the hashtag status for led on/off from. eg: (www.twitter.com/xyz)
char ssid[] = "canpuffin";      //  your network SSID (name)
char pass[] = "secretPassword";   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
const int ledPin =  8;     //pin at which led is attched

int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "server.com";  //a php api server address
unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds  (5 here is the number of seconds it will wait to check the twitter again)

String con = "";






void setup() 
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);


  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) 
  {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }


  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 5 seconds for connection:
    delay(5000);
  }
  // you're connected now, so print out the status:
  printWifiStatus();
}









void loop() 
{
  while (client.available()) 
  {
    char c = client.read();
    con += c;
  }
  if(con.length()>1)
  {
    Serial.println(con);   
    if(Contains(con, led_on_tag))
    {
      Serial.println(led_on_tag);
      digitalWrite(ledPin, HIGH); 
	  Serial.println("LED is ON now.");
	  Serial.println();
    }
    else if (Contains(con, led_off_tag))
    {
      Serial.println(led_off_tag);      
      digitalWrite(ledPin, LOW); 
	  Serial.println("LED is OFF now.");
	  Serial.println();
    }
    else
    {
      Serial.println("Response recieved but No LED #tag): ");
      Serial.println(con);  
      Serial.println("-----");
    }
    con = "";
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send request:
  if (millis() - lastConnectionTime > postingInterval) 
  {
    httpRequest();
  }
}




void httpRequest() 
{
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  //Serial.println("Checking twitter..."); 
  // if there's a successful connection:
  if (client.connect(server, 80))   
  {
    String getvar = "GET /twitter/index.php?a=";
    getvar += screen_name;
    getvar += " HTTP/1.1";
    //Serial.println(getvar);
    client.println(getvar);
    client.println("Host: vixor.acoxi.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    
    delay(500);
    client.stop();
    delay(500);
    //Serial.println("...");
    if (client.connect(server, 80)) 
    {
    String getvar2 = "GET /twitter/";
    getvar2 += screen_name;
    getvar2 += "_tweet.txt HTTP/1.1";
    //Serial.println(getvar2);
    client.println(getvar2);
    client.println("Host: vixor.acoxi.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    }
    else
    {
     //Serial.println("C1");
    }

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else 
  {
    Serial.println("connection failed");
  }
}





void printWifiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}



boolean Contains(String s, String search) 
{
  String ss = s;
  int tr = 0;
  while((ss.indexOf(search.charAt(0))!=-1)&&(tr<5))
  {
    String firstcut = ss.substring(ss.indexOf(search.charAt(0)));
    if(CContain(search, firstcut))
    {
     return true; 
    }
    else
    {
     ss =  ss.substring(ss.indexOf(search.charAt(0)) + 1);
    }
    tr++;
  }
  return false;
} 


boolean CContain(String search, String firstcut)
{
    if(search.charAt(0)==firstcut.charAt(0))
    {
      int ln = search.length();
      for(int j=0; j<ln; j++)
      {
        if(search.charAt(j)==firstcut.charAt(j))
        {
          if(j==(ln-1))
          {
            return true;
          }
        }
        else
        {
          break;
        }
      }
    }
    return false;
}
