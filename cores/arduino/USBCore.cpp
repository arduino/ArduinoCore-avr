

/* Copyright (c) 2010, Peter Barrett
** Sleep/Wakeup support added by Michael Dreher
**  
** Permission to use, copy, modify, and/or distribute this software for  
** any purpose with or without fee is hereby granted, provided that the  
** above copyright notice and this permission notice appear in all copies.  
** 
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL  
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED  
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR  
** BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES  
** OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,  
** WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  
** ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS  
** SOFTWARE.  
*/

#include "Arduino.h"
#include "api/USBAPI.h"
#include "api/PluggableUSB.h"
#include "USBCore.h"
#include "pins_arduino.h"
#include <stdlib.h>

#if defined(USBCON)

/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
volatile uint8_t TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
volatile uint8_t RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */

//==================================================================
//==================================================================

extern const uint16_t STRING_LANGUAGE[] PROGMEM;
extern const uint8_t STRING_PRODUCT[] PROGMEM;
extern const uint8_t STRING_MANUFACTURER[] PROGMEM;
extern const DeviceDescriptor USB_DeviceDescriptorIAD PROGMEM;

const uint16_t STRING_LANGUAGE[2] = {
	(3<<8) | (2+2),
	0x0409	// English
};

#ifndef USB_PRODUCT
// If no product is provided, use USB IO Board
#define USB_PRODUCT     "USB IO Board"
#endif

const uint8_t STRING_PRODUCT[] PROGMEM = USB_PRODUCT;

#if USB_VID == 0x2341
#  if defined(USB_MANUFACTURER)
#    undef USB_MANUFACTURER
#  endif
#  define USB_MANUFACTURER "Arduino LLC"
#elif USB_VID == 0x1b4f
#  if defined(USB_MANUFACTURER)
#    undef USB_MANUFACTURER
#  endif
#  define USB_MANUFACTURER "SparkFun"
#elif !defined(USB_MANUFACTURER)
// Fall through to unknown if no manufacturer name was provided in a macro
#  define USB_MANUFACTURER "Unknown"
#endif

const uint8_t STRING_MANUFACTURER[] PROGMEM = USB_MANUFACTURER;


#define DEVICE_CLASS 0x02

//	DEVICE DESCRIPTOR
const DeviceDescriptor USB_DeviceDescriptorIAD =
	D_DEVICE(0xEF,0x02,0x01,64,USB_VID,USB_PID,0x100,IMANUFACTURER,IPRODUCT,ISERIAL,1);

//==================================================================
//==================================================================

volatile uint8_t _usbConfiguration = 0;
volatile uint8_t _usbCurrentStatus = 0; // meaning of bits see usb_20.pdf, Figure 9-4. Information Returned by a GetStatus() Request to a Device
volatile uint8_t _usbSuspendState = 0; // copy of UDINT to check SUSPI and WAKEUPI bits

static inline void WaitIN(void)
{
	while (!(UEINTX & (1<<TXINI)))
		;
}

static inline void ClearIN(void)
{
	UEINTX = ~(1<<TXINI);
}

static inline void WaitOUT(void)
{
	while (!(UEINTX & (1<<RXOUTI)))
		;
}

static inline uint8_t WaitForINOrOUT()
{
	while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))))
		;
	return (UEINTX & (1<<RXOUTI)) == 0;
}

static inline void ClearOUT(void)
{
	UEINTX = ~(1<<RXOUTI);
}

static inline void Recv(volatile uint8_t* data, uint8_t count)
{
	while (count--)
		*data++ = UEDATX;
	
	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;	
}

static inline uint8_t Recv8()
{
	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;

	return UEDATX;	
}

static inline void Send8(uint8_t d)
{
	UEDATX = d;
}

static inline void SetEP(uint8_t ep)
{
	UENUM = ep;
}

static inline uint8_t FifoByteCount()
{
	return UEBCLX;
}

static inline uint8_t ReceivedSetupInt()
{
	return UEINTX & (1<<RXSTPI);
}

static inline void ClearSetupInt()
{
	UEINTX = ~((1<<RXSTPI) | (1<<RXOUTI) | (1<<TXINI));
}

static inline void Stall()
{
	UECONX = (1<<STALLRQ) | (1<<EPEN);
}

static inline uint8_t ReadWriteAllowed()
{
	return UEINTX & (1<<RWAL);
}

static inline uint8_t Stalled()
{
	return UEINTX & (1<<STALLEDI);
}

