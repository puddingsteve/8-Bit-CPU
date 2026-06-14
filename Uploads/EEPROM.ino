/*

Modified from https://github.com/beneater/eeprom-programmer?tab=readme-ov-file

*/
#define SHIFT_DATA 2
#define SHIFT_CLK 3
#define ST_CLK 4
#define IO_0 5
#define IO_7 12
#define WRITE_EN 13

#define HLT 0b1000000000000000  // Halt clock
#define PCI 0b0100000000000000  // Jump (program counter in)
#define PCO 0b0010000000000000  // Program counter out
#define PCE 0b0001000000000000  // Program counter enable
#define MI  0b0000100000000000  // Memory address register in
#define RI  0b0000010000000000  // RAM data in
#define RO  0b0000001000000000  // RAM data out
#define II  0b0000000100000000  // Instruction register in
#define IO  0b0000000010000000  // Instruction register out
#define OI  0b0000000001000000  // Output register in
#define SO  0b0000000000100000  // ALU out
#define SUB 0b0000000000010000  // ALU subtract
#define AI  0b0000000000001000  // A register in
#define AO  0b0000000000000100  // A register out
#define BI  0b0000000000000010  // B register in
#define FI  0b0000000000000001  // Flag Register In

/*

# NOP
Address  Op    c       c
   0     0000  000 -> MI|PCO 
   1     0000  001 -> RO|II 
   2     0000  010 -> PCE 
   3     0000  011 -> 0 
   4     0000  100 -> 0 
   5     0000  101 -> 0          
   6     0000  110 -> 0 
   7     0000  111 -> 0
#LDA: 
   8     0001  000 -> MI|PCO 
   9     0001  001 -> RO|II 
  10     0001  010 -> PCE 
  11     0001  011 -> IO|MI 
  12     0001  100 -> RO|AI 
  13     0001  101 -> 0          
  14     0001  110 -> 0 
  15     0001  111 -> 0
*/

uint16_t flag_0_data[] = {
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 0000 - NOP
  MI|PCO,  RO|II, PCE,  IO|MI,    RO|AI,  0,             0, 0,   // 0001 - LDA
  MI|PCO,  RO|II, PCE,  IO|MI,    RO|BI,  SO|AI,         0, 0,   // 0010 - ADD
  MI|PCO,  RO|II, PCE,  IO|MI,    RO|BI,  SO|SUB|FI|AI,  0, 0,   // 0011 - SUB
  MI|PCO,  RO|II, PCE,  IO|MI,    AO|RI,  0,             0, 0,   // 0100 - STA
  MI|PCO,  RO|II, PCE,  IO|AI,    0,      0,             0, 0,   // 0101 - LDI
  MI|PCO,  RO|II, PCE,  IO|PCI,   0,      0,             0, 0,   // 0110 - JMP
  MI|PCO,  RO|II, PCE,  IO|PCI,   0,      0,             0, 0,   // 0111 - JNZ
  MI|PCO,  RO|II, PCE,  IO|BI,    SO|SUB|FI|AI, 0,       0, 0,   // 1000 - SUBAI
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1001
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1010
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1011
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1100
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1101
  MI|PCO,  RO|II, PCE,  AO|OI,    0,      0,             0, 0,   // 1110 - OUT
  MI|PCO,  RO|II, HLT,  0,      0,      0,             0, 0,   // 1111 - HLT
};

/*

#NOP: 
index   Address   flag  Op    s       c
0        0+128     1    0000  000 -> MI|PCO 
1        1+128     1    0000  001 -> RO|II 
2        2+128     1    0000  010 -> PCE 
3        3+128     1    0000  011 -> 0 
4        4+128     1    0000  100 -> 0 
5        5+128     1    0000  101 -> 0          
6        6+128     1    0000  110 -> 0 
7        7+128     1    0000  111 -> 0
*/

