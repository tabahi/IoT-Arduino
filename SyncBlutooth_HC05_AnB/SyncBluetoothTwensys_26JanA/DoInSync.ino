

void DoInSync()
{
  
  
  // Code to execute after synchronization
  
  
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

  for (t = 0; t < nodes; t++) 
  {                                   // print all the values of the array one by one
    ISRreport += String(readings[t]);
    ISRreport += "\t";
  } 
  ISRreport += "\r\n";
 
}

