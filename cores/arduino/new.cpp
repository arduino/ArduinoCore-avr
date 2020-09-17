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

namespace std {
  const nothrow_t nothrow;
}

void * operator new(size_t size) {
  return malloc(size);
}
void * operator new[](size_t size) {
  return operator new(size);
}

void * operator new(size_t size, const std::nothrow_t tag) noexcept {
  return operator new(size);
}
void * operator new[](size_t size, const std::nothrow_t& tag) noexcept {
  return operator new[](size);
}

void * operator new(size_t size, void *place) noexcept {
  // Nothing to do
  (void)size; // unused
  return place;
}
void * operator new[](size_t size, void *place) noexcept {
  return operator new(size, place);
}

void operator delete(void * ptr) noexcept {
  free(ptr);
}
void operator delete[](void * ptr) noexcept {
  operator delete(ptr);
}

#if __cplusplus >= 201402L
void operator delete(void* ptr, size_t size) noexcept {
  operator delete(ptr);
}
void operator delete[](void * ptr, size_t size) noexcept {
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
