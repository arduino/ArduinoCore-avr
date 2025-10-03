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

#include "USBAPI.h"
#include "PluggableUSB.h"
#include <stdlib.h>

#if defined(USBCON)

/** Pulse generation counters to keep track of the number of milliseconds remaining for each pulse type */
#define TX_RX_LED_PULSE_MS 100
volatile u8 TxLEDPulse; /**< Milliseconds remaining for data Tx LED pulse */
volatile u8 RxLEDPulse; /**< Milliseconds remaining for data Rx LED pulse */

//==================================================================
//==================================================================

extern const u16 STRING_LANGUAGE[] PROGMEM;
extern const u8 STRING_PRODUCT[] PROGMEM;
extern const u8 STRING_MANUFACTURER[] PROGMEM;

const u16 STRING_LANGUAGE[2] = {
	(3<<8) | (2+2),
	0x0409	// English
};

#ifndef USB_PRODUCT
// If no product is provided, use USB IO Board
#define USB_PRODUCT     "USB IO Board"
#endif

const u8 STRING_PRODUCT[] PROGMEM = USB_PRODUCT;

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

const u8 STRING_MANUFACTURER[] PROGMEM = USB_MANUFACTURER;


//==================================================================
//==================================================================

volatile u8 _usbConfiguration = 0;
volatile u8 _usbCurrentStatus = 0; // meaning of bits see usb_20.pdf, Figure 9-4. Information Returned by a GetStatus() Request to a Device
volatile u8 _usbSuspendState = 0; // copy of UDINT to check SUSPI and WAKEUPI bits

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

static inline u8 WaitForINOrOUT()
{
	while (!(UEINTX & ((1<<TXINI)|(1<<RXOUTI))))
		;
	return (UEINTX & (1<<RXOUTI)) == 0;
}

static inline void ClearOUT(void)
{
	UEINTX = ~(1<<RXOUTI);
}

static inline void Recv(volatile u8* data, u8 count)
{
	while (count--)
		*data++ = UEDATX;
	
	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;	
}

static inline u8 Recv8()
{
	RXLED1;					// light the RX LED
	RxLEDPulse = TX_RX_LED_PULSE_MS;

	return UEDATX;	
}

static inline void Send8(u8 d)
{
	UEDATX = d;
}

static inline void SetEP(u8 ep)
{
	UENUM = ep;
}

static inline u8 FifoByteCount()
{
	return UEBCLX;
}

static inline u8 ReceivedSetupInt()
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

static inline u8 ReadWriteAllowed()
{
	return UEINTX & (1<<RWAL);
}

static inline u8 Stalled()
{
	return UEINTX & (1<<STALLEDI);
}

static inline u8 FifoFree()
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

static inline u8 FrameNumber()
{
	return UDFNUML;
}

//==================================================================
//==================================================================

u8 USBGetConfiguration(void)
{
	return _usbConfiguration;
}

