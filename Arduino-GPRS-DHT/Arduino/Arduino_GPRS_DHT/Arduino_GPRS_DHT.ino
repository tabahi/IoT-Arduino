#include <Narcoleptic.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#define DHTPIN 4     // whatever your DHT sensor pin is connected to
#define DHTTYPE DHT11   // DHT 11 

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial GPRSS(2, 3); // Rx, Tx

char server[] = "web.server.com";  // Your PHP based webserver address
String server_file = "/DHTdata/test.php"; // The directory of the script which is reading this request
boolean gprsok = false;

boolean gprson = false;

String APNname = "APN";//SET APNname here
String APNuser = "";
String APNpass = "";

const int events_to_buffer = 20; // Last 20 values will be stored if connection is not established times and again.
struct event
{
  int status;      //for current status of log. 0 means empty, 1 means logged but not sent, 2 means sent.
  float t1;
  float t2;
  unsigned long time;  //will hold value of millis()
  int counter;
};

struct temperatures
{
  float t1;
  float t2;
};

struct event eventlog[events_to_buffer + 1];  //a buffer to hold values of pressure and temperature
int events_in_buffer = 0;

int events_happened = 0;
int events_sent = 0;

boolean first_try_failed = false;

void setup()
{
  pinMode(8, OUTPUT);
  Serial.begin(9600);
  GPRSS.begin(4800);
  dht.begin();
  Serial.println("Initializing GPRSS...");
  while (gprs_initialize() == false)
  {
    Serial.println("GPRS Problem");

      delay(11110);
    
  }
  else
  {
    Serial.println("GPRS OK");
  }

  for (int e = 0; e <= events_to_buffer; e++)
  {
    eventlog[e].status = 0;  //setting buffer status to zero. means its empty
  }
}

void loop()
{
  //This is how to log any values to buffer:

  log_activity();
  delay(100);

  for (int e = 0; e <= events_to_buffer; e++)
  {
    if (eventlog[e].status == 1) //if this buffer status is 'logged but not sent yet'.
    {
      if (httpRequest(eventlog[e].t1, eventlog[e].t2, eventlog[e].time, eventlog[e].counter) == true)
      {
        Serial.println("Log no. " + String(e) + " sent successfully");
        eventlog[e].status = 2;   //set status of logged to 'sent'.
      }
      else
      {
        Serial.println("Log no. " + String(e) + " sending failed");
        first_try_failed = true;
      }
    }
  }

  if (first_try_failed == false) //an hour delay
  {
    while (check_power_on(10000) == true)
    {
      GPRSpowerSwitch();
    }
    Serial.println("GPRS is OFF");
    gprson = false;
    for (int mins = 0; mins < 60; mins++)
    {
      Narcoleptic.delay(60000); //a minute
    }
  }
  else    // 5 minute delay
  {
    first_try_failed = false;
    for (int mins = 0; mins < 5; mins++)
    {
      Narcoleptic.delay(60000); //a minute
    }
  }

}


struct temperatures read_temp()
{
  // Wait a few seconds between measurements.

  double avg_cel = 0;
  double avg_fah = 0;

  struct temperatures tempsss;

  for (int i = 0; i < 10; i ++)
  {
    delay(2000);
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)

    float t = dht.readTemperature();
    // Read temperature as Fahrenheit
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if ( isnan(t) || isnan(f) )
    {
      Serial.println("Failed to read from DHT sensor!");
    }
    avg_cel += t;
    avg_fah += f;
  }

  avg_cel /= 10;
  avg_fah /= 10;

  tempsss.t1 = avg_cel;
  tempsss.t2 = avg_fah;

  Serial.print("Avg Temp:\t");
  Serial.print(avg_cel);
  Serial.print(" *C\t");
  Serial.print(avg_fah);
  Serial.print(" *F");

  return tempsss;

}

void log_activity()
{
  struct temperatures tempLog = read_temp();
  eventlog[events_in_buffer].status = 1;
  eventlog[events_in_buffer].t1 = tempLog.t1;
  eventlog[events_in_buffer].t2 = tempLog.t2;
  eventlog[events_in_buffer].time = millis();
  eventlog[events_in_buffer].counter = events_happened;
  events_in_buffer++;
  events_happened++;
  if (events_in_buffer > events_to_buffer)
  {
    events_in_buffer = 0;
  }
}