uint16_t flag_1_data[] = {
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 0000 - NOP
  MI|PCO,  RO|II, PCE,  IO|MI,    RO|AI,  0,             0, 0,   // 0001 - LDA
  MI|PCO,  RO|II, PCE,  IO|MI,    RO|BI,  SO|AI,         0, 0,   // 0010 - ADD
  MI|PCO,  RO|II, PCE,  IO|MI,    RO|BI,  SO|SUB|FI|AI,  0, 0,   // 0011 - SUB
  MI|PCO,  RO|II, PCE,  IO|MI,    AO|RI,  0,             0, 0,   // 0100 - STA
  MI|PCO,  RO|II, PCE,  IO|AI,    0,      0,             0, 0,   // 0101 - LDI
  MI|PCO,  RO|II, PCE,  IO|PCI,   0,      0,             0, 0,   // 0110 - JMP
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 0111 - JNZ
  MI|PCO,  RO|II, PCE,  IO|BI,    SO|SUB|FI|AI, 0,       0, 0,   // 1000 - SUBAI
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1001
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1010
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1011
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1100
  MI|PCO,  RO|II, PCE,  0,        0,      0,             0, 0,   // 1101
  MI|PCO,  RO|II, PCE,  AO|OI,    0,      0,             0, 0,   // 1110 - OUT
  MI|PCO,  RO|II, HLT,  0,      0,      0,             0, 0,   // 1111 - HLT
}; 

void invoke_store_clock() {
  digitalWrite(ST_CLK, LOW);
  digitalWrite(ST_CLK, HIGH);
  digitalWrite(ST_CLK, LOW);
}

void put_shift_data(int data) {
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, data >> 8);
    shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, data);
    invoke_store_clock();
}


void set_address_for_read(int address) {
  put_shift_data(address);
}

void set_address_for_write(int address) {
  address = address | 0x8000;
  put_shift_data(address);
}

void writeROM(int address, byte data) {
  for (int pin = IO_0; pin <= IO_7; pin += 1) {
    pinMode(pin, OUTPUT);
  }

  set_address_for_write(address);
  set_one_byte_data(data);
  invoke_write_pulse();
}

void set_one_byte_data(byte data) {
  // IO_0 -> IO_7
  for (int pin = IO_0; pin <= IO_7; pin += 1) {
    digitalWrite(pin, data & 1); // 00000001
    data = data >> 1;
  }
}

void invoke_write_pulse() {
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(10);
  digitalWrite(WRITE_EN, HIGH);
  delay(20);
}

void printContents() {
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1) {
      data[offset] = readROM(base + offset);
    }

    char buf[80];
    sprintf(buf, "%03x:  %02x %02x %02x %02x %02x %02x %02x %02x   %02x %02x %02x %02x %02x %02x %02x %02x",
            base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7],
            data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);
  }
}

byte readROM(int address) {
  for (int pin = IO_0; pin <= IO_7; pin += 1) {
    pinMode(pin, INPUT);
  }
  set_address_for_read(address); // output

  byte data = 0;
  for (int pin = IO_7; pin >= IO_0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

void setup() {
  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(ST_CLK, OUTPUT);

  digitalWrite(WRITE_EN, HIGH);
  pinMode(WRITE_EN, OUTPUT);

  Serial.begin(57600);
  Serial.print("Write Rom...");
  
  //writeROM(int address, byte data);
  //writeROM(0b00001011, 0b00000000);

  for (int address = 0; address < 128; address += 1) {
    // left part
    //writeROM(address, flag_0_data[address] >> 8);
    //writeROM(address+128, flag_1_data[address] >> 8);

    // right
    writeROM(address, flag_0_data[address] & 0b0000000011111111);
    writeROM(address+128, flag_1_data[address] & 0b0000000011111111);
    if (address % 16 == 0) {
      Serial.print(".");
    }
  }

  Serial.println("Done!");
  printContents();
}

void loop() {

}