#define USB_RECV_TIMEOUT
class LockEP
{
	u8 _sreg;
public:
	LockEP(u8 ep) : _sreg(SREG)
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
u8 USB_Available(u8 ep)
{
	LockEP lock(ep);
	return FifoByteCount();
}

//	Non Blocking receive
//	Return number of bytes read
int USB_Recv(u8 ep, void* d, int len)
{
	if (!_usbConfiguration || len < 0)
		return -1;
	
	LockEP lock(ep);
	u8 n = FifoByteCount();
	len = min(n,len);
	n = len;
	u8* dst = (u8*)d;
	while (n--)
		*dst++ = Recv8();
	if (len && !FifoByteCount())	// release empty buffer
		ReleaseRX();
	
	return len;
}

//	Recv 1 byte if ready
int USB_Recv(u8 ep)
{
	u8 c;
	if (USB_Recv(ep,&c,1) != 1)
		return -1;
	return c;
}

//	Space in send EP
u8 USB_SendSpace(u8 ep)
{
	LockEP lock(ep);
	if (!ReadWriteAllowed())
		return 0;
	return USB_EP_SIZE - FifoByteCount();
}

//	Blocking Send of data to an endpoint
int USB_Send(u8 ep, const void* d, int len)
{
	if (!_usbConfiguration)
		return -1;

	if (_usbSuspendState & (1<<SUSPI)) {
		//send a remote wakeup
		UDCON |= (1 << RMWKUP);
	}

	int r = len;
	const u8* data = (const u8*)d;
	u8 timeout = 250;		// 250ms timeout on send? TODO
	bool sendZlp = false;

	while (len || sendZlp)
	{
		u8 n = USB_SendSpace(ep);
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

#define EP_SINGLE_64 0x32	// EP0
#define EP_DOUBLE_64 0x36	// Other endpoints
#define EP_SINGLE_16 0x12

static inline
u8 BankSizeMask(const uint8_t nbytes) {
	uint8_t mask = 0;
	for (uint8_t size = 8; size < 64; size <<= 1) {
		if (nbytes <= size) break;
		mask++;
	}
	return (mask << EPSIZE0);
}
static inline

void InitEP(u8 index, u8 type, u8 size)
{
	UENUM = index;
	UECONX = (1<<EPEN);
	UECFG0X = type;
	UECFG1X = size;
}

static inline
bool InitEPSize(const u8 index, const u8 type, const u8 nbanks, const u8 banksize){
	if (index >= USB_ENDPOINTS) return false;
	uint8_t size = ((1 << ALLOC) | ((nbanks > 1) ? (1 << EPBK0) : 0) | BankSizeMask(banksize));
	InitEP(index, type, size);
	return UESTA0X & (1 << CFGOK);  // Success
}
static
void InitEndpoints() {
	InitEPSize(ARDUINODS4_TX_ENDPOINT, EP_TYPE_INTERRUPT_IN,  2, 64);  // Control Data Send
	InitEPSize(ARDUINODS4_RX_ENDPOINT, EP_TYPE_INTERRUPT_OUT, 2, 64);  // Control Data Receive
	InitEPSize(5, EP_TYPE_INTERRUPT_IN,  2, 64);  // Expansion Interface NACK (avoid config reset)

	UERST = 0x7E;  // Reset endpoints
	UERST = 0;     // End reset

	SetEP(ARDUINODS4_TX_ENDPOINT);
	UEIENX |= (1 << TXINE);

	SetEP(ARDUINODS4_RX_ENDPOINT);
	UEIENX |= (1 << RXOUTE);
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
bool SendControl(u8 d)
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
int USB_SendControl(u8 flags, const void* d, int len)
{
	int sent = len;
	const u8* data = (const u8*)d;
	bool pgm = flags & TRANSFER_PGM;
	while (len--)
	{
		u8 c = pgm ? pgm_read_byte(data++) : *data++;
		if (!SendControl(c))
			return -1;
	}
	return sent;
}

// Send a USB descriptor string. The string is stored in PROGMEM as a
// plain ASCII string but is sent out as UTF-16 with the correct 2-byte
// prefix
static bool USB_SendStringDescriptor(const u8*string_P, u8 string_len, uint8_t flags) {
        SendControl(2 + string_len * 2);
        SendControl(3);
        bool pgm = flags & TRANSFER_PGM;
        for(u8 i = 0; i < string_len; i++) {
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
		Recv((u8*)d + len - length, recvLength);
		ClearOUT();
		length -= recvLength;
	}
	return len;
}

//	Construct a dynamic configuration descriptor
//	This really needs dynamic endpoint allocation etc
//	TODO
static
bool SendConfiguration(int maxlen)
{
	InitControl(maxlen);
	USB_SendControl(TRANSFER_PGM, &USB_ConfigDescriptor, USB_ConfigDescriptorSize);
	return true;
}

static
bool SendDescriptor(USBSetup& setup)
{
	u8 t = setup.wValueH;
	if (USB_CONFIGURATION_DESCRIPTOR_TYPE == t)
		return SendConfiguration(setup.wLength);

	InitControl(setup.wLength);
#ifdef PLUGGABLE_USB_ENABLED
	int ret = PluggableUSB().getDescriptor(setup);
	if (ret != 0) {
		return (ret > 0 ? true : false);
	}
#endif

	if (t == 0x21) {
        InitControl(setup.wLength);
    	USB_SendControl(TRANSFER_PGM, hidClassDescriptor, hidClassDescriptorSize, setup.wLength));
    	return true;
    }

	if (t == 0x22) { // HID Report descriptor request
        return USB_SendControl(TRANSFER_PGM, hidDescriptor, hidDescriptorSize) > 0;
    }

	const u8* desc_addr = 0;
	if (USB_DEVICE_DESCRIPTOR_TYPE == t)
	{
		desc_addr = (const u8*) &USB_DeviceDescriptor;
	}
	else if (USB_STRING_DESCRIPTOR_TYPE == t)
	{
		if (setup.wValueL == 0) {
			desc_addr = (const u8*)&STRING_LANGUAGE;
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
#else
			return USB_SendStringDescriptor(STRING_SERIAL, strlen((char*)STRING_SERIAL), TRANSFER_PGM);
#endif
		}
		else if (setup.wValueL == ISECURITY) {
			return USB_SendStringDescriptor(STRING_SECURITY, strlen((char*)STRING_SECURITY), TRANSFER_PGM);
		}
		else
			return false;
	}

	if (desc_addr == 0)
		return false;
	u8 desc_length = pgm_read_byte(desc_addr);

	USB_SendControl(TRANSFER_PGM,desc_addr,desc_length);
	return true;
}

//	Endpoint interrupt
ISR(USB_COM_vect)
{
	SetEP(ARDUINODS4_TX_ENDPOINT);
	if (UEINTX & (1 << TXINI)) {  // If TX buffer is ready
		UEINTX &= ~(1 << TXINI);  // Clear interrupt flag
		if (ArduinoDS4USB::SendCallback != nullptr) {
			ArduinoDS4USB::SendCallback();  // Call callback function if it exists
		}
	}
	
	SetEP(ARDUINODS4_RX_ENDPOINT);
	if (UEINTX & (1 << RXOUTI)) {  // If data received...
		UEINTX &= ~(1 << RXOUTI);  // Clear interrupt flag
		if (ArduinoDS4USB::RecvCallback != nullptr) {
			ArduinoDS4USB::RecvCallback();  // Call callback function if it exists
		}
	}
    SetEP(0);
	if (!ReceivedSetupInt())
		return;

	USBSetup setup;
	Recv((u8*)&setup,8);
	ClearSetupInt();

	u8 requestType = setup.bmRequestType;
	if (requestType & REQUEST_DEVICETOHOST)
		WaitIN();
	else
		ClearIN();

    bool ok = true;
	if (REQUEST_STANDARD == (requestType & REQUEST_TYPE))
	{
		//	Standard Requests
		u8 r = setup.bRequest;
		u16 wValue = setup.wValueL | (setup.wValueH << 8);
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
	    // Handle HID class requests
	    if ((requestType & REQUEST_TYPE) == REQUEST_CLASS)
	    {
	        // HID class requests (0x01 = GET_REPORT, 0x09 = SET_REPORT)
	        if (setup.bRequest == 0x01)  // HID_GET_REPORT
	        {
	            if (setup.wValueH == 0x03)  // Feature report type
	            {
	                uint8_t reportId = setup.wValueL;
	                const u8* featureData = nullptr;
	                u16 featureLen = 0;
	                
	                // Handle different report IDs
	                switch(reportId) {
	                    case 0x02:  // Calibration
	                        featureData = output_0x02;
	                        featureLen = output_0x02_size;
	                        break;
	                    case 0x03:  // Controller descriptor  
	                        featureData = output_0x03;
	                        featureLen = output_0x03_size;
	                        break;
	                    case 0x12:  // MAC address
	                        featureData = output_0x12;
	                        featureLen = output_0x12_size;
	                        break;
	                    case 0xA3:  // Version/date
	                        featureData = output_0xa3;
	                        featureLen = output_0xa3_size;
	                        break;
	                    case 0xF3:  // Reset auth
	                        featureData = output_0xf3;
	                        featureLen = output_0xf3_size;
	                        break;
						case 0xF1:
                		case 0xF2:
							InitControl(0);
                    		ok = true;
                    		break;
	                    default:
	                        ok = false;
	                        break;
	                }
	                
	                if (featureData && featureLen > 0) {
	                    InitControl(min(featureLen, setup.wLength));
	                    USB_SendControl(TRANSFER_PGM, featureData, min(featureLen, setup.wLength));
	                    ok = true;
	                }
	            }
	        }
	        else if (setup.bRequest == 0x09)  // HID_SET_REPORT
	        {
	            if (setup.wValueH == 0x03)  // Feature report type
	            {
	                // For basic PS4 support, we can mostly ignore SET_REPORT
	                // Just acknowledge it was received
	                uint8_t buffer[64];
	                int len = USB_RecvControl(buffer, min(sizeof(buffer), setup.wLength));
	                ok = (len >= 0);
	            }
	            else if (setup.wValueH == 0x02)  // Output report type
	            {
	                // Handle rumble/LED data if needed
	                uint8_t buffer[64];
	                int len = USB_RecvControl(buffer, min(sizeof(buffer), setup.wLength));
	                ok = (len >= 0);
	            }
				else {
			    	ok = true;
				}
				ClearIN();
	        }
			else if (setup.bRequest == 0x0A || setup.bRequest == 0x0B){
  				ClearIN();
  				ok = true;
			}
			else if (setup.bRequest == 0x02)  // HID_GET_IDLE
			{
  				Send8(0);
  				ok = true;
			}
			else if (setup.bRequest == 0x03)  // HID_GET_PROTOCOL
			{
  				Send8(0);
  				ok = true;
			}
	    }
	    else
	    {
	        ok = true;  // Default behavior for vendor requests etc
	    }
	}

	if (ok)
		ClearIN();
	else
	{
		Stall();
	}
}

void USB_Flush(u8 ep)
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
	u8 udint = UDINT;
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
		USB_ClockEnable();             // restart PLL, unfreeze clock
  		UDIEN = (UDIEN & ~(1<<WAKEUPE)) | (1<<SUSPE);
  		UDINT &= ~(1<<WAKEUPI);
  		_usbSuspendState &= ~(1<<SUSPI);
	}
	else if (udint & (1<<SUSPI)) // only one of the WAKEUPI / SUSPI bits can be active at time
	{
		USB_ClockDisable();        // stop PLL, freeze USB clock
  		UDIEN = (UDIEN & ~(1<<SUSPE)) | (1<<WAKEUPE);
  		UDINT &= ~((1<<WAKEUPI)|(1<<SUSPI));
  		_usbSuspendState |= (1<<SUSPI);
	}
}

//	VBUS or counting frames
//	Any frame counting?
u8 USBConnected()
{
	u8 f = UDFNUML;
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

bool USBDevice_::isSuspended()
{
	return (_usbSuspendState & (1 << SUSPI));
}


#endif /* if defined(USBCON) */
