//
//
// Pins definitions for Generic 1284P
// Original file from Maniacbug's Mighty-1284 core
//
// This version by Phillip Stevens (7th February 2016)
//
//

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

//                           ATMEL ATMEGA1284P
//
//                               +---\/---+
//            PCINT8 (D14) PB0  1|        |40  PA0 (AI 0 / D24) PCINT0
//            PCINT9 (D15) PB1  2|        |39  PA1 (AI 1 / D25) PCINT1
//      PCINT10/INT2 (D 8) PB2  3|        |38  PA2 (AI 2 / D26) PCINT2
//      PCINT11/OC0A*(D 9) PB3  4|        |37  PA3 (AI 3 / D27) PCINT3
//   PCINT12/0C0B/SS*(D10) PB4  5|        |36  PA4 (AI 4 / D28) PCINT4
//      PCINT13/MOSI (D11) PB5  6|        |35  PA5 (AI 5 / D29) PCINT5
// PCINT14/OC3A/MISO*(D12) PB6  7|        |34  PA6 (AI 6 / D30) PCINT6
//  PCINT15/OC3B/SCK*(D13) PB7  8|        |33  PA7 (AI 7 / D31) PCINT7
//                         RST  9|        |32  AREF
//                         VCC 10|        |31  GND
//                         GND 11|        |30  AVCC
//                       XTAL2 12|        |29  PC7 (D23) TOSC2/PCINT23
//                       XTAL1 13|        |28  PC6 (D22) TOSC1/PCINT22
//       PCINT24/RX0 (D 0) PD0 14|        |27  PC5 (D21) TDI/PCINT21
//       PCINT25/TX0 (D 1) PD1 15|        |26  PC4 (D20) TDO/PCINT20
//  PCINT26/INT0/RX1 (D 2) PD2 16|        |25  PC3 (D19) TMS/PCINT19
//  PCINT27/INT1/TX1 (D 3) PD3 17|        |24  PC2 (D18) TCK/PCINT18
//      PCINT28/OC1B*(D 4) PD4 18|        |23  PC1 (D17) SDA/PCINT17
//      PCINT29/OC1A*(D 5) PD5 19|        |22  PC0 (D16) SCL/PCINT16
//      PCINT30/OC2B*(D 6) PD6 20|        |21  PD7 (D 7)*OC2A/PCINT31
//                               +--------+
// * = PWM capable pin
// TOSCn = RTC Crystal pinout
// TCK/TMS/TDO/TDI = JTAG pinout

/*
                         PCICR
   PCINT7-0:   D31-24  : bit 0
   PCINT15-8:  D15- 8  : bit 1
   PCINT23-16: D23-16  : bit 2
   PCINT31-24: D 7- 0  : bit 3
*/

#define NUM_DIGITAL_PINS	(32)
#define NUM_ANALOG_INPUTS	(8)

#define PIN_SPI_SS    (10)
#define PIN_SPI_MOSI  (11)
#define PIN_SPI_MISO  (12)
#define PIN_SPI_SCK   (13)

static const uint8_t SS   = PIN_SPI_SS;
static const uint8_t MOSI = PIN_SPI_MOSI;
static const uint8_t MISO = PIN_SPI_MISO;
static const uint8_t SCK  = PIN_SPI_SCK;

#define PIN_WIRE_SDA  (17)
#define PIN_WIRE_SCL  (16)

static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

#define LED_BUILTIN  (13)

#define PIN_A0   (24)
#define PIN_A1   (25)
#define PIN_A2   (26)
#define PIN_A3   (27)
#define PIN_A4   (28)
#define PIN_A5   (29)
#define PIN_A6   (30)
#define PIN_A7   (31)

static const uint8_t A0 = PIN_A0;
static const uint8_t A1 = PIN_A1;
static const uint8_t A2 = PIN_A2;
static const uint8_t A3 = PIN_A3;
static const uint8_t A4 = PIN_A4;
static const uint8_t A5 = PIN_A5;
static const uint8_t A6 = PIN_A6;
static const uint8_t A7 = PIN_A7;

#define PIN_LINE  (30)
#define PIN_MIC   (31)

static const uint8_t LINE = PIN_LINE;
static const uint8_t MIC = PIN_MIC;

#define analogInputToDigitalPin(p) ( ((p) < NUM_ANALOG_INPUTS) ? ((p) + 24) : -1)

#define analogPinToChannel(p)   ( ((p) >= A0 && (p) <= A7) ? ((p) - 24) : -1 )

#define digitalPinHasPWM(p)     ((p) == 4 || (p) == 5 || (p) == 6 || (p) == 7 || (p) == 9 || (p) == 10 || (p) == 12 || (p) == 13)

