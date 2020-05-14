void sendcom(String cmd, unsigned long dela)
{
  //Serial.println(cmd);
  BTSerial.println(cmd);
  unsigned long delai = 0;

  String lala = "";
  while (delai < dela)
  {
    while (BTSerial.available())
    {
      char c = BTSerial.read();
      lala += c;
    }
    if (lala.indexOf("OK") != -1)
    {
      delai = dela;
    }
    delay(10);
    delai = delai + 10;
  }
  if (lala.indexOf("OK") < 0)
  {
    Serial.print("\r\nError Y\r\n");
  }
  //Serial.print(lala);
}


void sendINQ(String cmd, unsigned long dela)
{
  //Serial.println(cmd);
  BTSerial.println(cmd);
  unsigned long delai = 0;
  String lala = "";

  while (delai < dela)
  {
    while (BTSerial.available())
    {
      char c = BTSerial.read();
      if (c == '\r')
      {
        String s2 = lala;
        lala = "";
        if (s2.indexOf("INQ:") != -1)
        {
          String blid = s2.substring((s2.indexOf(":") + 1), 22);
          blid = blid.substring(0, blid.indexOf(','));
          s2 = s2.substring(s2.indexOf(',') + 1);


          String class_s = s2.substring(0, s2.indexOf(','));
          s2 = s2.substring(s2.indexOf(',') + 1);
          String rssi_s = s2.substring(0, 4);


          unsigned long rssi_number = (unsigned long) strtol( &rssi_s[0], NULL, 16);

          Serial.println();
          Serial.print("MAC: " + blid);
          Serial.print("\t RSSI: " + String(rssi_number));
          if (rssi_number > (66472 - range))
          {
            Serial.print("\t In Range!");
            boolean alogged = false;
            boolean space_free = false;
            for (int k = 0; k < bluetooths_to_buffer; k++)
            {
              if ((eventlog[k].ID.indexOf(blid) != -1) && (eventlog[k].inrange == true))
              {
                Serial.print(" Update " + String(k));
                alogged = true;
                eventlog[k].ltime = millis();
                eventlog[k].rssi = rssi_number - 66472 + range;
                eventlog[k].inrange = true;
                break;
              }
            }
            if (alogged == false)
            {
              for (int k = 0; k < bluetooths_to_buffer; k++)
              {
                if ((eventlog[k].inrange == false) && (eventlog[k].rssi<=0))
                {
                  Serial.print(" Space filled " + String(k));
                  space_free = true;
                  eventlog[k].ID = blid;
                  eventlog[k].stime = millis();
                  eventlog[k].ltime = millis();
                  eventlog[k].rssi = rssi_number - 66472 + range;
                  eventlog[k].inrange = true;
                  eventlog[k].sent1 = false;
                  // Serial.println(eventlog[k].ID);
                  break;
                }
              }

              if (space_free == false)
              {
                Serial.print(" Buffer incremented " + String(clog));
                eventlog[clog].ID = blid;
                eventlog[clog].stime = millis();
                eventlog[clog].ltime = millis();
                eventlog[clog].rssi = rssi_number - 66472 + range;
                eventlog[clog].inrange = true;
                eventlog[clog].sent1 = false;

                clog++;
                if (clog >= bluetooths_to_buffer)
                {
                  clog = 0;
                }

              }
            }
          }

        }
      }
      
      
      lala += c;
      //Serial.write(c);
    }
    if (lala.indexOf("OK") != -1)
    {
      delai = dela;
    }
    delay(1);
    delai = delai + 1;
  }

  if (lala.indexOf("\r") == -1)
  {
    Serial.print("\r\nError X\r\n");
  }

}


unsigned long hex2int(char *a, unsigned int len)
{
  int i;
  unsigned long val = 0;

  for (i = 0; i < len; i++)
    if (a[i] <= 57)
      val += (a[i] - 48) * (1 << (4 * (len - 1 - i)));
    else
      val += (a[i] - 55) * (1 << (4 * (len - 1 - i)));
  return val;
}
