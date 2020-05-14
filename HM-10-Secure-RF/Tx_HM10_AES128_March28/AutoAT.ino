

boolean get_response(unsigned long ts_min, unsigned long ts_max, String expected, String expected_error)
{

  String ss = "";
  unsigned long tpassed = 0;
  boolean exp_found = false;
  boolean exp_error_found = false;
  int lexp = expected.length();

  while ((tpassed < ts_max) && (((!exp_found) && (!exp_error_found)) || (tpassed < ts_min)) )
  {
    while (Serial1.available())
    {
      char incHAR = Serial1.read();
      ss += incHAR;
    }
    if (((ss.indexOf(expected_error) != -1) || ss.indexOf(errorstring) != -1) || (ss.indexOf("CONNE") != -1) ) //|| (ss.indexOf("CONNA")) || (ss.indexOf("CONNF"))
    {
      Serial.println(ss);
      ss = "";
      exp_error_found = true;
    }
    else if (ss.indexOf(expected) != -1)
    {
      Serial.println(ss);
      ss = "";
      exp_found = true;
    }
    else if (ss.length() > (lexp * 2))
    {
      Serial.println(ss);
      ss = ss.substring(lexp - 1);
    }
    tpassed = tpassed + 10;
    delay(10);
  }

  if (ss.length() > 0)
  {
    Serial.print("REPLY: \t");
    Serial.println(ss);
  }

  if (exp_found)
    return true;
  else
    return false;
}

void cmd(String ATCmd)
{
  Serial1.print(ATCmd);
  Serial.print("CMD:");
  Serial.println(ATCmd);
}