void GPRSpowerSwitch()
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

boolean gprs_initialize()
{
  boolean ret = false;
  delay(100);

  while (check_power_on(10000) == false)
  {
    GPRSpowerSwitch();
  }
  Serial.println("GPRS is ON");
  gprson = true;
  sendcom("AT+CIPSHUT");
  check_response(1000, true);
  delay(500);
  sendcom("AT+CIPSTATUS");
  check_response(1000, true);
  delay(500);
  sendcom("AT+CIPMUX=0");
  check_response(1000, true);
  delay(500);
  String apnini = "AT+CSTT=\"";
  apnini += APNname;
  apnini += "\",";

  if (APNuser.length() > 0)
  {
    apnini += "\"";
    apnini += APNuser;
    apnini += "\"";
  }
  apnini += ",";
  if (APNpass.length() > 0)
  {
    apnini += "\"";
    apnini += APNpass;
    apnini += "\"";
  }
  sendcom(apnini);
  check_response(5000, true);
  delay(500);
  sendcom("AT+CIICR");
  check_response(3000, true);
  if (gprsok == true)
  {
    ret = true;
  }
  delay(500);
  sendcom("AT+CIFSR");
  check_response(5000, true);


  return ret;
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

boolean httpRequest(float tC, float tF, unsigned long time, int count)
{
  if (gprson == false)
  {
    GPRSpowerSwitch();
    while (gprs_initialize() == false)
    {
      delay(5000);
    }
  }
  unsigned long delay_in_sending = millis() - time;
  boolean rep = false;
  String data = "t1=" + String(tC) + "&t2=" + String(tF)  + "&delay=" + String((unsigned long)(delay_in_sending / 1000)) + "&count=" + String(count);


  String hostnamereq = "Host: ";
  hostnamereq += server;


  sendcom("AT+CIPSTART=\"TCP\",\"" + String(server) + "\",80");
  delay(1000);
  check_response(10000, true);
  delay(500);

  String getvar = "POST ";
  getvar += server_file;
  getvar += " HTTP/1.1\r\n";
  getvar += hostnamereq + "\r\n";
  getvar += "Content-Type: application/x-www-form-urlencoded\r\n";
  getvar += "User-Agent: ArduinoGPRS/1.1\r\n";
  getvar += "Connection: close\r\n";
  getvar += "Content-Length: " + String(data.length()) + "\r\n\r\n";
  getvar += data;
  getvar += "\r\n\r\n";
  check_response(500, false);
  sendcom("AT+CIPSEND=" + String(getvar.length())); // Increase 3 to higher values like 4 or 5 , it needed
  delay(1000);
  check_response(1000, true);
  check_response(1000, false);
  Serial.println("...");
  getvar = "";
  sendit("POST ");
  sendit(server_file);
  sendit(" HTTP/1.1\r\n");
  sendit(hostnamereq + "\r\n");
  sendit("Content-Type: application/x-www-form-urlencoded\r\n");
  sendit("User-Agent: ArduinoGPRS/1.1\r\n");
  sendit("Connection: close\r\n");
  sendit("Content-Length: ");
  sendit(String(data.length()) + "\r\n\r\n");
  sendit(data);
  sendit("\r\n\r\n");

  delay(1000);

  rep = false;
  String check = "";
  int waiter = 0;
  while (waiter < 1000)
  {
    while (GPRSS.available())
    {
      char c = GPRSS.read();
      Serial.write(c);
      check += c;

      if ((check.indexOf("CLOSED") != -1) || (c=='@'))
      {
        rep = true;
      }
    }
    delay(10);
    waiter++;
  }
  delay(1000);
  sendcom("AT+CIPCLOSE");
  check_response(1000, true);


  delay(100);;
  return rep;;
}

void sendit(String com)
{
  Serial.println("Sending: " + com);
  GPRSS.print(com);
  delay(10);
}

void sendcom(String com)
{
  Serial.println("\r\nCMD: " + com);
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
