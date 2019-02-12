
# Arduino AVR I2C library timeout
The read write operations in I2C for AVR cores do not have any timeout. So, in an unsuccessful I2C read or write operation, the hardware may get stuck in an infinite loop. This situation has been observed mostly when the connection between the hardware and the I2C device is loose. In this submission a timeout function has been added to Wire library.
