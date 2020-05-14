//A7105 transmission with Arduino UNO or MEGA  - DECEMBER 2015


//Settings:
#define output_format 1 // 1=ascii   0=HEX  format of Serial Output

//SPI connection pins with A7105
#define SDI_pin 11    //  SDIO D11 
#define SCLK_pin 13   //  SCLK D13
#define CS_pin 10     //  CS   D10
#define GIO_pin 2     //  GIO2 D2  (Rx Interrupt)

#define channel_1 0x71  //Rx Channel
#define channel_2 0x71  //Tx Channel

#define Tx_count 16       //16 times to send each message
#define PAYLOAD_SIZE 6    //Size of transmission in bytes

uint8_t id[4] = { 0x35, 0x99, 0x9A, 0x5A };   //ID. Both transcievers should have same ID




//Constants:

#ifndef _IFACE_A7105_H_
#define _IFACE_A7105_H_

enum A7105_State {
  A7105_SLEEP     = 0x80,
  A7105_IDLE      = 0x90,
  A7105_STANDBY   = 0xA0,
  A7105_PLL       = 0xB0,
  A7105_RX        = 0xC0,
  A7105_TX        = 0xD0,
  A7105_RST_WRPTR = 0xE0,
  A7105_RST_RDPTR = 0xF0,
};

enum {
  A7105_00_MODE         = 0x00,
  A7105_01_MODE_CONTROL = 0x01,
  A7105_02_CALC         = 0x02,
  A7105_03_FIFOI        = 0x03,
  A7105_04_FIFOII       = 0x04,
  A7105_05_FIFO_DATA    = 0x05,
  A7105_06_ID_DATA      = 0x06,
  A7105_07_RC_OSC_I     = 0x07,
  A7105_08_RC_OSC_II    = 0x08,
  A7105_09_RC_OSC_III   = 0x09,
  A7105_0A_CK0_PIN      = 0x0A,
  A7105_0B_GPIO1_PIN1   = 0x0B,
  A7105_0C_GPIO2_PIN_II = 0x0C,
  A7105_0D_CLOCK        = 0x0D,
  A7105_0E_DATA_RATE    = 0x0E,
  A7105_0F_PLL_I        = 0x0F,
  A7105_10_PLL_II       = 0x10,
  A7105_11_PLL_III      = 0x11,
  A7105_12_PLL_IV       = 0x12,
  A7105_13_PLL_V        = 0x13,
  A7105_14_TX_I         = 0x14,
  A7105_15_TX_II        = 0x15,
  A7105_16_DELAY_I      = 0x16,
  A7105_17_DELAY_II     = 0x17,
  A7105_18_RX           = 0x18,
  A7105_19_RX_GAIN_I    = 0x19,
  A7105_1A_RX_GAIN_II   = 0x1A,
  A7105_1B_RX_GAIN_III  = 0x1B,
  A7105_1C_RX_GAIN_IV   = 0x1C,
  A7105_1D_RSSI_THOLD   = 0x1D,
  A7105_1E_ADC          = 0x1E,
  A7105_1F_CODE_I       = 0x1F,
  A7105_20_CODE_II      = 0x20,
  A7105_21_CODE_III     = 0x21,
  A7105_22_IF_CALIB_I   = 0x22,
  A7105_23_IF_CALIB_II  = 0x23,
  A7105_24_VCO_CURCAL   = 0x24,
  A7105_25_VCO_SBCAL_I  = 0x25,
  A7105_26_VCO_SBCAL_II = 0x26,
  A7105_27_BATTERY_DET  = 0x27,
  A7105_28_TX_TEST      = 0x28,
  A7105_29_RX_DEM_TEST_I  = 0x29,
  A7105_2A_RX_DEM_TEST_II = 0x2A,
  A7105_2B_CPC          = 0x2B,
  A7105_2C_XTAL_TEST    = 0x2C,
  A7105_2D_PLL_TEST     = 0x2D,
  A7105_2E_VCO_TEST_I   = 0x2E,
  A7105_2F_VCO_TEST_II  = 0x2F,
  A7105_30_IFAT         = 0x30,
  A7105_31_RSCALE       = 0x31,
  A7105_32_FILTER_TEST  = 0x32,
};
#define A7105_0F_CHANNEL A7105_0F_PLL_I

enum A7105_MASK
{
  A7105_MASK_FBCF = 1 << 4,
  A7105_MASK_VBCF = 1 << 3,
};

