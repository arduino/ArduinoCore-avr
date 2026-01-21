/*
  Wire.h - TWI/I2C library for Arduino & Wiring
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

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
*/

#pragma once

#ifndef TwoWire_h
#define TwoWire_h

#include <inttypes.h>
#include "Stream.h"

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1

class TwoWire : public Stream
{
  private:
    static uint8_t rxBufferIndex;
    static uint8_t rxBufferLength;

    static uint8_t txAddress;
    static uint8_t txBufferIndex;
    static uint8_t txBufferLength;

    static uint8_t transmitting;
    static void (*user_onRequest)(void);
    static void (*user_onReceive)(int);
    static void onRequestService(void);
    static void onReceiveService(uint8_t*, int);
  public:
    TwoWire();
    void begin();
    void begin(uint8_t);
    void begin(int);
    void end();
    void setClock(uint32_t);
    void setWireTimeout(uint32_t timeout = 25000, bool reset_with_timeout = false);
    bool getWireTimeoutFlag(void);
    void clearWireTimeoutFlag(void);
    void beginTransmission(uint8_t);
    inline void beginTransmission(int address) {beginTransmission(static_cast<uint8_t> (address));}
    uint8_t endTransmission(void);
    uint8_t endTransmission(uint8_t);
    uint8_t requestFrom(uint8_t, uint8_t, uint32_t, uint8_t, uint8_t);
    inline uint8_t requestFrom(uint8_t address, uint8_t quantity) {
      return requestFrom(static_cast<uint8_t>(address), static_cast<uint8_t>(quantity),
          static_cast<uint8_t> (true));
    }
    inline uint8_t requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
      return requestFrom(static_cast<uint8_t>(address), static_cast<uint8_t>(quantity),
          static_cast<uint32_t>(0), static_cast<uint8_t>(0), static_cast<uint8_t>(sendStop));
    }
    inline uint8_t requestFrom(int address, int quantity) {
      return requestFrom(static_cast<uint8_t>(address), static_cast<uint8_t>(quantity),
          static_cast<uint8_t>(true));
    }
    inline uint8_t requestFrom(int address, int quantity, int sendStop) {
      return requestFrom(static_cast<uint8_t>(address), static_cast<uint8_t>(quantity),
          static_cast<uint8_t>(sendStop));
    }
    size_t write(uint8_t) override;
    size_t write(const uint8_t *, size_t) override;
    int available(void) override;
    int read(void) override;
    int peek(void) override;
    void flush(void) override;
    void onReceive( void (*)(int) );
    void onRequest( void (*)(void) );

    inline size_t write(unsigned long n) { return write((uint8_t)n); }
    inline size_t write(long n) { return write((uint8_t)n); }
    inline size_t write(unsigned int n) { return write((uint8_t)n); }
    inline size_t write(int n) { return write((uint8_t)n); }
    using Print::write;
};

extern TwoWire Wire;

#endif

