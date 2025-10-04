/*
 *  Project     ArduinoPS4 AVR core
 *  @author     Flamethrower
 *  @link       github.com/Flamethr0wer/ArduinoPS4_AVR
 *  @license    MIT - Copyright (c) 2019 Flamethrower
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "ArduinoPS4_USBAPI.h"

#ifdef ARDUINOPS4_USB

void (*ArduinoPS4USB::RecvCallback)(void) = nullptr;
void (*ArduinoPS4USB::SendCallback)(void) = nullptr;

boolean ArduinoPS4USB::connected() {
	return USBDevice.configured();
}

uint8_t ArduinoPS4USB::available() {
	return USB_Available(ARDUINOPS4_RX_ENDPOINT);
}

int ArduinoPS4USB::recv(void *buffer, uint8_t nbytes) {
	return USB_Recv(ARDUINOPS4_RX_ENDPOINT, buffer, nbytes);
}

int ArduinoPS4USB::send(const void *buffer, uint8_t nbytes) {
	int result = USB_Send(ARDUINOPS4_TX_ENDPOINT, buffer, nbytes);
	if (result > 0) {
		USB_Flush(ARDUINOPS4_TX_ENDPOINT);
	}
	return result;
}

void ArduinoPS4USB::setRecvCallback(void(*callback)(void)) {
	ArduinoPS4USB::RecvCallback = callback;
}

void ArduinoPS4USB::setSendCallback(void(*callback)(void)) {
	ArduinoPS4USB::SendCallback = callback;
}

#endif
