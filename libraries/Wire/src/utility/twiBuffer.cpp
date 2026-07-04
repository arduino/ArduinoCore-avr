/*
  twiBuffer.cpp - TWI/I2C library for Arduino & Wiring
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
*/

#include <stdint.h>
#include <stddef.h>
#include "twiBuffer.h"

// Default buffers as weak buffers
namespace twiBuffer { \
  extern __attribute__((weak)) const bufferSize_t TWI_MASTER_BUFFER_SIZE = TWI_DEFAULT_BUFFER_SIZE;
  extern __attribute__((weak)) const bufferSize_t TWI_TX_BUFFER_SIZE = TWI_DEFAULT_BUFFER_SIZE;
  extern __attribute__((weak)) const bufferSize_t TWI_RX_BUFFER_SIZE = TWI_DEFAULT_BUFFER_SIZE;
  __attribute__((weak)) uint8_t twi_masterBuffer[TWI_DEFAULT_BUFFER_SIZE];
  __attribute__((weak)) uint8_t twi_txBuffer[TWI_DEFAULT_BUFFER_SIZE];
  __attribute__((weak))  uint8_t twi_rxBuffer[TWI_DEFAULT_BUFFER_SIZE];
}