void A7105_Initialize();
void A7105_WriteReg(uint8_t addr, uint8_t value);
void A7105_WriteData(uint8_t *dpbuffer, uint8_t len, uint8_t channel);
void A7105_ReadData(uint8_t *dpbuffer, uint8_t len);
uint8_t A7105_ReadReg(uint8_t addr);
void A7105_Reset();
void A7105_WriteID(uint32_t id);
void A7105_Strobe(enum A7105_State);
void A7105_SetPower(int power);


#endif


//Register values:
static const uint8_t A7105_regs[] =
{
  0xFF, // MODE_REG           0x00
  0x62, // MODECTRL_REG       0x01
  0xFF, // CALIBRATION_REG    0x02
  PAYLOAD_SIZE - 1, // FIFO1_REG          0x03  0x01
  0x00, // FIFO2_REG          0x04
  0x00, // FIFO_REG           0x05
  0x00, // IDCODE_REG         0x06
  0x00, // RCOSC1_REG         0x07
  0x00, // RCOSC2_REG         0x08
  0x00, // RCOSC3_REG         0x09
  0x00, // CKO_REG            0x0A
  0x21, // GPIO1_REG          0x0B
  0x01, // GPIO2_REG          0x0C
  0x05, // CLOCK_REG          0x0D
  0x00, // DATARATE_REG       0x0E
  0x50, // PLL1_REG           0x0F
  0x9E, // PLL2_REG           0x10
  0x4B, // PLL3_REG           0x11
  0x00, // PLL4_REG           0x12
  0x02, // PLL5_REG           0x13
  0x16, // TX1_REG            0x14
  0x2B, // TX2_REG            0x15
  0x12, // DELAY1_REG         0x16
  0x00, // DELAY2_REG         0x17
  0x62, // RX_REG             0x18
  0x80, // RXGAIN1_REG        0x19
  0x80, // RXGAIN2_REG        0x1A
  0x00, // RXGAIN3_REG        0x1B
  0x0A, // RXGAIN4_REG        0x1C
  0x32, // RSSI_REG           0x1D
  0xC3, // ADC_REG            0x1E
  0x07, // CODE1_REG          0x1F
  0x16, // CODE2_REG          0x20
  0x00, // CODE3_REG          0x21
  0x00, // IFCAL1_REG         0x22
  0x00, // IFCAL2_REG         0x23
  0x00, // VCOCCAL_REG        0x24
  0x00, // VCOCAL1_REG        0x25
  0x3B, // VCOCAL2_REG        0x26
  0x00, // BATTERY_REG        0x27
  0x17, // TXTEST_REG         0x28
  0x47, // RXDEM1_REG         0x29
  0x80, // RXDEM2_REG         0x2A
  0x03, // CPC_REG            0x2B
  0x01, // CRYSTALTEST_REG    0x2C
  0x45, // PLLTEST_REG        0x2D
  0x18, // VCOTEST1_REG       0x2E
  0x00, // VCOTEST2_REG       0x2F
  0x01, // IFAT_REG           0x30
  0x0F, // RSCALE_REG         0x31
  0x00, // FILTERTEST_REG     0x32
};




#define NOP() __asm__ __volatile__("nop")
static uint8_t channel;
static byte counter = 255;
static uint8_t aid[4];

void setup()
{
  Serial.begin(38400);


  //pin modes
  pinMode(12, INPUT); //this pin is not used. just setting the mode for the sake of SPI
  pinMode(SDI_pin, OUTPUT);
  pinMode(SCLK_pin, OUTPUT);
  pinMode(CS_pin, OUTPUT);
  digitalWrite(CS_pin, HIGH);
  digitalWrite(SDI_pin, HIGH);
  digitalWrite(SCLK_pin, LOW);

  delay(10);
  A7105_reset();//reset A7105
  delay(100);
  Serial.print("ID: ");
  A7105_WriteID(&id[0]);
  A7105_ReadID();         // Reads the ID set in module
  A7105_DumpRegs();       // shows values of all registers on serial monitor
  A7105_Calibrate();

  // set read mode
  _spi_write_adress(0x0F, channel_1);
  _spi_strobe(A7105_STANDBY);
  _spi_strobe(A7105_RST_RDPTR);
  _spi_strobe(A7105_RX);

}

uint8_t in[PAYLOAD_SIZE];
uint8_t out[PAYLOAD_SIZE];
boolean rx_tx = false;
int h = 0;

