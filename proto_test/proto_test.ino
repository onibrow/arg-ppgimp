#include <SPI.h>

const uint8_t CS_10 = 17;
const uint8_t CS_1  = 19;

char buff[3];
char serbuff[10];

bool DEBUG = true;

void setup() {
  Serial.begin(115200);

  if (DEBUG) {
    Serial.println("Initializing PPG IMP Prototype board...\n");
  }

  if (DEBUG) {
    Serial.println("--- Starting SPI --- ");
  }
  // start the SPI library:
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV32);
  // initialize the  data ready and chip select pins:
  pinMode(CS_10, OUTPUT);
  pinMode(CS_1,  OUTPUT);
  digitalWrite(CS_10, HIGH);
  digitalWrite(CS_1,  HIGH);

  writeRegister(CS_1, 0);
  writeRegister(CS_10, 0);
}

void loop() {
  clearBuffers();
  int i = 0;
  while (Serial.available()) {
    char c = (char) Serial.read();
    if (c != '\r' & c != '\n') {
      buff[i] = c;
    }
    i = (i + 1) % 3;
  }
  if (buff[0] != 0 | buff[1] != 0 | buff[2] != 0) {
    int rec;
    sscanf(buff, "%d", &rec);
    uint8_t rec_char = (uint8_t) rec;
    writeRegister(CS_1, rec);
    writeRegister(CS_10, rec);
  }
}

void clearBuffers() {
  memset(buff, 0, sizeof buff);
  memset(serbuff, 0, sizeof serbuff);
}

// Write to AD8400
void writeRegister(uint8_t cs, uint8_t val) {
  /* Addr: [9:8]
     Data: [7:0]

     For single Channel AD8400, Addr is 0b00 */
  uint8_t addr = 0;
  uint8_t data = val;
  if (DEBUG) {
    Serial.print("Writing 0x");
    Serial.print(addr, HEX);
    Serial.println(val, HEX);
  }
  // transfer payload
  digitalWrite(cs, LOW);
  SPI.transfer(addr);
  SPI.transfer(data);
  digitalWrite(cs, HIGH);
}
