/*
  twi.h - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
*/

#pragma once

#ifndef twi_h
#define twi_h

  #include <inttypes.h>
  //#define ATMEGA8

  void twi_init(void);
  void twi_disable(void);
  void twi_setAddress(uint8_t);
  void twi_setFrequency(uint32_t);
  uint8_t twi_readFrom(uint8_t, uint8_t*, uint8_t, uint8_t);
  uint8_t twi_writeTo(uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t);
  uint8_t twi_transmit(const uint8_t*, uint8_t);
  void twi_attachSlaveRxEvent( void (*)(uint8_t*, int) );
  void twi_attachSlaveTxEvent( void (*)(void) );
  void twi_reply(uint8_t);
  void twi_stop(void);
  void twi_releaseBus(void);
  void twi_setTimeoutInMicros(uint32_t, bool);
  void twi_handleTimeout(bool);
  bool twi_manageTimeoutFlag(bool);

#if DEBUG_TWI_BUFFER
  size_t twi_getMasterBufferSize();
  size_t twi_getMasterBuffer();
  size_t twi_getRxBufferSize();
  size_t twi_getRxBuffer();
  size_t twi_getTxBufferSize();
  size_t twi_getTxBuffer();
#endif

#endif