void loop()
{

  int len;
  char cmd;

  if (rx_tx == false)   //RX data - if data comming
  {
    rx_tx = true;
    if (digitalRead(GIO_pin) == LOW)      //check if A7105 has sent any RX intterrupt
    {
      len = PAYLOAD_SIZE;
      A7105_ReadData(len, &in[0]);        //Arduino reads data from RX buffer of A7105
      // set read mode
      _spi_write_adress(0x0F, channel_1); //Set to Rx channel frequency
      _spi_strobe(A7105_STANDBY);
      _spi_strobe(A7105_RST_RDPTR);
      _spi_strobe(A7105_RX);              //Set to Rx mode
    }
  }
  else                  //Tx data if any data to send
  {
    rx_tx = false;

    //Read data from Serial input
    if (Serial.available())
    {
      h = 0;
      unsigned long delayer = 0;
      while ((Serial.available()) || (delayer < 200))
      {
        delayer++;
        char cd = Serial.read();
        Serial.write(cd);
        if ( (cd == '\r') || (cd == '\n') )
        {
          Serial.flush();
          break;
        }
        else
        {
          out[h] = cd;
          h++;
          if (h >= PAYLOAD_SIZE)
          {
            break;
          }
        }
        delay(1); //delay to wait for incomming incomplete string from serial
      }
      tx_data();    //start data transmission
    }


    //OR
    //to send programmed data:
    //out[0] = 0x01;
    //out[1] = 0x02;
    //out[2] = 0x03;
    //out[3] = 0x04;      //[length] should be less than PAYLOAD_SIZE
    //tx_data();
  }
}











//------------------------------------------------High level functions

void tx_data()
{
  int i, c;
  int len;
  for (int y = h; y < PAYLOAD_SIZE; y++)  //if 'out' buffer is not full then fill the rest by 0x00
  {
    out[y] = 0x00;                        //out is the buffer that holds the tx data before transmission
  }
  boolean empty = true;
  for (int h = 0; h < PAYLOAD_SIZE; h++)  //check if 'out' is totally empty
  {
    if (out[h] != 0x00)
    {
      empty = false;
    }
  }

  if (empty == false)     //if out is not totally empty then send data to A7105
  {
    channel = channel_2;
    len = PAYLOAD_SIZE;
    Serial.print("\r\nCH: ");
    Serial.println(channel, HEX);
    for (c = 0; c < Tx_count; c++)
    {
      //Tx mode
      _spi_strobe(A7105_STANDBY);
      _spi_strobe(A7105_RST_WRPTR);
      _spi_write_adress(0x0F, channel);   //Sets the frequency channel for TX
      A7105_WriteData(len, &out[0]);      //data is loaded into Tx buffer of A7105
      _spi_strobe(A7105_TX);              //A7105 actually transmits data from its buffer
      delay(2);
      // set read mode
      _spi_write_adress(0x0F, channel_1); //Sets the frequency channel for RX
      _spi_strobe(A7105_STANDBY);
      _spi_strobe(A7105_RST_RDPTR);
      _spi_strobe(A7105_RX);              //Set A7105 to Rx mode - It will send arduino an interrupt when any data is recived
      delay(18);
    }
  }
  for (int y = 0; y < PAYLOAD_SIZE; y++)
  {
    out[y] = 0x00;        //empty the out buffer
  }
}







void A7105_ReadData(int len, uint8_t * data)
{
  uint8_t i;
  digitalWrite(CS_pin, LOW);
  _spi_write(0x45);
  Serial.print("Rx <");
  for (i = 0; i < len; i++)
  {
    *data = _spi_read();
    if (output_format == 1)
    {
      char c = *data;
      if (c != 0x00)
        Serial.print(c);
    }
    else
    {
      Serial.print(*data, HEX);
      Serial.print(" ");
    }
    data++;
  }
  Serial.println("");
  digitalWrite(CS_pin, HIGH);;
}


void A7105_WriteData(int len, uint8_t * data)
{
  int i;
  digitalWrite(CS_pin, LOW);
  _spi_write(0x05);
  Serial.print("Tx >");
  for (i = 0; i < len; i++)
  {
    _spi_write(*data);
    Serial.print(*data, HEX);
    Serial.print(" ");
    data++;
  }
  Serial.println("");
  digitalWrite(CS_pin, HIGH);
}














//------------------------------------------------Small level functions

void A7105_reset(void)
{
  _spi_write_adress(0x00, 0x00);
}

void A7105_WriteID(uint8_t *ida)
{
  int i;
  digitalWrite(CS_pin, LOW);
  _spi_write(0x06);
  for (i = 0; i < 4; i++)
  {
    _spi_write(*ida);
    ida++;
  }
  digitalWrite(CS_pin, HIGH);;
}