#define digitalPinToPCICR(p)    (((p) >= 0 && (p) < NUM_DIGITAL_PINS) ? (&PCICR) : ((uint8_t *)0))

#define digitalPinToPCICRbit(p) (((p) <= 7) ? 3 : (((p) <= 15) ? 1 : (((p) <= 23) ? 2 : 0)))

#define digitalPinToPCMSK(p)    (((p) <= 7) ? (&PCMSK3) : (((p) <= 15) ? (&PCMSK1) : (((p) <= 23) ? (&PCMSK2) : (&PCMSK0) )))

// The special case here, created by keeping the SPI interface on Arduino Uno pin numbering.
#define digitalPinToPCMSKbit(p) (((p) > 7 && (p) < 16) ? (((p) < 14 ? (p)+2 : (p)-6) % 8) : ((p) % 8))

#define digitalPinToInterrupt(p)  ((p) == 2 ? 0 : ((p) == 3 ? 1 : ((p) == 8 ? 2 : NOT_AN_INTERRUPT)))


#ifdef ARDUINO_MAIN

#define PA 1
#define PB 2
#define PC 3
#define PD 4

// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] =
{
	NOT_A_PORT,
	(uint16_t) &DDRA,
	(uint16_t) &DDRB,
	(uint16_t) &DDRC,
	(uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] =
{
	NOT_A_PORT,
	(uint16_t) &PORTA,
	(uint16_t) &PORTB,
	(uint16_t) &PORTC,
	(uint16_t) &PORTD,
};

const uint16_t PROGMEM port_to_input_PGM[] =
{
	NOT_A_PORT,
	(uint16_t) &PINA,
	(uint16_t) &PINB,
	(uint16_t) &PINC,
	(uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] =
{
	PD, /* 0 */
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PD,
	PB, /* 8 */
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PB,
	PC, /* 16 */
	PC,
	PC,
	PC,
	PC,
	PC,
   	PC,
	PC,
	PA, /* 24 */
	PA,
	PA,
	PA,
	PA,
	PA,
	PA,
	PA  /* 31 */
};

const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] =
{
	_BV(0), /* 0, port D */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(2), /* 8, port B */
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0),
	_BV(1),
	_BV(0), /* 16, port C */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7),
	_BV(0), /* 24, port A */
	_BV(1),
	_BV(2),
	_BV(3),
	_BV(4),
	_BV(5),
	_BV(6),
	_BV(7)
};

const uint8_t PROGMEM digital_pin_to_timer_PGM[] =
{
	NOT_ON_TIMER, 	/* 0  - PD0 */
	NOT_ON_TIMER, 	/* 1  - PD1 */
	NOT_ON_TIMER, 	/* 2  - PD2 */
	NOT_ON_TIMER, 	/* 3  - PD3 */
	TIMER1B,     	/* 4  - PD4 */
	TIMER1A,     	/* 5  - PD5 */
	TIMER2B,     	/* 6  - PD6 */
	TIMER2A,     	/* 7  - PD7 */

	NOT_ON_TIMER, 	/* 8  - PB2 */
	TIMER0A,     	/* 9  - PB3 */
	TIMER0B,        /* 10 - PB4 */
	NOT_ON_TIMER, 	/* 11 - PB5 */
	TIMER3A,        /* 12 - PB6 */
	TIMER3B,        /* 13 - PB7 */

	NOT_ON_TIMER, 	/* 14 - PB0 */
	NOT_ON_TIMER, 	/* 15 - PB1 */

	NOT_ON_TIMER, 	/* 16 - PC0 */
	NOT_ON_TIMER,   /* 17 - PC1 */
	NOT_ON_TIMER,   /* 18 - PC2 */
	NOT_ON_TIMER,   /* 19 - PC3 */
	NOT_ON_TIMER,   /* 20 - PC4 */
	NOT_ON_TIMER,   /* 21 - PC5 */
	NOT_ON_TIMER,   /* 22 - PC6 */
	NOT_ON_TIMER,   /* 23 - PC7 */
	
	NOT_ON_TIMER,   /* 24 - PA0 */
	NOT_ON_TIMER,   /* 25 - PA1 */
	NOT_ON_TIMER,   /* 26 - PA2 */
	NOT_ON_TIMER,   /* 27 - PA3 */
	NOT_ON_TIMER,   /* 28 - PA4 */
	NOT_ON_TIMER,   /* 29 - PA5 */
	NOT_ON_TIMER,   /* 30 - PA6 */
	NOT_ON_TIMER    /* 31 - PA7 */
};

#endif // ARDUINO_MAIN

#endif // Pins_Arduino_h

