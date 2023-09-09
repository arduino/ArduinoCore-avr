/*
 * Wire_register_manipulations
 * 
 * This is example showing how to read and write to registers of I2C devices.
 * 
 * The example shows:
 * 1. Writing byte to register
 * 2. Reading byte from register (both blocking and non-blocking)
 * 3. Writing byte to register with verification
 * 4. Writing data block to registers (useful for FIFO buffers)
 * 5. Reading data block from registers (useful for FIFO buffers)
 * 
 * Example is writen by Ivan Stefanov on 10 Sep 2023
 */

#include <Wire.h>

// Demo is made for USB Type-C port controller IC FUSB302B
#define FUSB302_ADDR    (0x22) // FUSB302B I2C address
#define FUSB302_REG_R   (0x01) // FUSB302B I2C register read-only
#define FUSB302_REG_RW  (0x02) // FUSB302B I2C register read/write

void setup() {
  Wire.begin();
  Wire.setClock(I2C_FAST_SPEED);
  delay(100); // I2C device power-up delay

  Serial.begin(115200);
  Serial.println("Wire library register manipulation functions example");

  uint8_t test = 0;
  bool dev = false;

  /* Write and read byte from register */
  
  // Write byte to device register
  Wire.registerWrite(FUSB302_ADDR, FUSB302_REG_RW, 0x64);
  Serial.print("Write to read/write register: ");
  Serial.println(0x64, HEX);

  // Read(blocking) byte from device register
  test = Wire.registerRead(FUSB302_ADDR, FUSB302_REG_RW);
  Serial.print("Read read/write register: ");
  Serial.println((unsigned int)test, HEX);

  // Write and verify byte to device register
  dev = Wire.registerWriteVerify(FUSB302_ADDR, FUSB302_REG_R, 0xC8);
  Serial.print("Write 0xC8 to read-only register > success: ");
  Serial.println(dev);

  // Read(non-blocking) byte from device register
  test = Wire.registerRead(FUSB302_ADDR, FUSB302_REG_R, false);
  Serial.print("Read read-only register: ");
  Serial.println((unsigned int)test, HEX);

  // Data to be writen to register block
  uint8_t arr[4] = {0}, dat[4] = {0x91, 0x64, 0xA4, 0x30};

  /* Write and read 4 byte block */
  
  // write data starting from read-only register followed by 3 read/write registers
  Wire.registerBlockWrite(FUSB302_ADDR, FUSB302_REG_R, dat, 4);
  Serial.print("Block write data: {0x91, 0x64, 0xA4, 0x30}\n");

  // read data and print to serial monitor
  Wire.registerBlockRead(FUSB302_ADDR, FUSB302_REG_R, arr, sizeof(arr));
  Serial.print("Block Read: ");
  for (size_t cnt = 0 ; cnt < 4 ; cnt++) {
    Serial.print((unsigned int)arr[cnt], HEX);
    Serial.print(", ");
  }
  Serial.print("\n");
}

void loop() {
  // put your main code here, to run repeatedly:

}