static inline uint8_t FifoFree()
{
	return UEINTX & (1<<FIFOCON);
}

static inline void ReleaseRX()
{
	UEINTX = 0x6B;	// FIFOCON=0 NAKINI=1 RWAL=1 NAKOUTI=0 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=1
}

static inline void ReleaseTX()
{
	UEINTX = 0x3A;	// FIFOCON=0 NAKINI=0 RWAL=1 NAKOUTI=1 RXSTPI=1 RXOUTI=0 STALLEDI=1 TXINI=0
}

static inline uint8_t FrameNumber()
{
	return UDFNUML;
}

//==================================================================
//==================================================================

uint8_t USBGetConfiguration(void)
{
	return _usbConfiguration;
}

#define USB_RECV_TIMEOUT
class LockEP
{
	uint8_t _sreg;
public:
	LockEP(uint8_t ep) : _sreg(SREG)
	{
		cli();
		SetEP(ep & 7);
	}
	~LockEP()
	{
		SREG = _sreg;
	}
};

//	Number of bytes, assumes a rx endpoint
uint8_t USB_Available(uint8_t ep)
{
	LockEP lock(ep);
	return FifoByteCount();
}

//	Non Blocking receive
//	Return number of bytes read
int USB_Recv(uint8_t ep, void* d, int len)
{
	if (!_usbConfiguration || len < 0)
		return -1;
	
	LockEP lock(ep);
	uint8_t n = FifoByteCount();
	len = min(n,len);
	n = len;
	uint8_t* dst = (uint8_t*)d;
	while (n--)
		*dst++ = Recv8();
	if (len && !FifoByteCount())	// release empty buffer
		ReleaseRX();
	
	return len;
}

//	Recv 1 byte if ready
int USB_Recv(uint8_t ep)
{
	uint8_t c;
	if (USB_Recv(ep,&c,1) != 1)
		return -1;
	return c;
}

//	Space in send EP
uint8_t USB_SendSpace(uint8_t ep)
{
	LockEP lock(ep);
	if (!ReadWriteAllowed())
		return 0;
	return USB_EP_SIZE - FifoByteCount();
}

//	Blocking Send of data to an endpoint
int USB_Send(uint8_t ep, const void* d, int len)
{
	if (!_usbConfiguration)
		return -1;

	if (_usbSuspendState & (1<<SUSPI)) {
		//send a remote wakeup
		UDCON |= (1 << RMWKUP);
	}

	int r = len;
	const uint8_t* data = (const uint8_t*)d;
	uint8_t timeout = 250;		// 250ms timeout on send? TODO
	bool sendZlp = false;

	while (len || sendZlp)
	{
		uint8_t n = USB_SendSpace(ep);
		if (n == 0)
		{
			if (!(--timeout))
				return -1;
			delay(1);
			continue;
		}

		if (n > len) {
			n = len;
		}

		{
			LockEP lock(ep);
			// Frame may have been released by the SOF interrupt handler
			if (!ReadWriteAllowed())
				continue;

			len -= n;
			if (ep & TRANSFER_ZERO)
			{
				while (n--)
					Send8(0);
			}
			else if (ep & TRANSFER_PGM)
			{
				while (n--)
					Send8(pgm_read_byte(data++));
			}
			else
			{
				while (n--)
					Send8(*data++);
			}

			if (sendZlp) {
				ReleaseTX();
				sendZlp = false;
			} else if (!ReadWriteAllowed()) { // ...release if buffer is full...
				ReleaseTX();
				if (len == 0) sendZlp = true;
			} else if ((len == 0) && (ep & TRANSFER_RELEASE)) { // ...or if forced with TRANSFER_RELEASE
				// XXX: TRANSFER_RELEASE is never used can be removed?
				ReleaseTX();
			}
		}
	}
	TXLED1;					// light the TX LED
	TxLEDPulse = TX_RX_LED_PULSE_MS;
	return r;
}

uint16_t _initEndpoints[USB_ENDPOINTS] =
{
	0,                      // Control Endpoint
	
	EP_TYPE_INTERRUPT_IN,   // CDC_ENDPOINT_ACM
	EP_TYPE_BULK_OUT,       // CDC_ENDPOINT_OUT
	EP_TYPE_BULK_IN,        // CDC_ENDPOINT_IN

	// Following endpoints are automatically initialized to 0
};

#define EP_SINGLE_64 0x32	// EP0
#define EP_DOUBLE_64 0x36	// Other endpoints
#define EP_SINGLE_16 0x12

