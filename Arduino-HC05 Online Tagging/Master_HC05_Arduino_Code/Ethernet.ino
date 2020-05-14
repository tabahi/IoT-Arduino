
unsigned long lastupdate = 0;

void senddata()
{
  String data = "&data=";
  unsigned long cmills = millis();
  boolean updateall = false;
  if ((cmills - lastupdate) > ((unsigned long)(time_to_remember / 2)) )
  {
    updateall = true;
    lastupdate = cmills;
  }

  for (int k = 0; k < bluetooths_to_buffer; k++)
  {
    if ( (eventlog[k].rssi > 0) && ( (eventlog[k].inrange == false)  ||  ( (eventlog[k].inrange == true) && ((eventlog[k].sent1 == false) || (updateall == true)))  ) )
    {
      unsigned long sdelay = cmills - eventlog[k].stime;
      unsigned long duration = eventlog[k].ltime - eventlog[k].stime;
      data += eventlog[k].ID + "$" + eventlog[k].rssi + "$" + String(sdelay) + "$" +  String(duration) + ";";
    
      if ( (eventlog[k].inrange == true) && ((eventlog[k].sent1 == false) || (updateall == true)) )
      {
        eventlog[k].sent1 = true;
      }
    }
  }

  if (data.length() > 10)
  {
    data += "$$$$$";
    if (httpRequest(data) == true)
    {
      for (int k = 0; k < bluetooths_to_buffer; k++)
      {
        if ((eventlog[k].inrange == false) && (eventlog[k].rssi > 0) )
        {
          eventlog[k].ID = "";
          eventlog[k].stime = 0;
          eventlog[k].ltime = 0;
          eventlog[k].rssi = 0;
          eventlog[k].inrange = false;
        }
      }

    }
    else
    {
      Serial.println("Error sending data");
    }
  }

}

boolean httpRequest(String &data)
{
  //Serial.println(data);
  //return true;
  boolean rep = false;


  client.stop();
  if (client.connect(server, 80))
  {
    String hostnamereq = "Host: ";
    hostnamereq += server;

    Serial.println("Sendind data");
    client.print("POST ");
    client.print(server_directory);
    client.println(" HTTP/1.1");
    client.println(hostnamereq);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("User-Agent: ArduinoEthernet/1.1");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.print(data);
    Serial.println(data);
    client.println();


    delay(1000);

    int jksl = 0;

    while (jksl < 1000)
    {
      while (client.available())
      {
        char c = client.read();
        //con += c;
        Serial.write(c);
        if (c == '@')
        {
          rep = true;
        }
      }
      delay(10);
      jksl++;
    }




    delay(100);
    client.stop();
    return rep;;
  }
  else
  {
    Serial.println("Connection failed");
    return false;
  }
}
