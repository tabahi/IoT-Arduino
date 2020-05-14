
//Spi Comm.pins with A7105/PPM
#define SDI_pin 11 //SDIO-D11 
#define SCLK_pin 13 //SCK-D13
#define CS_pin 10//CS-D10
#define GIO_pin 2

#define RX_Channel 0x71
#define TX_Channel 0x71

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

enum A7105_MASK {
  A7105_MASK_FBCF = 1 << 4,
  A7105_MASK_VBCF = 1 << 3,
};

void A7105_Initialize();
void A7105_WriteReg(u8 addr, u8 value);
void A7105_WriteData(u8 *dpbuffer, u8 len, u8 channel);
void A7105_ReadData(u8 *dpbuffer, u8 len);
u8 A7105_ReadReg(u8 addr);
void A7105_Reset();
void A7105_WriteID(u32 id);
void A7105_Strobe(enum A7105_State);
void A7105_SetPower(int power);


#endif


#define PAYLOAD_SIZ 6
uint8_t id[4] = { 0x35, 0x99, 0x9A, 0x5A };
//  uint8_t id[4] = { 0x5A, 0x9A, 0x99, 0x35 };

static const uint8_t A7105_regs[] = {
  0xFF, // MODE_REG           0x00
  0x42, // MODECTRL_REG       0x01
  0xFF, // CALIBRATION_REG    0x02
  PAYLOAD_SIZ - 1, // FIFO1_REG          0x03  0x01
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

//########## Variables #################
static uint8_t channel_tx;
static uint8_t channel_rx;
static byte counter = 255;
static uint8_t aid[4];//for debug only

void setup()
{
  Serial.begin(38400);
  //RF module pins
  pinMode(12, INPUT);
  pinMode(SDI_pin, OUTPUT);//SDI   SDIO
  pinMode(SCLK_pin, OUTPUT);//SCLK SCL
  pinMode(CS_pin, OUTPUT);//CS output
  digitalWrite(CS_pin, HIGH);//start CS high
  digitalWrite(SDI_pin, HIGH);
  digitalWrite(SCLK_pin, LOW);
  //digitalWrite(SDI_pin, HIGH);//start SDIO high
  //digitalWrite(SCLK_pin, LOW);//start sck low

  //
  //for debug
  delay(10);//wait 10ms for A7105 wakeup
  A7105_reset();//reset A7105
  delay(100);
  Serial.print("ID: ");
  A7105_WriteID(&id[0]);
  A7105_ReadID();//for debug only
  A7105_DumpRegs();   // Dump registers
  A7105_Calibrate();  // calibrate A7105

  // set read mode
  _spi_write_adress(0x0F, channel_rx);
  _spi_strobe(A7105_STANDBY);
  _spi_strobe(A7105_RST_RDPTR);
  _spi_strobe(A7105_RX);

}

uint8_t in[PAYLOAD_SIZ];
uint8_t out[PAYLOAD_SIZ];

boolean rx_tx = false;

void loop()
{
  int i, c;
  int len;
  char cmd;

  if (rx_tx == false)
  {
    rx_tx = true;
    if (digitalRead(GIO_pin) == LOW)
    {
      len = PAYLOAD_SIZ;
      A7105_ReadData(len, &in[0]);
      // set read mode
      _spi_write_adress(0x0F, channel_rx);
      _spi_strobe(A7105_STANDBY);
      _spi_strobe(A7105_RST_RDPTR);
      _spi_strobe(A7105_RX);
      //    delay(10);
    }
  }
  else
  {
    rx_tx = false;
    if (Serial.available())
    {
      int h = 0;
      while (Serial.available())
      {
        char cd = Serial.read();
        Serial.write(cd);

        if ( (cd == '\r') || (cd == '\n') )
        {
          Serial.flush();
          while (h < PAYLOAD_SIZ)
          {
            out[h] = 0x00;
            h++;
          }
          break;
        }
        else
        {
          out[h] = cd;
          h++;
          if (h >= PAYLOAD_SIZ)
          {
            break;
          }
        }
      }

      boolean empty = true;
      for (int h = 0; h < PAYLOAD_SIZ; h++)
      {
        if (out[h] != 0x00)
        {
          empty = false;
        }
      }

      if (empty == false)
      {
        channel_tx = TX_Channel;
        channel_rx = RX_Channel;
        len = PAYLOAD_SIZ;
        Serial.print("\r\nchan: ");
        Serial.println(channel_tx, HEX);
        for (c = 0; c < 16; c++)
        {
          _spi_strobe(A7105_STANDBY);
          _spi_strobe(A7105_RST_WRPTR);
          _spi_write_adress(0x0F, channel_tx);
          A7105_WriteData(len, &out[0]);
          _spi_strobe(A7105_TX);
          delay(2);
          // set read mode
          _spi_write_adress(0x0F, channel_rx);
          _spi_strobe(A7105_STANDBY);
          _spi_strobe(A7105_RST_RDPTR);
          _spi_strobe(A7105_RX);
          delay(18);
        }
      }

    }
  }
}

//-------------------------------
//-------------------------------
//A7105 SPI routines
//-------------------------------
//-------------------------------

//--------------------------------------
void A7105_reset(void) {
  _spi_write_adress(0x00, 0x00);
}

//--------------------------------------
void A7105_WriteID(uint8_t *ida) {
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

//--------------------------------------
void A7105_ReadID() {
  int i;
  digitalWrite(CS_pin, LOW);
  _spi_write(0x46);
  for (i = 0; i < 4; i++) {
    aid[i] = _spi_read();
    Serial.print(aid[i], HEX);
  }
  Serial.println("");
  digitalWrite(CS_pin, HIGH);;
}

//--------------------------------------
void A7105_DumpRegs() {
  int i;
  uint8_t r;
  for (i = 0; i < 0x33; i++)
  {

    if (A7105_regs[i] != 0xff)
      _spi_write_adress(i, A7105_regs[i]);

    r = _spi_read_adress(i);
    Serial.print(i, HEX);
    Serial.print('\t');
    //Serial.print(A7105_regs[i], BIN);
    //Serial.print('\t');
    Serial.print(r, BIN);
    Serial.println("");
  }
  Serial.println("");
}

//--------------------------------------
void A7105_Calibrate() {
  uint8_t if_calibration1;
  uint8_t vco_calibration0;
  uint8_t vco_calibration1;

  _spi_strobe(0xA0);//stand-by
  _spi_write_adress(0x02, 0x01);
  while (_spi_read_adress(0x02)) {
    if_calibration1 = _spi_read_adress(0x22);
    if (if_calibration1 & 0x10) { //do nothing
    }
  }
  _spi_write_adress(0x24, 0x13);
  _spi_write_adress(0x26, 0x3b);
  _spi_write_adress(0x0F, 0x00); //channel 0
  _spi_write_adress(0x02, 0x02);
  while (_spi_read_adress(0x02)) {
    vco_calibration0 = _spi_read_adress(0x25);
    if (vco_calibration0 & 0x08) { //do nothing
    }
  }
  _spi_write_adress(0x0F, 0xA0);
  _spi_write_adress(0x02, 0x02);
  while (_spi_read_adress(0x02)) {
    vco_calibration1 = _spi_read_adress(0x25);
    if (vco_calibration1 & 0x08) { //do nothing
    }
  }
  _spi_write_adress(0x25, 0x08);
  _spi_write_adress(0x28, 0x1F); //set power to 1db maximum
  _spi_strobe(0xA0);//stand-by strobe command
  delay(100);
  _spi_strobe(A7105_RST_WRPTR);
  _spi_write_adress(0x0F, channel_rx);
  _spi_strobe(A7105_STANDBY);
  _spi_strobe(A7105_RST_RDPTR);
  _spi_strobe(A7105_RX);
}

//--------------------------------------
void A7105_ReadData(int len, uint8_t * data) {
  uint8_t i;
  digitalWrite(CS_pin, LOW);
  _spi_write(0x45);
  Serial.print("Rx <");
  for (i = 0; i < len; i++) {
    *data = _spi_read();
    Serial.print(*data, HEX);
    Serial.print(" ");
    data++;
  }
  Serial.println("");
  digitalWrite(CS_pin, HIGH);;
}

//--------------------------------------
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

//--------------------------------------
void _spi_write(uint8_t command) {
  uint8_t n = 8;
  digitalWrite(SCLK_pin, LOW);
  digitalWrite(SDI_pin, LOW);
  while (n--) {
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
//--------------------------------------
void _spi_write_adress(uint8_t address, uint8_t data) {
  digitalWrite(CS_pin, LOW);
  _spi_write(address);
  NOP();
  _spi_write(data);
  digitalWrite(CS_pin, HIGH);
}
//-----------------------------------------
uint8_t _spi_read(void) {
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
//--------------------------------------------
uint8_t _spi_read_adress(uint8_t address) {
  uint8_t result;
  digitalWrite(CS_pin, LOW);
  address |= 0x40;
  _spi_write(address);
  result = _spi_read();
  digitalWrite(CS_pin, HIGH);
  return (result);
}
//------------------------
void _spi_strobe(uint8_t address) {
  digitalWrite(CS_pin, LOW);
  _spi_write(address);
  digitalWrite(CS_pin, HIGH);
}
