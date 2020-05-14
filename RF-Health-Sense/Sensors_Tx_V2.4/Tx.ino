//433Mhz Transmission part


void senddata()
{
  String data = "";
  
  
  
  data += "&P:";
  data += (int)highbeat;
  data += ",";
  data += (int)lowbeat;
  data += ",";
  data += String((float)t);
  data += "$---";
  sendmsg(data);
  data = "";
  
  
  data += "&C:";
  data += (int)red;
  data += ",";
  data += (int)green;
  data += ",";
  data += (int)blue;
  data += "$-";
  sendmsg(data);
  data = "";

  
  
}


void sendmsg(String smsg)
{
  unsigned int len = smsg.length() + 1;
  char msg[len];
  smsg.toCharArray(msg, len);

  //Serial.print("Sending: ");
  //Serial.print(msg);
  vw_send((uint8_t *)msg, strlen(msg));
  //Serial.print("..");
  vw_wait_tx(); // Wait until the whole message is gone
  //Serial.println(".!");
}
