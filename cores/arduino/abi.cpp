/*
  Copyright (c) 2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <abi.h>
#include <stdint.h>
#include <exception>

#include <avr/io.h>
#include <avr/interrupt.h>

namespace {
// guard is an integer type big enough to hold flag and a mutex.
// By default gcc uses long long int and avr ABI does not change it
// So we have 32 or 64 bits available. Actually, we need 16.

inline char& flag_part(__guard *g) {
    return *(reinterpret_cast<char*>(g));
}

inline uint8_t& sreg_part(__guard *g) {
    return *(reinterpret_cast<uint8_t*>(g) + sizeof(char));
}
}

int __cxa_guard_acquire(__guard *g) {
    uint8_t oldSREG = SREG;
    cli();
    // Initialization of static variable has to be done with blocked interrupts
    // because if this function is called from interrupt and sees that somebody
    // else is already doing initialization it MUST wait until initializations
    // is complete. That's impossible.
    // If you don't want this overhead compile with -fno-threadsafe-statics
    if (flag_part(g)) {
        SREG = oldSREG;
        return false;
    } else {
        sreg_part(g) = oldSREG;
        return true;
    }
}

void __cxa_guard_release (__guard *g) {
    flag_part(g) = 1;
    SREG = sreg_part(g);
}

void __cxa_guard_abort (__guard *g) {
    SREG = sreg_part(g);
}

void __cxa_pure_virtual(void) {
    // We might want to write some diagnostics to uart in this case
    std::terminate();
}

void __cxa_deleted_virtual(void) {
    // We might want to write some diagnostics to uart in this case
    std::terminate();
}