void* epBuffer(unsigned int lastEp) {
	return &(_initEndpoints[lastEp]);
}

static
void InitEP(uint8_t index, uint8_t type, uint8_t size)
{
	UENUM = index;
	UECONX = (1<<EPEN);
	UECFG0X = type;
	UECFG1X = size;
}

static
void InitEndpoints()
{
	for (uint8_t i = 1; i < sizeof(_initEndpoints) && _initEndpoints[i] != 0; i++)
	{
		UENUM = i;
		UECONX = (1<<EPEN);
		UECFG0X = (uint8_t)_initEndpoints[i];
#if USB_EP_SIZE == 16
		UECFG1X = EP_SINGLE_16;
#elif USB_EP_SIZE == 64
		UECFG1X = EP_DOUBLE_64;
#else
#error Unsupported value for USB_EP_SIZE
#endif
	}
	UERST = 0x7E;	// And reset them
	UERST = 0;
}

//	Handle CLASS_INTERFACE requests
static
bool ClassInterfaceRequest(USBSetup& setup)
{
	uint8_t i = setup.wIndex;

	if (CDC_ACM_INTERFACE == i)
		return CDC_Setup(setup);

#ifdef PLUGGABLE_USB_ENABLED
	return PluggableUSB().setup(setup);
#endif
	return false;
}

static int _cmark;
static int _cend;
void InitControl(int end)
{
	SetEP(0);
	_cmark = 0;
	_cend = end;
}

static
bool SendControl(uint8_t d)
{
	if (_cmark < _cend)
	{
		if (!WaitForINOrOUT())
			return false;
		Send8(d);
		if (!((_cmark + 1) & 0x3F))
			ClearIN();	// Fifo is full, release this packet
	}
	_cmark++;
	return true;
}

//	Clipped by _cmark/_cend
int USB_SendControl(uint8_t flags, const void* d, int len)
{
	int sent = len;
	const uint8_t* data = (const uint8_t*)d;
	bool pgm = flags & TRANSFER_PGM;
	while (len--)
	{
		uint8_t c = pgm ? pgm_read_byte(data++) : *data++;
		if (!SendControl(c))
			return -1;
	}
	return sent;
}

// Send a USB descriptor string. The string is stored in PROGMEM as a
// plain ASCII string but is sent out as UTF-16 with the correct 2-byte
// prefix
static bool USB_SendStringDescriptor(const uint8_t*string_P, uint8_t string_len, uint8_t flags) {
        SendControl(2 + string_len * 2);
        SendControl(3);
        bool pgm = flags & TRANSFER_PGM;
        for(uint8_t i = 0; i < string_len; i++) {
                bool r = SendControl(pgm ? pgm_read_byte(&string_P[i]) : string_P[i]);
                r &= SendControl(0); // high byte
                if(!r) {
                        return false;
                }
        }
        return true;
}

//	Does not timeout or cross fifo boundaries
int USB_RecvControl(void* d, int len)
{
	auto length = len;
	while(length)
	{
		// Dont receive more than the USB Control EP has to offer
		// Use fixed 64 because control EP always have 64 bytes even on 16u2.
		auto recvLength = length;
		if(recvLength > 64){
			recvLength = 64;
		}

		// Write data to fit to the end (not the beginning) of the array
		WaitOUT();
		Recv((uint8_t*)d + len - length, recvLength);
		ClearOUT();
		length -= recvLength;
	}
	return len;
}

static uint8_t SendInterfaces()
{
	uint8_t interfaces = 0;

	CDC_GetInterface(&interfaces);

#ifdef PLUGGABLE_USB_ENABLED
	PluggableUSB().getInterface(&interfaces);
#endif

	return interfaces;
}

//	Construct a dynamic configuration descriptor
//	This really needs dynamic endpoint allocation etc
//	TODO
static
bool SendConfiguration(int maxlen)
{
	//	Count and measure interfaces
	InitControl(0);
	uint8_t interfaces = SendInterfaces();
	ConfigDescriptor config = D_CONFIG(_cmark + sizeof(ConfigDescriptor),interfaces);

	//	Now send them
	InitControl(maxlen);
	USB_SendControl(0,&config,sizeof(ConfigDescriptor));
	SendInterfaces();
	return true;
}

