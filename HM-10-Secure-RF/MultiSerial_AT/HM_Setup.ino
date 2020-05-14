boolean setup_HM10()
{
  check_busy();

  cmd(F("AT+RESET "));
  check_busy();


  cmd(F("AT+ADDR?"));
  get_response(1000, 2000, F("OK+ADDR:74DAEXXXXXX"), errorstring);

  String st = "AT+PASS";
  st += pass_code;
  cmd(st);

  if (get_response(100, 3000, pass_code, errorstring) == false)
  {
    Serial.println(F("Can't set passcode"));
    return false;
  }
  cmd("AT+TYPE2");
  get_response(100, 1000, F("OK+Set:2"), errorstring);

  cmd("AT+MODE0");    //0=Trasmission, 1=Remote Control, 2=Remote Collect
  get_response(50, 5000, F("OK+Set:0"), errorstring);

  cmd("AT+NOTI1");
  get_response(100, 1000, F("OK+Set:1"), errorstring);

  if (ROLE == 1) //central
  {
    cmd("AT+ROLE1");    //0 peri , 1 central
    if (get_response(50, 5000, F("OK+Set:1"), errorstring) == false)
    {
      Serial.println(F("Can't set ROLE"));
      return false;
    }

    //cmd("AT+FILT0");
    //get_response(5000, 1000, "OK", errorstring);

    cmd("AT+DISC?");
    get_response(200, 10000, F("OK+DISCS:"), F("+DISCE"));

    st = "AT+CON";
    st += remote_ADDR;
    cmd(st);
    if (get_response(10, 2000, "CONNA", "CONNE") == false) //connect
    {
      Serial.println(F("Connect Error"));
      return false;
    }
    else
    {
      Serial.println(F("Sending Request"));
      return true;
    }
  }
  else
  {
    cmd("AT+ROLE0");    //0 peri , 1 central
    if (get_response(50, 5000, F("OK+Set:0"), errorstring) == false)
    {
      Serial.println(F("Can't set ROLE"));
      return false;
    }
    Serial.println(F("Waiting for connection"));
  }
  return true;
}


void check_busy()
{
  int failed_tries = 0;
  cmd("AT");
  while (get_response(50, 1000, "OK", errorstring) == false)
  {
    delay(100);
    failed_tries++;
    cmd("AT");
    if (failed_tries > 6)
    {
      failed_tries = 0;
      Serial.println(F("Error: No response from module"));
      delay(3000);
    }
  }
}

/*

  //cmd("AT+SHOW1");
    //get_response(500, 5000, "OK+Set:1", "ERROR");

  cmd("AT+PCTL0");    //normal power
      get_response(100, 500, "OK", "ERROR");
      cmd("AT+POWE0");    //min power   0 - 3
      get_response(100, 500, "OK", "ERROR");


  cmd("AT+SLEEP");
    get_response(100, 500, "OK", "ERROR");
*/

