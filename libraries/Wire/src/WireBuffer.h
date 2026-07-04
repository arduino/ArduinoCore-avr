/*
  WireBuffer.h - TWI/I2C library for Arduino & Wiring
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

#pragma once

#ifndef Wire_WireBuffer_h_
#define Wire_WireBuffer_h_

#include <stdint.h>
#include <stddef.h>
#include "utility/twiBuffer.h"

// Use extra namespace to avoid collision with other symbols
namespace WireBuffer {
  // Declare twi buffers
  typedef size_t bufferSize_t;
  constexpr bufferSize_t WIRE_BUFFER_DEFAULT_SIZE = twiBuffer::TWI_DEFAULT_BUFFER_SIZE;
  extern  const bufferSize_t RX_BUFFER_SIZE;
  extern  const bufferSize_t TX_BUFFER_SIZE;
  extern uint8_t rxBuffer[];
  extern uint8_t txBuffer[];
}

#define SET_Wire_BUFFERS(rxBufferSize, txBufferSize, enableMaster, enableSlave) \
    namespace twiBuffer { \
      const bufferSize_t TWI_MASTER_BUFFER_SIZE = sizes<rxBufferSize, txBufferSize, enableMaster, enableSlave>::M; \
      const bufferSize_t TWI_RX_BUFFER_SIZE = sizes<rxBufferSize, txBufferSize, enableMaster, enableSlave>::RX; \
      const bufferSize_t TWI_TX_BUFFER_SIZE = sizes<rxBufferSize, txBufferSize, enableMaster, enableSlave>::TX; \
      uint8_t twi_masterBuffer[TWI_MASTER_BUFFER_SIZE]; \
      uint8_t twi_rxBuffer[TWI_RX_BUFFER_SIZE]; \
      uint8_t twi_txBuffer[TWI_TX_BUFFER_SIZE]; \
    } \
    namespace WireBuffer { \
      const bufferSize_t TX_BUFFER_SIZE = txBufferSize; \
      const bufferSize_t RX_BUFFER_SIZE = rxBufferSize; \
      uint8_t rxBuffer[rxBufferSize]; \
      uint8_t txBuffer[txBufferSize]; \
    }


#endif /* Wire_WireBuffer_h_ */
