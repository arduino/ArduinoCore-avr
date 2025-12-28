/*
  twiBuffer.h - TWI/I2C library for Arduino & Wiring
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

#ifndef twi_twiBuffer_h_
#define twi_twiBuffer_h_

#include <stdint.h>
#include <stddef.h>

// Use extra namespace to avoid collision with other symbols
namespace twiBuffer {
  typedef size_t bufferSize_t;

  // Template struct calculating twi buffer sizes.
  template<
    size_t RX_SIZE, // Receive buffer size. May be zero, if only transmitting data is needed
    size_t TX_SIZE, // Transmit buffer size. May be zero, if only receiving data is needed
    bool ENABLE_MASTER,   // If master is disabled, it will save twi master buffer space
    bool ENABLE_SLAVE     // If slave is disabled, it will save twi slave buffer space
    >
  struct sizes {
    static_assert(ENABLE_MASTER == true || ENABLE_SLAVE == true, "You should not disable master and slave together.");

    // twi master capacity becomes max of RX_SIZE, TX_SIZE
    static constexpr twiBuffer::bufferSize_t TWI_MASTER_BUFFER_SIZE = RX_SIZE > TX_SIZE ? RX_SIZE : TX_SIZE;

    // Set the twi master buffer capacity to 0, if master mode isn't needed.
    static constexpr twiBuffer::bufferSize_t M = ENABLE_MASTER ? TWI_MASTER_BUFFER_SIZE : 0;

    // Set the twi slave buffers capacity to 0, if slave mode isn't needed.
    static constexpr twiBuffer::bufferSize_t RX = ENABLE_SLAVE ? RX_SIZE : 0;
    static constexpr twiBuffer::bufferSize_t TX = ENABLE_SLAVE ? TX_SIZE : 0;
  };

  // Declare twi buffers
  constexpr bufferSize_t TWI_DEFAULT_BUFFER_SIZE = 32;
  extern uint8_t twi_masterBuffer[];
  extern const bufferSize_t TWI_MASTER_BUFFER_SIZE;
  extern uint8_t twi_rxBuffer[];
  extern const bufferSize_t TWI_RX_BUFFER_SIZE;
  extern uint8_t twi_txBuffer[];
  extern const bufferSize_t TWI_TX_BUFFER_SIZE;
}

#endif twi_twiBuffer_h_
