
#define MAX_MSG_LENGTH 64

enum states
{
  wait_for_new_msg,
  reading,
  msg_finished_encrypted,
  msg_finished_non_encrypted
};

states status = wait_for_new_msg;
String inString = "";
//add inString.reserve(MAX_MSG_LENGTH); to setup()

unsigned long msg_start_timer = 0;


void BT_read()
{
  while (btleSerial.available())
  {
    char inChar = btleSerial.read();

    if (inChar == '&')
    {
      msg_start_timer = millis();
      inString = "";
      status = reading;
    }

    if (status == reading)
    {
      if (inChar == '#')
      {
        btleSerial.write("ACK0000"); // Order new part
        Serial.println(F("PETICION SIGUIENTE MENSAJE"));
        Serial.println(F("FIN DE RECEPCION CORRECTA TRAMO"));
      }
      else if (inChar == '@') //End of encrypted message
      {
        int msg_length = inString.length();
        Serial.print(F("MSG length:"));
        Serial.println(msg_length);
        
        status = msg_finished_encrypted;
        if (msg_length == 57)
        { //antes 89
          Serial.println(F("FIN DE RECEPCION CORRECTA MENSAJE"));
          //do something with encrypted msg here
        }
        else
        {
          Serial.println(F("FIN DE RECEPCION INCORRECTA MENSAJE"));
        }

        //reset the status
        status = wait_for_new_msg;
        inString = "";
      }
      else if (inChar == '$') // End of non-encrypted message. Only must have less than 16 bytes.
      {
        status = msg_finished_non_encrypted;
        iv1_string = inString; //We use iv1string to save memory
        Serial.print(F("Mensaje para no desencriptar: "));
        Serial.println(iv1_string);
        Serial.println(F("FIN DE RECEPCION MENSAJE SIN ENCRIPTAR"));
        //do something with msg here

        //reset the status
        status = wait_for_new_msg;
        inString = "";
      }
      else if (((inChar >= 48) && (inChar <= 57)) || ((inChar >= 97) && (inChar <= 122))) //only alpha-numeric characters, small case
      {
        inString += inChar; //add to string
      }
      else
      {
        //some unexpected character is recieved here.
      }

      if ((inString.length() > MAX_MSG_LENGTH) || ((millis() - msg_start_timer) > 5000)) //if length exceeds 64 OR time exceed 5 seconds then reset msg
      {
        inString = ""; //back to start
        status = wait_for_new_msg;
      }
    }




  }
}

