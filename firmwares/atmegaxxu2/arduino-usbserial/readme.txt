Arduino USBSerial Firmware Build and Flash Instructions
======================================================

This firmware provides USB-to-serial functionality for Arduino boards
using the ATmega8U2/16U2 USB interface.

Note: Arduino Uno R3 uses ATmega16U2 (not ATmega8U2).

--------------------------------------------------
1. Requirements
--------------------------------------------------

Install required tools:

On Debian/Ubuntu:
    sudo apt update
    sudo apt install gcc-avr avr-libc make dfu-programmer git

Verify installation:
    avr-gcc --version
    dfu-programmer --version

--------------------------------------------------
2. LUFA Dependency
--------------------------------------------------

This project depends on the LUFA USB framework.

Tested version:
    LUFA 100807

Download LUFA:
    git clone https://github.com/abcminiuser/lufa.git
    cd lufa
    git checkout 100807

--------------------------------------------------
3. Project Setup
--------------------------------------------------

Place the firmware inside LUFA's Projects directory:

    lufa/
      Projects/
        arduino-usbserial/

Alternatively, specify LUFA path manually:

    make LUFA_PATH=/path/to/lufa

--------------------------------------------------
4. Configure Build
--------------------------------------------------

Edit the Makefile:

    MCU = atmega8u2   (or atmega16u2 for Uno R3)
    ARDUINO_MODEL_PID = <value>

Common values:
    Uno       -> 0x0043
    Mega2560  -> 0x0010

--------------------------------------------------
5. Build Firmware
--------------------------------------------------

    make clean
    make LUFA_PATH=/path/to/lufa

Output:
    Arduino-usbserial.hex

--------------------------------------------------
6. Enter DFU Mode
--------------------------------------------------

1. Connect RESET to GND
2. Connect HWB to GND
3. Release RESET
4. Release HWB

Device should appear as:
    "Arduino Uno DFU" or "Arduino Mega 2560 DFU"

--------------------------------------------------
7. Flash Firmware
--------------------------------------------------

Linux/macOS:

    dfu-programmer atmega8u2 erase
    dfu-programmer atmega8u2 flash Arduino-usbserial.hex
    dfu-programmer atmega8u2 reset

For ATmega16U2:
    replace atmega8u2 with atmega16u2

Windows:
    Use Atmel FLIP ("make flip")

--------------------------------------------------
8. Verification
--------------------------------------------------

Board should appear as:
    "Arduino Uno" or "Arduino Mega 2560"

Test by uploading a sketch using Arduino IDE.

--------------------------------------------------
9. Troubleshooting
--------------------------------------------------

Build fails:
    - Ensure LUFA version is 100807
    - Verify LUFA_PATH is correct

Device not detected:
    - Ensure DFU mode is active
    - Try a different USB port or cable

Board not recognized after flashing:
    - Verify correct MCU (8U2 vs 16U2)
    - Reflash firmware