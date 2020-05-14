#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <VirtualWire.h>

// pin definition for the Uno
#define cs   10
#define dc   9
#define rst  8

const int textsize = 2;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
String rs = "";


//parameters defining


int pitch = 0;


int red = 0;
int green = 0;
int blue = 0;

int highbeat = 0;
int lowbeat = 0;

float t  = 0;

boolean se = true;
String s = "";
unsigned long timer1 = 0;
unsigned long timer2 = 0;


void setup() 
{

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(textsize);



  Serial.begin(9600);
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_set_rx_pin(12);
  vw_setup(4000);  // Bits per sec\

  vw_rx_start();       // Start the receiver PLL running
  Serial.println("Listening...");
  /*
  collect("@@@---&A:-11,22,33$---");
   collect("&O:22,500,68$---");
   collect("&G:333,-90,22$---");
   collect("&P:23,23,931697$---");
   collect("&L:83,163,163$---###");
   */
  lcdprint();

}

void loop() 
{

  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t buflen = VW_MAX_MESSAGE_LEN;
  String rec = "";
  if (vw_get_message(buf, &buflen)) // Non-blocking
  {
    Serial.print("Rx: ");
    for(int i=0; i<buflen; i++)
    {
      char chr = buf[i];
      rec += chr;
    }
    Serial.println(rec);
    collect(rec);
    //lcdprint();
  }
  if(millis()-timer1>500)  //refresh screen after 5 seconds
  {
    lcdprint();
    timer1 = millis();
  }

}

void collect(String col)
{
  rs += col;
  while( (rs.indexOf('&')>-1) && (rs.indexOf('$')>-1) && ((rs.indexOf('$')) > (rs.indexOf('&')))   )
  {
    String rs2 = rs.substring( rs.indexOf('$') + 1 );
    String ws = rs.substring(rs.indexOf('&') + 1, rs.indexOf('$') + 1);
    rs = rs2;


    // Serial.println(ws);
    if((ws[1]==':') && (ws.indexOf('$')>1))
    {
      if(ws[0]=='P')
      {
        
        highbeat = ws.substring(2, ws.indexOf(',')).toInt();
        ws = ws.substring(ws.indexOf(',') + 1);
        lowbeat = ws.substring(0, ws.indexOf(',')).toInt();
        t = ws.substring(ws.indexOf(',') + 1, ws.indexOf('$')).toFloat();

        Serial.println("H:" + String(highbeat) + "," + String(lowbeat) + "\tT:" + String(t));
      }
      else if(ws[0]=='C')
      {        
        red = ws.substring(2, ws.indexOf(',')).toInt();
        ws = ws.substring(ws.indexOf(',') + 1);
        green = ws.substring(0, ws.indexOf(',')).toInt();
        blue = ws.substring(ws.indexOf(',') + 1, ws.indexOf('$')).toInt();
        Serial.print("\tRGB(");
        Serial.print(red);
        Serial.print(",");
        Serial.print(green);
        Serial.print(",");
        Serial.print(blue);
        Serial.println(")");
      }



    }
  }

}


void lcdprint()
{

  if(s.length()>0)
  {
    //clearing screen
    TFTscreen.stroke(0, 0, 0);
    //coverting to char array
    char carrayv[s.length() + 1];
    s.toCharArray(carrayv, s.length() + 1);

    TFTscreen.text(carrayv, 0, 0);
  }

  String s2 = "";
  
  se = true;

  if(se==true)
  { 
    s2 += "Heart: ";
    s2 += String(highbeat);
    s2 += "  ";
    s2 += String(lowbeat);
    s2 += "\n";
    s2 += "Temp: ";
    s2 += String(t);
    s2 += "\n";
    s2 += "RGB(";
    s2 += String(red);
    s2 += ",";
    s2 += String(green);
    s2 += ",";
    s2 += String(blue);
    s2 += ")";
    s2 += "\n";
    s = s2;
  }

  if(millis() - timer2 >5000)
  {
    se = !se;
    timer2 = millis();
    Serial.println(s);
  }

  //coverting to char array
  char carray[s.length() + 1];
  s.toCharArray(carray, s.length() + 1);

  TFTscreen.stroke(255, 255, 255);
  TFTscreen.text(carray, 0, 0);

}



