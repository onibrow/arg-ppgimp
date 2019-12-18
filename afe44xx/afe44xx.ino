//////////////////////////////////////////////////////////////////////////////////////////
//
//   AFE44xx Arduino Firmware
//
//   Copyright (c) 2016 ProtoCentral
//
//   SpO2 computation based on original code from Maxim Integrated
//
//   This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//   For information on how to use the HealthyPi, visit https://github.com/Protocentral/afe44xx_Oximeter
/////////////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <SPI.h>
#include <math.h>

//afe44xx Register definition
#define CONTROL0    0x00
#define LED2STC     0x01
#define LED2ENDC    0x02
#define LED2LEDSTC    0x03
#define LED2LEDENDC   0x04
#define ALED2STC    0x05
#define ALED2ENDC   0x06
#define LED1STC     0x07
#define LED1ENDC    0x08
#define LED1LEDSTC    0x09
#define LED1LEDENDC   0x0a
#define ALED1STC    0x0b
#define ALED1ENDC   0x0c
#define LED2CONVST    0x0d
#define LED2CONVEND   0x0e
#define ALED2CONVST   0x0f
#define ALED2CONVEND  0x10
#define LED1CONVST    0x11
#define LED1CONVEND   0x12
#define ALED1CONVST   0x13
#define ALED1CONVEND  0x14
#define ADCRSTCNT0    0x15
#define ADCRSTENDCT0  0x16
#define ADCRSTCNT1    0x17
#define ADCRSTENDCT1  0x18
#define ADCRSTCNT2    0x19
#define ADCRSTENDCT2  0x1a
#define ADCRSTCNT3    0x1b
#define ADCRSTENDCT3  0x1c
#define PRPCOUNT    0x1d
#define CONTROL1    0x1e
#define SPARE1      0x1f
#define TIAGAIN     0x20
#define TIA_AMB_GAIN  0x21
#define LEDCNTRL    0x22
#define CONTROL2    0x23
#define SPARE2      0x24
#define SPARE3      0x25
#define SPARE4      0x26
#define SPARE4      0x26
#define RESERVED1   0x27
#define RESERVED2   0x28
#define ALARM     0x29
#define LED2VAL     0x2a
#define ALED2VAL    0x2b
#define LED1VAL     0x2c
#define ALED1VAL    0x2d
#define LED2ABSVAL    0x2e
#define LED1ABSVAL    0x2f
#define DIAG      0x30
#define count 60

const int SPISTE = 4; // 10 for UNO, 4 for MIPPE32U4 // chip select

void afe44xxInit (void);
void afe44xxWrite (uint8_t address, uint32_t data);
uint32_t afe44xxRead (uint8_t address);

unsigned long IRval, AIRval, REDval, AREDval, IRabsval, REDabsval;
volatile int i;

#define FS            25    //sampling frequency
#define BUFFER_SIZE  (FS*4)
#define MA4_SIZE  4 // DONOT CHANGE
#define min(x,y) ((x) < (y) ? (x) : (y))

uint32_t CONTROL0_VAL = 0x00000000;

void setup()
{

  Serial.begin(57600);
  while (!Serial){};
  
  Serial.println("Intilazition AFE44xx.. ");

  delay(500) ;   // pause for a moment

  SPI.begin();

  // set the directions
  pinMode (SPISTE, OUTPUT); //Slave Select

  // set SPI transmission
  SPI.setClockDivider (SPI_CLOCK_DIV8); // set Speed as 2MHz , 16MHz/ClockDiv
  SPI.setDataMode (SPI_MODE0);          //Set SPI mode as 0
  SPI.setBitOrder (MSBFIRST);           //MSB first

  afe44xxInit ();
  delay(3000);
}

void loop() {
  IRval = afe44xxReadData(LED2VAL);
  AIRval = afe44xxReadData(ALED2VAL);
  IRabsval = afe44xxReadData(LED2ABSVAL);
  REDval = afe44xxReadData(LED1VAL);
  AREDval = afe44xxReadData(ALED1VAL);
  REDabsval = afe44xxReadData(LED1ABSVAL);
  Serial.print(REDval);
  Serial.print(" ");
  Serial.print(AREDval);
  Serial.print(" ");
  Serial.print(REDabsval);
  Serial.print(" ");
  Serial.print(IRval);
  Serial.print(" ");
  Serial.print(AIRval);
  Serial.print(" ");
  Serial.println(IRabsval);
  //  delay(10);
}