static
bool SendDescriptor(USBSetup& setup)
{
	uint8_t t = setup.wValueH;
	if (USB_CONFIGURATION_DESCRIPTOR_TYPE == t)
		return SendConfiguration(setup.wLength);

	InitControl(setup.wLength);
#ifdef PLUGGABLE_USB_ENABLED
	int ret = PluggableUSB().getDescriptor(setup);
	if (ret != 0) {
		return (ret > 0 ? true : false);
	}
#endif

	const uint8_t* desc_addr = 0;
	if (USB_DEVICE_DESCRIPTOR_TYPE == t)
	{
		desc_addr = (const uint8_t*)&USB_DeviceDescriptorIAD;
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		if (setup.wValueL == 0) {
			desc_addr = (const uint8_t*)&STRING_LANGUAGE;
		}
		else if (setup.wValueL == IPRODUCT) {
			return USB_SendStringDescriptor(STRING_PRODUCT, strlen(USB_PRODUCT), TRANSFER_PGM);
		}
		else if (setup.wValueL == IMANUFACTURER) {
			return USB_SendStringDescriptor(STRING_MANUFACTURER, strlen(USB_MANUFACTURER), TRANSFER_PGM);
		}
		else if (setup.wValueL == ISERIAL) {
#ifdef PLUGGABLE_USB_ENABLED
			char name[ISERIAL_MAX_LEN];
			PluggableUSB().getShortName(name);
			return USB_SendStringDescriptor((uint8_t*)name, strlen(name), 0);
#endif
		}
		else
			return false;
	}

	if (desc_addr == 0)
		return false;
	uint8_t desc_length = pgm_read_byte(desc_addr);

	USB_SendControl(TRANSFER_PGM,desc_addr,desc_length);
	return true;
}

//	Endpoint 0 interrupt
ISR(USB_COM_vect)
{
    SetEP(0);
	if (!ReceivedSetupInt())
		return;

	USBSetup setup;
	Recv((uint8_t*)&setup,8);
	ClearSetupInt();

	uint8_t requestType = setup.bmRequestType;
	if (requestType & REQUEST_DEVICETOHOST)
		WaitIN();
	else
		ClearIN();

    bool ok = true;
	if (REQUEST_STANDARD == (requestType & REQUEST_TYPE))
	{
		//	Standard Requests
		uint8_t r = setup.bRequest;
		uint16_t wValue = setup.wValueL | (setup.wValueH << 8);
		if (GET_STATUS == r)
		{
			if (requestType == (REQUEST_DEVICETOHOST | REQUEST_STANDARD | REQUEST_DEVICE))
			{
				Send8(_usbCurrentStatus);
				Send8(0);
			}
			else
			{
				// TODO: handle the HALT state of an endpoint here
				// see "Figure 9-6. Information Returned by a GetStatus() Request to an Endpoint" in usb_20.pdf for more information
				Send8(0);
				Send8(0);
			}
		}
		else if (CLEAR_FEATURE == r)
		{
			if((requestType == (REQUEST_HOSTTODEVICE | REQUEST_STANDARD | REQUEST_DEVICE))
				&& (wValue == DEVICE_REMOTE_WAKEUP))
			{
				_usbCurrentStatus &= ~FEATURE_REMOTE_WAKEUP_ENABLED;
			}
		}
		else if (SET_FEATURE == r)
		{
			if((requestType == (REQUEST_HOSTTODEVICE | REQUEST_STANDARD | REQUEST_DEVICE))
				&& (wValue == DEVICE_REMOTE_WAKEUP))
			{
				_usbCurrentStatus |= FEATURE_REMOTE_WAKEUP_ENABLED;
			}
		}
		else if (SET_ADDRESS == r)
		{
			WaitIN();
			UDADDR = setup.wValueL | (1<<ADDEN);
		}
		else if (GET_DESCRIPTOR == r)
		{
			ok = SendDescriptor(setup);
		}
		else if (SET_DESCRIPTOR == r)
		{
			ok = false;
		}
		else if (GET_CONFIGURATION == r)
		{
			Send8(1);
		}
		else if (SET_CONFIGURATION == r)
		{
			if (REQUEST_DEVICE == (requestType & REQUEST_RECIPIENT))
			{
				InitEndpoints();
				_usbConfiguration = setup.wValueL;
			} else
				ok = false;
		}
		else if (GET_INTERFACE == r)
		{
		}
		else if (SET_INTERFACE == r)
		{
		}
	}
	else
	{
		InitControl(setup.wLength);		//	Max length of transfer
		ok = ClassInterfaceRequest(setup);
	}

	if (ok)
		ClearIN();
	else
	{
		Stall();
	}
}

