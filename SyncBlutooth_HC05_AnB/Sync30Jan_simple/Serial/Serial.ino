const int ledPin = 13;
const int scopePin = 12;

uint8_t Buffer[768];

void setup() 
{
  pinMode(ledPin, OUTPUT);
  pinMode(scopePin, OUTPUT);
  
  Serial.begin(9600);
  Serial.flush();
}

void loop() 
{
  uint16_t length = Serial.available();
  if (length > 0)
  {
    digitalWrite(ledPin, HIGH);
    Serial.readBytes((char *)Buffer, length);
    
    Serial.write(Buffer, length);
    Serial.send_now();
    
    digitalWrite(ledPin, LOW);
  }
}

