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

#include "ArduinoDS4_USBDesc.h"

#if defined(USBCON)

const DeviceDescriptor USB_DeviceDescriptor = {
	0x12,           // bLength (18)
	0x01,           // bDescriptorType (DEVICE)
	0x0200,         // bcdUSB (2.0)
	0x00,           // bDeviceClass
	0x00,           // bDeviceSubClass
	0x00,           // bDeviceProtocol
	0x40,           // bMaxPacketSize0
	0x054C,        // idEVendor
	0x05C4,        // idProduct
	0x0100,          // bcdDevice
	0x01,  // iManufacturer
	0x02,       // iProduct
	0x00,        // iSerialNumber
	0x01,           // bNumConfigurations
};

const u8 USB_ConfigDescriptor[] = {
	// Configuration Descriptor
	0x09,        // bLength
	0x02,        // bDescriptorType (CONFIGURATION)
	0x29, 0x00,   // wTotalLength (41)
	0x01,        // bNumInterfaces
	0x01,        // bConfigurationValue
	0x00,        // iConfiguration
	0xC0,        // bmAttributes
	0xFA,        // bMaxPower

	/* ---------------------------------------------------- */
	// Interface 0: Control Data
	0x09,        // bLength
	0x04,        // bDescriptorType (INTERFACE)
	0x00,        // bInterfaceNumber
	0x00,        // bAlternateSetting
	0x02,        // bNumEndpoints
	0x03,        // bInterfaceClass
	0x00,        // bInterfaceSubClass
	0x00,        // bInterfaceProtocol
	0x00,        // iInterface

	// Human Interface Device (HID) Descriptor
	0x09,        // bLength
	0x21,        // bDescriptorType
	0x11, 0x01,  // bcdHID 1.17
	0x00,        // bCountryCode
	0x01,        // bNumDescriptors
	0x22,        // bDescriptorType
	0xD3, 0x01,  // DescriptorLength

	// Endpoint 1: Control Surface Send
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x84,        // bEndpointAddress (IN, 1)
	0x03,        // bmAttributes
	0x40, 0x00,  // wMaxPacketSize
	0x05,        // bInterval

	// Endpoint 1: Control Surface Receive
	0x07,        // bLength
	0x05,        // bDescriptorType (ENDPOINT)
	0x03,        // bEndpointAddress (OUT, 2)
	0x03,        // bmAttributes
	0x40, 0x00,  // wMaxPacketSize
	0x05,        // bInterval
};

const u16 USB_ConfigDescriptorSize = sizeof(USB_ConfigDescriptor);

const u8 STRING_SERIAL[] = "Arduino DS4 AVR";
const u8 STRING_SECURITY[] = "PS4 protocol";

#endif