void USB_Flush(uint8_t ep)
{
	SetEP(ep);
	if (FifoByteCount())
		ReleaseTX();
}

static inline void USB_ClockDisable()
{
#if defined(OTGPADE)
	USBCON = (USBCON & ~(1<<OTGPADE)) | (1<<FRZCLK); // freeze clock and disable VBUS Pad
#else // u2 Series
	USBCON = (1 << FRZCLK); // freeze clock
#endif
	PLLCSR &= ~(1<<PLLE);  // stop PLL
}

static inline void USB_ClockEnable()
{
#if defined(UHWCON)
	UHWCON |= (1<<UVREGE);			// power internal reg
#endif
	USBCON = (1<<USBE) | (1<<FRZCLK);	// clock frozen, usb enabled

// ATmega32U4
#if defined(PINDIV)
#if F_CPU == 16000000UL
	PLLCSR |= (1<<PINDIV);                   // Need 16 MHz xtal
#elif F_CPU == 8000000UL
	PLLCSR &= ~(1<<PINDIV);                  // Need  8 MHz xtal
#else
#error "Clock rate of F_CPU not supported"
#endif

#elif defined(__AVR_AT90USB82__) || defined(__AVR_AT90USB162__) || defined(__AVR_ATmega32U2__) || defined(__AVR_ATmega16U2__) || defined(__AVR_ATmega8U2__)
	// for the u2 Series the datasheet is confusing. On page 40 its called PINDIV and on page 290 its called PLLP0
#if F_CPU == 16000000UL
	// Need 16 MHz xtal
	PLLCSR |= (1 << PLLP0);
#elif F_CPU == 8000000UL
	// Need 8 MHz xtal
	PLLCSR &= ~(1 << PLLP0);
#endif

// AT90USB646, AT90USB647, AT90USB1286, AT90USB1287
#elif defined(PLLP2)
#if F_CPU == 16000000UL
#if defined(__AVR_AT90USB1286__) || defined(__AVR_AT90USB1287__)
	// For Atmel AT90USB128x only. Do not use with Atmel AT90USB64x.
	PLLCSR = (PLLCSR & ~(1<<PLLP1)) | ((1<<PLLP2) | (1<<PLLP0)); // Need 16 MHz xtal
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB647__)
	// For AT90USB64x only. Do not use with AT90USB128x.
	PLLCSR = (PLLCSR & ~(1<<PLLP0)) | ((1<<PLLP2) | (1<<PLLP1)); // Need 16 MHz xtal
#else
#error "USB Chip not supported, please defined method of USB PLL initialization"
#endif
#elif F_CPU == 8000000UL
	// for Atmel AT90USB128x and AT90USB64x
	PLLCSR = (PLLCSR & ~(1<<PLLP2)) | ((1<<PLLP1) | (1<<PLLP0)); // Need 8 MHz xtal
#else
#error "Clock rate of F_CPU not supported"
#endif
#else
#error "USB Chip not supported, please defined method of USB PLL initialization"
#endif

	PLLCSR |= (1<<PLLE);
	while (!(PLLCSR & (1<<PLOCK)))		// wait for lock pll
	{
	}

	// Some tests on specific versions of macosx (10.7.3), reported some
	// strange behaviors when the board is reset using the serial
	// port touch at 1200 bps. This delay fixes this behavior.
	delay(1);
#if defined(OTGPADE)
	USBCON = (USBCON & ~(1<<FRZCLK)) | (1<<OTGPADE);	// start USB clock, enable VBUS Pad
#else
	USBCON &= ~(1 << FRZCLK);	// start USB clock
#endif

#if defined(RSTCPU)
#if defined(LSM)
	UDCON &= ~((1<<RSTCPU) | (1<<LSM) | (1<<RMWKUP) | (1<<DETACH));	// enable attach resistor, set full speed mode
#else // u2 Series
	UDCON &= ~((1 << RSTCPU) | (1 << RMWKUP) | (1 << DETACH));	// enable attach resistor, set full speed mode
#endif
#else
	// AT90USB64x and AT90USB128x don't have RSTCPU
	UDCON &= ~((1<<LSM) | (1<<RMWKUP) | (1<<DETACH));	// enable attach resistor, set full speed mode
#endif
}

