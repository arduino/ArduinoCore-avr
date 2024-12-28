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

#ifndef TwiBuffers_h
#define TwiBuffers_h

#include <stdint.h>
#include <stddef.h>
#include "Wire.h"

namespace TwoWireBuffers {

/* Template class that implements an compile time fixed size array. */
template<unsigned CAPACITY>
class StaticBuffer {
  uint8_t mByteArray[CAPACITY];
public:
  inline uint8_t capacity() const {return CAPACITY;}
  inline uint8_t* storage() {return mByteArray;}
};

/* Specialization of StaticBuffer template class with zero size. */
template<>
class StaticBuffer<0> {
public:
  inline uint8_t capacity() const {return 0;}
  inline uint8_t* storage() {return nullptr;}
};


/* Interface that provides buffers for twi driver and TwoWire objects */
class Interface {
public:
  virtual uint8_t* twi_masterBuffer() = 0;
  virtual size_t twi_masterBufferCapacity() = 0;

  virtual uint8_t* twi_rxBuffer() = 0;
  virtual size_t twi_rxBufferCapacity() = 0;

  virtual uint8_t* twi_txBuffer() = 0;
  virtual size_t twi_txBufferCapacity() = 0;

  virtual uint8_t* rxWireBuffer() = 0;
  virtual size_t rxWireBufferCapacity() = 0;

  virtual uint8_t* txWireBuffer() = 0;
  virtual size_t txWireBufferCapacity() = 0;
};

/* Template class implementing Interface with template parameter
 * determined buffer sizes.
 */
template<
  size_t RX_CAPACITY, // Receive buffer size. May be zero, if only transmitting data is needed
  size_t TX_CAPACITY, // Transmit buffer size. May be zero, if only receiving data is needed
  bool ENABLE_MASTER,   // If master is disabled, it will save twi master buffer space
  bool ENABLE_SLAVE     // If slave is disabled, it will save twi slave buffer space
  >
class Impl : public Interface {
  static_assert(ENABLE_MASTER == true || ENABLE_SLAVE == true,
      "You should not disable master and slave together.");

  static constexpr size_t TWI_MASTER_BUFFER_CAPACITY =
      RX_CAPACITY > TX_CAPACITY ? RX_CAPACITY : TX_CAPACITY;

  // Set the twi master buffer capacity to 0, if master mode isn't needed.
  TwoWireBuffers::StaticBuffer<ENABLE_MASTER ? TWI_MASTER_BUFFER_CAPACITY : 0> mTwiMasterBuffer;

  // Set the twi slave buffers capacity to 0, if slave mode isn't needed.
  TwoWireBuffers::StaticBuffer<ENABLE_SLAVE ? RX_CAPACITY : 0> mTwiRxBuffer;
  TwoWireBuffers::StaticBuffer<ENABLE_SLAVE ? TX_CAPACITY : 0> mTwiTxBuffer;

  // Set the capacity for a TwoWire object.
  TwoWireBuffers::StaticBuffer<RX_CAPACITY> mRxWireBuffer;
  TwoWireBuffers::StaticBuffer<TX_CAPACITY> mTxWireBuffer;

public:
  virtual uint8_t* twi_masterBuffer() override {return mTwiMasterBuffer.storage();}
  virtual size_t twi_masterBufferCapacity() override {return mTwiMasterBuffer.capacity();}

  virtual uint8_t* twi_rxBuffer() override {return mTwiRxBuffer.storage();}
  virtual size_t twi_rxBufferCapacity() override {return mTwiRxBuffer.capacity();}

  virtual uint8_t* twi_txBuffer() override {return mTwiTxBuffer.storage();}
  virtual size_t twi_txBufferCapacity() override {return mTwiTxBuffer.capacity();}

  virtual uint8_t* rxWireBuffer() override {return mRxWireBuffer.storage();}
  virtual size_t rxWireBufferCapacity() override {return mRxWireBuffer.capacity();}

  virtual uint8_t* txWireBuffer() override {return mTxWireBuffer.storage();}
  virtual size_t txWireBufferCapacity() override {return mTxWireBuffer.capacity();}
};

} // namespace TwoWireBuffers

// For avr there is currently only a single Wire object that is associated with wireNum 0
template<size_t wireNum> struct WireBuffers { // The buffers for the Wire object
  static TwoWireBuffers::Interface& instance();
};

#define SET_WIRE_BUFFERS(rxBufferCapacity, txBufferCapacity, enableMaster, enableSlave) \
    template<> TwoWireBuffers::Interface& WireBuffers<0>::instance() { \
      static TwoWireBuffers::Impl<rxBufferCapacity, txBufferCapacity, enableMaster, enableSlave> buffers; \
      return buffers; \
    }

#define GET_WIRE_BUFFERS() \
    WireBuffers<0>::instance();

#endif /* TwiBuffers_h */
