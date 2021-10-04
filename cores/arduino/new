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

#ifndef NEW_H
#define NEW_H

#include <stdlib.h>

namespace std {
  struct nothrow_t {};
  extern const nothrow_t nothrow;

  // These are not actually implemented, to prevent overhead and
  // complexity. They are still declared to allow implementing
  // them in user code if needed.
  typedef void (*new_handler)();
  new_handler set_new_handler(new_handler new_p) noexcept;
  new_handler get_new_handler() noexcept;

  // This is normally declared in various headers that we do not have
  // available, so just define it here. We could also use ::size_t
  // below, but then anyone including <new> can no longer assume
  // std::size_t is available.
  using size_t = ::size_t;
} // namespace std

[[gnu::weak]] void * operator new(std::size_t size);
[[gnu::weak]] void * operator new[](std::size_t size);

[[gnu::weak]] void * operator new(std::size_t size, const std::nothrow_t tag) noexcept;
[[gnu::weak]] void * operator new[](std::size_t size, const std::nothrow_t& tag) noexcept;

void * operator new(std::size_t size, void *place) noexcept;
void * operator new[](std::size_t size, void *place) noexcept;

[[gnu::weak]] void operator delete(void * ptr) noexcept;
[[gnu::weak]] void operator delete[](void * ptr) noexcept;

#if __cplusplus >= 201402L
[[gnu::weak]] void operator delete(void* ptr, std::size_t size) noexcept;
[[gnu::weak]] void operator delete[](void * ptr, std::size_t size) noexcept;
#endif // __cplusplus >= 201402L

[[gnu::weak]] void operator delete(void* ptr, const std::nothrow_t& tag) noexcept;
[[gnu::weak]] void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept;

void operator delete(void* ptr, void* place) noexcept;
void operator delete[](void* ptr, void* place) noexcept;

#endif