//	General interrupt
ISR(USB_GEN_vect)
{
	uint8_t udint = UDINT;
	UDINT &= ~((1<<EORSTI) | (1<<SOFI)); // clear the IRQ flags for the IRQs which are handled here, except WAKEUPI and SUSPI (see below)

	//	End of Reset
	if (udint & (1<<EORSTI))
	{
		InitEP(0,EP_TYPE_CONTROL,EP_SINGLE_64);	// init ep0
		_usbConfiguration = 0;			// not configured yet
		UEIENX = 1 << RXSTPE;			// Enable interrupts for ep0
	}

	//	Start of Frame - happens every millisecond so we use it for TX and RX LED one-shot timing, too
	if (udint & (1<<SOFI))
	{
		USB_Flush(CDC_TX);				// Send a tx frame if found
		
		// check whether the one-shot period has elapsed.  if so, turn off the LED
		if (TxLEDPulse && !(--TxLEDPulse))
			TXLED0;
		if (RxLEDPulse && !(--RxLEDPulse))
			RXLED0;
	}

	// the WAKEUPI interrupt is triggered as soon as there are non-idle patterns on the data
	// lines. Thus, the WAKEUPI interrupt can occur even if the controller is not in the "suspend" mode.
	// Therefore the we enable it only when USB is suspended
	if (udint & (1<<WAKEUPI))
	{
		UDIEN = (UDIEN & ~(1<<WAKEUPE)) | (1<<SUSPE); // Disable interrupts for WAKEUP and enable interrupts for SUSPEND

		//TODO
		// WAKEUPI shall be cleared by software (USB clock inputs must be enabled before).
		//USB_ClockEnable();
		UDINT &= ~(1<<WAKEUPI);
		_usbSuspendState = (_usbSuspendState & ~(1<<SUSPI)) | (1<<WAKEUPI);
	}
	else if (udint & (1<<SUSPI)) // only one of the WAKEUPI / SUSPI bits can be active at time
	{
		UDIEN = (UDIEN & ~(1<<SUSPE)) | (1<<WAKEUPE); // Disable interrupts for SUSPEND and enable interrupts for WAKEUP

		//TODO
		//USB_ClockDisable();

		UDINT &= ~((1<<WAKEUPI) | (1<<SUSPI)); // clear any already pending WAKEUP IRQs and the SUSPI request
		_usbSuspendState = (_usbSuspendState & ~(1<<WAKEUPI)) | (1<<SUSPI);
	}
}

//	VBUS or counting frames
//	Any frame counting?
uint8_t USBConnected()
{
	uint8_t f = UDFNUML;
	delay(3);
	return f != UDFNUML;
}

//=======================================================================
//=======================================================================

USBDevice_ USBDevice;

USBDevice_::USBDevice_()
{
}

void USBDevice_::attach()
{
	_usbConfiguration = 0;
	_usbCurrentStatus = 0;
	_usbSuspendState = 0;
	USB_ClockEnable();

	UDINT &= ~((1<<WAKEUPI) | (1<<SUSPI)); // clear already pending WAKEUP / SUSPEND requests
	UDIEN = (1<<EORSTE) | (1<<SOFE) | (1<<SUSPE);	// Enable interrupts for EOR (End of Reset), SOF (start of frame) and SUSPEND
	
	TX_RX_LED_INIT;
}

void USBDevice_::detach()
{
}

//	Check for interrupts
//	TODO: VBUS detection
bool USBDevice_::configured()
{
	return _usbConfiguration;
}

void USBDevice_::poll()
{
}

bool USBDevice_::wakeupHost()
{
	// clear any previous wakeup request which might have been set but could be processed at that time
	// e.g. because the host was not suspended at that time
	UDCON &= ~(1 << RMWKUP);

	if(!(UDCON & (1 << RMWKUP))
	  && (_usbSuspendState & (1<<SUSPI))
	  && (_usbCurrentStatus & FEATURE_REMOTE_WAKEUP_ENABLED))
	{
		// This short version will only work, when the device has not been suspended. Currently the
		// Arduino core doesn't handle SUSPEND at all, so this is ok.
		USB_ClockEnable();
		UDCON |= (1 << RMWKUP); // send the wakeup request
		return true;
	}

	return false;
}


PluggableUSB_::PluggableUSB_() : lastIf(CDC_ACM_INTERFACE + CDC_INTERFACE_COUNT),
                                 lastEp(CDC_FIRST_ENDPOINT + CDC_ENPOINT_COUNT),
                                 rootNode(NULL), totalEP(USB_ENDPOINTS)
{
	// Empty
}

bool USBDevice_::isSuspended()
{
	return (_usbSuspendState & (1 << SUSPI));
}


#endif /* if defined(USBCON) */
