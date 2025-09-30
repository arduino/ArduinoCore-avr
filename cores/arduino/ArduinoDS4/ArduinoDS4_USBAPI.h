/*
 *  Project     ArduinoDS4 AVR core
 *  @author     Flamethrower
 *  @link       github.com/Flamethr0wer/ArduinoDS4_AVR
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

#include "USBAPI.h"

#ifndef ARDUINODS4_USBAPI_H
#define ARDUINODS4_USBAPI_H

#ifdef USBCON

#define ARDUINODS4_USB

class ArduinoDS4USB {
public:
	static bool connected(void);
	static uint8_t available(void);
	static int send(const void *buffer, uint8_t nbytes);
	static int recv(void *buffer, uint8_t nbytes);
	static void setRecvCallback(void(*callback)(void));
	static void setSendCallback(void(*callback)(void));

	static void (*RecvCallback)(void);
	static void (*SendCallback)(void);
};

#endif

#endif
