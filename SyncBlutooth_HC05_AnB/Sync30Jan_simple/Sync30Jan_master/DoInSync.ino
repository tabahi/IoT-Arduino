
void DoInSync()
{
  for (j = 0; j < digitalNum; j++)  
  {                              // loop over all digital pins
    digitalWrite(digitalPin[j], HIGH);                             // set digital pins one by one high 
    for (k = 0; k < analogNum; k++)  
    {                            // loop over all analog pins
      s = s++;                                                    // to put the readings at the right place in array
      if (s >= nodes)  
      {
        s = 0;
      }
      readings[s] = analogRead(analogPin[k]);                     // read all the analog pins
    }
    digitalWrite(digitalPin[j], LOW);                              // set the digital pin back to LOW
  }
  /*
  for (t = 0; t < nodes; t++)  {                                   // print all the values of the array one by one
    Serial.print(readings[t]);
    Serial.print("\t");
  } 
    int timestamp = millis();
    Serial.print(timestamp); 
    Serial.println("");
      */  
  reading_no++;
  //Example ISRreport:  8388#99,189,99,10,22,22,55,88,44,
  ISRreport +=  String(reading_no) + "#," + String(millis()) + ",";
  for (t = 0; t < nodes; t++) 
  {
    ISRreport += String(readings[t]);
    ISRreport += ",";
  }
  ISRreport += "a";

}


