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

#include "new.h"

// The C++ spec dictates that allocation failure should cause the
// (non-nothrow version of the) operator new to throw an exception.
// Since we expect to have exceptions disabled, it would be more
// appropriate (and probably standards-compliant) to terminate instead.
// Historically failure causes null to be returned, but this define
// allows switching to more robust terminating behaviour (that might
// become the default at some point in the future). Note that any code
// that wants null to be returned can (and should) use the nothrow
// versions of the new statement anyway and is unaffected by this.
// #define NEW_TERMINATES_ON_FAILURE

namespace std {
  // Defined in abi.cpp
  void terminate();

  const nothrow_t nothrow;
}

static void * new_helper(std::size_t size) {
  // Even zero-sized allocations should return a unique pointer, but
  // malloc does not guarantee this
  if (size == 0)
    size = 1;
  return malloc(size);
}

void * operator new(std::size_t size) {
  void *res = new_helper(size);
#if defined(NEW_TERMINATES_ON_FAILURE)
  if (!res)
    std::terminate();
#endif
  return res;
}
void * operator new[](std::size_t size) {
  return operator new(size);
}

void * operator new(std::size_t size, const std::nothrow_t tag) noexcept {
#if defined(NEW_TERMINATES_ON_FAILURE)
  // Cannot call throwing operator new as standard suggests, so call
  // new_helper directly then
  return new_helper(size);
#else
  return operator new(size);
#endif
}
void * operator new[](std::size_t size, const std::nothrow_t& tag) noexcept {
#if defined(NEW_TERMINATES_ON_FAILURE)
  // Cannot call throwing operator new[] as standard suggests, so call
  // malloc directly then
  return new_helper(size);
#else
  return operator new[](size);
#endif
}

void * operator new(std::size_t size, void *place) noexcept {
  // Nothing to do
  (void)size; // unused
  return place;
}
void * operator new[](std::size_t size, void *place) noexcept {
  return operator new(size, place);
}

void operator delete(void * ptr) noexcept {
  free(ptr);
}
void operator delete[](void * ptr) noexcept {
  operator delete(ptr);
}

#if __cplusplus >= 201402L
void operator delete(void* ptr, std::size_t size) noexcept {
  operator delete(ptr);
}
void operator delete[](void * ptr, std::size_t size) noexcept {
  operator delete[](ptr);
}
#endif // __cplusplus >= 201402L

void operator delete(void* ptr, const std::nothrow_t& tag) noexcept {
  operator delete(ptr);
}
void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept {
  operator delete[](ptr);
}

void operator delete(void* ptr, void* place) noexcept {
  (void)ptr; (void)place; // unused
  // Nothing to do
}
void operator delete[](void* ptr, void* place) noexcept {
  (void)ptr; (void)place; // unused
  // Nothing to do
}
