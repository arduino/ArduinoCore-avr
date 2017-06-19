/*
  Print.h - Base class that provides print() and println()
  Copyright (c) 2008 David A. Mellis.  All right reserved.

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

#ifndef Print_h
#define Print_h

#include <inttypes.h>
#include <stdio.h> // for size_t

#include "WString.h"
#include "Printable.h"

#define DEC 10
#define HEX 16
#define OCT 8
#ifdef BIN // Prevent warnings if BIN is previously defined in "iotnx4.h" or similar
#undef BIN
#endif
#define BIN 2

#define _always_inline __attribute__ ((__always_inline__)) // undefined at end

class Print
{
  private:
    int write_error;
    size_t printNumber(unsigned long, uint8_t);
    size_t printFloat(double, uint8_t);
  protected:
    void setWriteError(int err = 1) { write_error = err; }
  public:
    Print() : write_error(0) {}
  
    int getWriteError() { return write_error; }
    void clearWriteError() { setWriteError(0); }
  
    virtual size_t write(uint8_t) = 0;
    size_t write(const char *str) {
      if (str == NULL) return 0;
      return write((const uint8_t *)str, strlen(str));
    }
    virtual size_t write(const uint8_t *buffer, size_t size);
    size_t write(const char *buffer, size_t size) {
      return write((const uint8_t *)buffer, size);
    }


    // default to zero, meaning "a single write may block"
    // should be overriden by subclasses with buffering
    virtual int availableForWrite() { return 0; }

    size_t doPrint(const __FlashStringHelper *);
    size_t doPrint(const String &);
    size_t doPrint(  signed long, int = DEC);
    size_t doPrint(unsigned long, int = DEC);
    size_t doPrint(double, int = 2);
    
    _always_inline size_t doPrint(const char str[])   { return write(str); }
    _always_inline size_t doPrint(const char c)       { return write(c); }
    _always_inline size_t doPrint(const Printable &x) { return x.printTo(*this); }
    
    _always_inline size_t doPrint(  signed char  n, int f = DEC) { return doPrint((  signed long) n, f); }
    _always_inline size_t doPrint(  signed short n, int f = DEC) { return doPrint((  signed long) n, f); }
    _always_inline size_t doPrint(  signed int   n, int f = DEC) { return doPrint((  signed long) n, f); }
    _always_inline size_t doPrint(unsigned char  n, int f = DEC) { return doPrint((unsigned long) n, f); }
    _always_inline size_t doPrint(unsigned short n, int f = DEC) { return doPrint((unsigned long) n, f); }
    _always_inline size_t doPrint(unsigned int   n, int f = DEC) { return doPrint((unsigned long) n, f); }
    _always_inline size_t doPrint(    float      n, int f = 2  ) { return doPrint((    double   ) n, f); }

    template <typename Check, typename T>
    struct check_type {
      using type = T;
    };
    template <typename Check, typename T> using check_type_t = typename check_type<Check, T>::type;

    template<typename T, typename F>
    _always_inline auto doPrint(T v, F f )
    -> check_type_t<decltype(f.printTo(this, v)), size_t> {
      return f.printTo(this, v);
    }
 
    size_t println(void);

    virtual void flush() { /* Empty implementation for backward compatibility */ }

#if __cplusplus >= 201103L
    template<typename ...Ts> _always_inline size_t println(const Ts &...args) { size_t t = print(args...); return t + println(); }
#else
    template<typename T> _always_inline size_t println(const T &arg)      { size_t t = print(arg);  return t + println(); }
    template<typename T, typename T2> _always_inline size_t println(const T &arg1, const T2& arg2) { size_t t = print(arg1, arg2); return t + println(); }
#endif // __cplusplus >= 201103L

    _always_inline size_t print() { return 0; }

    /** Variadic methods **/
#if __cplusplus >= 201103L  // requires C++11
    template<typename T, typename ...Ts>
    _always_inline size_t print(const T &arg, const Ts &...args) {
      size_t t = doPrint(arg);
      return t + print(args...);
    }

    template<typename T, typename T2, typename ...Ts>
    _always_inline auto print(const T &arg, const T2 &arg2, const Ts &...args)
    -> check_type_t<decltype(doPrint(arg, arg2)), size_t> {
      size_t t = doPrint(arg, arg2);
      return t + print(args...);
    }
/*
    // Some methods take an extra int parameter.  If so, use these templates.
    // In a future, it would be nice to make the base/precision a special type.
    template<typename ...Ts> _always_inline size_t print(  signed char  n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(  signed short n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(  signed int   n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(  signed long  n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(unsigned char  n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(unsigned short n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(unsigned int   n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(unsigned long  n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(    float      n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
    template<typename ...Ts> _always_inline size_t print(    double     n, int f, const Ts &...args) { size_t t = print(n, f); return t + print(args...); }
*/
#else
    template<typename T> _always_inline size_t print(const T &arg)      { return doPrint(arg); }
    template<typename T, typename T2> _always_inline size_t print(const T &arg1, const T2& arg2) { return doPrint(arg1, arg2); }
#endif // __cplusplus >= 201103L
};

#undef _always_inline

#endif
