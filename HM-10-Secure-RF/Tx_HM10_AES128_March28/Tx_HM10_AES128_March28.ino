#include <AESLib.h>
#include <SoftwareSerial.h>

#define ENCRYPTED_DATA_MAX_SIZE 32 //very risky above 32 - UNO,MINI,NANO can't handle large data

//Bluetooth parameters
#define ROLE 0 //0=Peripheral,  1=central
#define remote_ADDR "60D9A0326E4C"
#define pass_code "004483"  //6 characters only
#define RECONNECT_TIMEOUT 120000 //ms

//Encryption parameters
#define skLlave "vr7u45zg3rrrwvng"  //must be 16 bytes
#define ivLlave "agguy7e5nfff1iog"  //must be 16 bytes
//#define passId "ertuy3e3nddd1iog"   //must be 16 characters



/*******************************************************************/



//AES encypted data sample:
//91f7c2c50f40bfda724a837b61491a12cfbf08bed70fa6e9d24fb85aecf17803  //size: 32 bytes  (HEX: 64 chars)
//f615bd15de5caa1fc3222a602ac7ce16ecbe6aeb357bae0fd12d1dcf57ab3a47

uint8_t key[] = skLlave;
uint8_t iv[] = ivLlave;

#if !(defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__))
SoftwareSerial Serial1(10, 11); // RX, TX
#endif

const unsigned char enc_data_input_size = (ENCRYPTED_DATA_MAX_SIZE*2);  //doubles because HEX format takes 2 characters per byte
char recdata[enc_data_input_size];
boolean newdata = false;

const String errorstring = "ERROR";
boolean BLEconnected = false;
String ACKenc = "";
String xs;
String HM_msg;


unsigned long HM_msg_timeout = 0;
unsigned long connnection_retry_timer = 0;

void setup()
{
  xs.reserve(enc_data_input_size + 2);
  HM_msg.reserve(enc_data_input_size + 2);
  HM_msg = "";
  Serial.begin(9600);
  Serial1.begin(9600);    //initialize with baudrate of HM-10 serial
  Serial.println(F("Start"));
  delay(100);


  //AES Testing:
  Serial.println(F("TESTING"));
  Serial.println(decrypt_data(encrypt_data(F("Hello There")))); // encrypts then decrypts
  Serial.println(encrypt_data(F("123456789abcdefghijklmopq")));
  Serial.println(decrypt_data(F("91f7c2c50f40bfda724a837b61491a12cfbf08bed70fa6e9d24fb85aecf17803")));
  Serial.println(F("---TESTING END---"));





  ACKenc = encrypt_data("ACK");

  delay(100);
  Serial.println(F("Connecting BLE"));
  while (setup_HM10() == false)
  {
    delay(5000);
  }
  Serial.println("...");
}


void loop()
{

  while (Serial1.available())   //reading messages from HM-10
  {
    char c = Serial1.read();
    HM_msg += c;
    HM_msg_timeout = millis();
  }
  int HM_msg_length = HM_msg.length();

  if ((((millis() - HM_msg_timeout) > 500) && (HM_msg_length > 0)) || (HM_msg_length > enc_data_input_size)) //if something recieved
  {
    //Trim to size limit of enc_data_input_size
    if (HM_msg_length > (enc_data_input_size + 1))
    {
      HM_msg = HM_msg.substring(HM_msg_length - enc_data_input_size - 1);
    }

    if (HM_msg.indexOf(F("OK+LOST")) != -1)
    {
      BLEconnected = false;
      Serial.println(F("Connection Lost"));
      digitalWrite(13, LOW);
      HM_msg = "";
    }
    else if ((HM_msg.indexOf(F("OK+CONNF")) != -1) || (HM_msg.indexOf(F("OK+CONNE")) != -1))
    {
      BLEconnected = false;
      Serial.println(F("Couldn't connect"));
      digitalWrite(13, LOW);
      HM_msg = "";
    }
    else if (HM_msg.indexOf(F("OK+CONN")) != -1)
    {
      BLEconnected = true;
      Serial.println(F("Connected"));
      digitalWrite(13, HIGH);
      HM_msg = "";
    }
    else if (HM_msg.indexOf(ACKenc) != -1)
    {
      BLEconnected = true;
      Serial.println(F("ACK recieved"));
      HM_msg = "";
    }

    if (BLEconnected == true)
    {
      //Encypted data recieved
      int index_r = HM_msg.indexOf('\r');
      int index_n = HM_msg.indexOf('\n');

      if ((index_r >= 1) || (index_n >= 1)) //if NL or CR are found and size of msg is big enough
      {
        if ((index_r >= 1))
        {
          HM_msg = HM_msg.substring(0, index_r);
        }
        if (((index_n >= 1)) && (index_n < index_r))
        {
          HM_msg = HM_msg.substring(0, index_n);
        }
        while (HM_msg.length() > enc_data_input_size)
        {
          HM_msg = HM_msg.substring(1);
        }
        Serial.print(F("RECIEVED:"));
        xs = decrypt_data(HM_msg);
        HM_msg = "";
        Serial.println(xs);
        if (xs.indexOf(ACKenc) != -1)
        {
          Serial.println(F("Remote ACK recieved"));
        }
        else
        {
          Serial.println(F("ACK sent"));
          Serial1.println(ACKenc);
        }
      }
    }
    else if (HM_msg.length() > 0)
    {
      Serial.print(">");
      Serial.println(HM_msg);      //prints data recived when there is no connection and it doesn't know what to do with this data
      HM_msg = "";
    }


  }


#if ROLE    //for master only
  if ((BLEconnected == false) && ((millis() - connnection_retry_timer) > RECONNECT_TIMEOUT) )
  {
    HM_msg = "";
    Serial.println(F("Trying Again"));
    setup_HM10();
    connnection_retry_timer = millis();
  }
#endif



  //USB input
  while (Serial.available())
  {
    char ch = Serial.read(); 
    Serial1.write(ch);        //directtly foraward everything to HM-10
  }
  
  
}