void A7105_ReadID()
{
  int i;
  digitalWrite(CS_pin, LOW);
  _spi_write(0x46);
  for (i = 0; i < 4; i++)
  {
    aid[i] = _spi_read();
    Serial.print(aid[i], HEX);
  }
  Serial.println("");
  digitalWrite(CS_pin, HIGH);;
}

void A7105_DumpRegs()
{
  int i;
  uint8_t r;
  for (i = 0; i < 0x33; i++)
  {

    if (A7105_regs[i] != 0xff)
      _spi_write_adress(i, A7105_regs[i]);

    r = _spi_read_adress(i);
    Serial.print(i, HEX);
    Serial.print('\t');
    Serial.print(r, BIN);
    Serial.println("");
  }
  Serial.println("");
}

void A7105_Calibrate()
{
  uint8_t if_calibration1;
  uint8_t vco_calibration0;
  uint8_t vco_calibration1;

  _spi_write_adress(0x0F, channel_1);
  _spi_strobe(A7105_PLL);  //PLL mode

  //IF filter Bank Calibration
  _spi_write_adress(0x02, 0x01);   //Enable FBC
  while (_spi_read_adress(0x02))
  {
    if_calibration1 = _spi_read_adress(0x22);
    if (if_calibration1 & 0x10)
    {
      Serial.println("IF Calibration Failed.");
    }
    else
    {
      Serial.println("IF Calibration Done.");
    }
  }

  //VCO current Calibration
  _spi_write_adress(0x02, 0x02);  //Enable VCC
  _spi_write_adress(0x24, 0x13);  //manual calibration, VCO: 011
  _spi_write_adress(0x24, 0x13);


  //VCO Bank Calibration
  _spi_write_adress(0x02, 0x04);  //Enable VBC
  delay(10);
  _spi_write_adress(0x26, 0x3b);  //recommended value of VTH and VTL
  while (_spi_read_adress(0x02))
  {
    vco_calibration0 = _spi_read_adress(0x25);
    if (vco_calibration0 & 0x08)
    {
      Serial.println("VCO Bank Calibration Failed.");
    }
    else
    {
      Serial.println("VCO Bank Calibrated.");
    }
  }


  //_spi_write_adress(0x25, 0x08);
  _spi_write_adress(0x28, 0x1F); //set power to 1db maximum
  _spi_strobe(0xA0);            //stand-by strobe command
  delay(100);



  _spi_strobe(A7105_RST_WRPTR);
  _spi_write_adress(0x0F, channel_1);
  _spi_strobe(A7105_STANDBY);
  _spi_strobe(A7105_RST_RDPTR);
  _spi_strobe(A7105_RX);
}


void _spi_write(uint8_t command)
{
  uint8_t n = 8;
  digitalWrite(SCLK_pin, LOW);
  digitalWrite(SDI_pin, LOW);
  while (n--)
  {
    if (command & 0x80)
      digitalWrite(SDI_pin, HIGH);
    else
      digitalWrite(SDI_pin, LOW);
    digitalWrite(SCLK_pin, HIGH);
    NOP();
    digitalWrite(SCLK_pin, LOW);
    command = command << 1;
  }
  digitalWrite(SDI_pin, HIGH);
}

void _spi_write_adress(uint8_t address, uint8_t data)
{
  digitalWrite(CS_pin, LOW);
  _spi_write(address);
  NOP();
  _spi_write(data);
  digitalWrite(CS_pin, HIGH);
}



uint8_t _spi_read(void)
{
  uint8_t result;
  uint8_t i;
  result = 0;
  pinMode(SDI_pin, INPUT); //make SDIO pin input
  //digitalWrite(SDI_pin, HIGH);
  for (i = 0; i < 8; i++)
  {
    if (digitalRead(SDI_pin) == HIGH) ///if SDIO =1
      result = (result << 1) | 0x01;
    else
      result = result << 1;
    digitalWrite(SCLK_pin, HIGH);
    NOP();
    digitalWrite(SCLK_pin, LOW);
    NOP();
  }
  pinMode(SDI_pin, OUTPUT); //make SDIO pin output again
  return result;
}

uint8_t _spi_read_adress(uint8_t address)
{
  uint8_t result;
  digitalWrite(CS_pin, LOW);
  address |= 0x40;
  _spi_write(address);
  result = _spi_read();
  digitalWrite(CS_pin, HIGH);
  return (result);
}

void _spi_strobe(uint8_t address)
{
  digitalWrite(CS_pin, LOW);
  _spi_write(address);
  digitalWrite(CS_pin, HIGH);
}