////////////////AFE44xx initialization//////////////////////////////////////////
void afe44xxInit (void)
{
  Serial.println("afe44xx Initialization Start");
  CONTROL0_VAL = 0x00000008;
  afe44xxWrite(CONTROL0, CONTROL0_VAL);
  delay(10);
  CONTROL0_VAL = 0x000000;
  afe44xxWrite(CONTROL0, CONTROL0_VAL);
  delay(10);

  // Gain Stages
  afe44xxWrite(TIAGAIN, 0x000000); // CF = 5pF, RF = 500kR
  afe44xxWrite(TIA_AMB_GAIN, 0x000001);

  // Page 36 Table 2
  afe44xxWrite(LED2STC, 6050);
  afe44xxWrite(LED2ENDC, 7998);
  afe44xxWrite(LED2LEDSTC, 6000);
  afe44xxWrite(LED2LEDENDC, 7999);
  afe44xxWrite(ALED2STC, 50);
  afe44xxWrite(ALED2ENDC, 1998);

  afe44xxWrite(LED1STC, 2050);
  afe44xxWrite(LED1ENDC, 3998);
  afe44xxWrite(LED1LEDSTC, 2000);
  afe44xxWrite(LED1LEDENDC, 3999);
  afe44xxWrite(ALED1STC, 4050);
  afe44xxWrite(ALED1ENDC, 5998);

  afe44xxWrite(LED2CONVST, 4);
  afe44xxWrite(LED2CONVEND, 1999);
  afe44xxWrite(ALED2CONVST, 2004);
  afe44xxWrite(ALED2CONVEND, 3999);

  afe44xxWrite(LED1CONVST, 4004);
  afe44xxWrite(LED1CONVEND, 5999);
  afe44xxWrite(ALED1CONVST, 6004);
  afe44xxWrite(ALED1CONVEND, 7999);

  afe44xxWrite(ADCRSTCNT0, 0);
  afe44xxWrite(ADCRSTENDCT0, 3);
  afe44xxWrite(ADCRSTCNT1, 2000);
  afe44xxWrite(ADCRSTENDCT1, 2003);
  afe44xxWrite(ADCRSTCNT2, 4000);
  afe44xxWrite(ADCRSTENDCT2, 4003);
  afe44xxWrite(ADCRSTCNT3, 6000);
  afe44xxWrite(ADCRSTENDCT3, 6003);
  afe44xxWrite(PRPCOUNT, 7999);

  // Start Sampling
  afe44xxWrite(CONTROL1, 0x000101); // Timers ON, average 1 samples
  Serial.print("CONTROL1: 0x");
  Serial.println(afe44xxRead(CONTROL1), HEX);

  //  afe44xxWrite(LEDCNTRL, 0x000606);
  //    afe44xxWrite(LEDCNTRL, 0x000000);
  afe44xxWrite(LEDCNTRL, 0x001414);
  Serial.print("LEDCNTRL: 0x");
  Serial.println(afe44xxRead(LEDCNTRL), HEX);

  afe44xxWrite(CONTROL2, 0x060A00);
  Serial.print("CONTROL2: 0x");
  Serial.println(afe44xxRead(CONTROL2), HEX);

  delay(1000);
  Serial.println("afe44xx Initialization Done");
  afe44xxWrite(CONTROL0, CONTROL0_VAL | 0x1);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void afe44xxWrite (uint8_t address, uint32_t data)
{
  digitalWrite (SPISTE, LOW); // enable device
  SPI.transfer (address); // send address to device
  SPI.transfer ((data >> 16) & 0xFF); // write top 8 bits
  SPI.transfer ((data >> 8) & 0xFF); // write middle 8 bits
  SPI.transfer (data & 0xFF); // write bottom 8 bits
  digitalWrite (SPISTE, HIGH); // disable device
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t afe44xxRead (uint8_t address)
{
  afe44xxWrite(CONTROL0, CONTROL0_VAL | 0x1);
  uint32_t data = 0;
  digitalWrite (SPISTE, LOW); // enable device
  SPI.transfer (address); // send address to device
  data |= ((uint32_t)SPI.transfer (0) << 16); // read top 8 bits data
  data |= ((uint32_t)SPI.transfer (0) << 8); // read middle 8 bits  data
  data |= SPI.transfer (0); // read bottom 8 bits data
  digitalWrite (SPISTE, HIGH); // disable device
  afe44xxWrite(CONTROL0, CONTROL0_VAL & ~(0x1));

  return data; // return with 24 bits of read data
}

uint32_t afe44xxReadData(uint8_t address)
{
  uint32_t data = 0;
  digitalWrite (SPISTE, LOW); // enable device
  SPI.transfer (address); // send address to device
  data |= ((uint32_t)SPI.transfer (0) << 16); // read top 8 bits data
  data |= ((uint32_t)SPI.transfer (0) << 8); // read middle 8 bits  data
  data |= SPI.transfer (0); // read bottom 8 bits data
  digitalWrite (SPISTE, HIGH); // disable device
  return data;
}
