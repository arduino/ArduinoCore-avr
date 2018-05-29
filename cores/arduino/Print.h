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

template <typename Check, typename T>
struct check_type {
  using type = T;
};
template <typename Check, typename T> using check_type_t = typename check_type<Check, T>::type;

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
  
    struct Formatter { };

    struct FormatterOptionBase { };
    template <typename TFormatter, typename TValue>
    struct FormatterOption : FormatterOptionBase {
      // Is this the right place to define this? Or perhaps just move
      // down?
      using Formatter = TFormatter;
      TValue value;
      constexpr FormatterOption(const TValue value) : value(value) { }
    };

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

    template<typename T, typename F>
    _always_inline auto doPrint(T v, F f )
    -> check_type_t<decltype(f.printTo(this, v)), size_t> {
      return f.printTo(this, v);
    }
 
    size_t println(void);

    virtual void flush() { /* Empty implementation for backward compatibility */ }
    /*
    template<typename T, typename ...Ts>
    _always_inline size_t print(const T &arg, const Ts &...args) {
      // This might lead to infinite template instantion
      //return print(arg, DefaultFormatter<>(), args...);
      size_t n = DefaultFormatter<>().printTo(this, arg);
      return n + print(args...);
    }

    template<typename T, typename T2, typename ...Ts>
    _always_inline auto print(const T &arg, const T2 &arg2, const Ts &...args)
    -> check_type_t<decltype(arg2.printTo(this, arg)), size_t> {
      size_t n = arg2.printTo(this, arg);
      return n + print(args...);
    }

    template<typename T, typename T2, typename T3, typename ...Ts>
    _always_inline auto print(const T &arg, const T2 &arg2, const T3 &arg3, const Ts &...args)
    -> check_type_t<decltype(arg2.addOption(arg3)), size_t> {
      return print(arg, arg2.addOption(arg3), args...);
    }

    template<typename T, typename T2, typename ...Ts>
    _always_inline auto print(const T &arg, const T2 &arg2, const Ts &...args)
    -> check_type_t<decltype(DefaultFormatter<>().addOption(arg2)), size_t> {
      return print(arg, DefaultFormatter<>().addOption(arg2), args...);
    }
    */

#if __cplusplus >= 201103L
    template<typename ...Ts> _always_inline size_t print(const Ts &...args);
    template<typename ...Ts> _always_inline size_t println(const Ts &...args) { size_t t = print(args...); return t + println(); }
#else
    template<typename T> _always_inline size_t println(const T &arg)      { size_t t = print(arg);  return t + println(); }
    template<typename T, typename T2> _always_inline size_t println(const T &arg1, const T2& arg2) { size_t t = print(arg1, arg2); return t + println(); }
#endif // __cplusplus >= 201103L

    //_always_inline size_t print() { return 0; }

    /** Variadic methods **/
#if __cplusplus >= 201103L  // requires C++11
/*
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
*/

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

class DefaultFormatter;

// TODO: Do we really need a FormatterOption base class? Without it,
// options can be POD, not needing a constructor. With it, we can
// prevent accidentally treating things as options which are not, but if
// we already check a Formatter base class, we can also require that
// Formatters do not define nonsensical addOption methods (even more,
// without an explicit FormatterOption, Formatters can even use
// non-class types as options if they want).
// TODO: Where to define the "default formatter" for an option? Now, it
// is our superclass, but it might just as well be defined with a using
// directive directly here. Or perhaps it should be a method (this
// might be problematic, since the DefaultFormatter type is incomplete
// at this point). One completely different approach would be a
// DefaultFormatterFor template, which gets specialized, but this
// probably has the problem that once it is instantiated, you can no
// longer add to it. Using specializations does allow defining a default
// formatter for a type/option combination (allowing reuse of e.g. HEX
// for custom types) or for just a type (allowing default formatting of
// custom types).
struct FormatOptionBase : Print::FormatterOption<DefaultFormatter, uint8_t> {
  // TODO: We must provide an explicit constructor (if we have a
  // superclass, we are no longer a POD type), and if we do, there is no
  // real point in storing our value in the superclass (better have one
  // extra line here, but more explictness. Inheriting the constructor
  // is even more ugly (needs to repeat superclass template arguments).
  constexpr FormatOptionBase(uint8_t value) : Print::FormatterOption<DefaultFormatter, uint8_t>(value) { }
};
struct FormatOptionMinWidth : Print::FormatterOption<DefaultFormatter, uint8_t> {
  constexpr FormatOptionMinWidth(uint8_t value) : Print::FormatterOption<DefaultFormatter, uint8_t>(value) { }
};

#undef HEX
inline constexpr FormatOptionMinWidth PRINT_MIN_WIDTH(uint8_t min_width) { return {min_width}; }
inline constexpr FormatOptionBase PRINT_BASE(uint8_t base) { return {base}; }
constexpr FormatOptionBase HEX = PRINT_BASE(16);

struct DefaultFormatter : Print::Formatter {
  uint8_t base;
  uint8_t min_width;

  DefaultFormatter(uint8_t base = 10, uint8_t min_width = 0)
    : base(base), min_width(min_width)
  { }

  DefaultFormatter addOption(FormatOptionBase o) const {
    return {o.value, this->min_width};
  }

  DefaultFormatter addOption(FormatOptionMinWidth o) const {
    return {this->base, o.value};
  }

  size_t printTo(Print *p, int n) const;
  size_t printTo(Print *p, const char *) const;
  template <typename T>
  size_t printTo(Print *p, const T&) const;
};

// TODO: These can probably be moved back inline above (they were split
// when PrintHelper did not exist, so the Print class was defined
// between the declaration of DefaultFormatter and these method
// definitions).
inline size_t DefaultFormatter::printTo(Print *p, int n) const {
  return p->doPrint(n, this->base);
}

inline size_t DefaultFormatter::printTo(Print *p, const char * s) const {
  // TODO: Maybe replace bool has_int_options with a more detailed bitwise flag int
  // for better error messages.
  //static_assert(!has_int_options, "Cannot print strings with integer-specific formatting options");
  p->doPrint(s);
  return 0;
}

template <typename T>
inline size_t DefaultFormatter::printTo(Print *p, const T& v) const {
  return p->doPrint(v);
}

// TODO: Should we really need PrintHelper? It was now added allow
// referencing DefaultFormatter above. These methods could just be
// out-of-line definitions of methods in Print, but then the complicated
// method signature must be duplicated. Alternatively, a single method
// that generates a DefaultFormatter object could possibly be out of
// line (though, thinking on it, both of these options might not work,
// since they need DefaultFormatter as a return type in a
// declaration...).
class PrintHelper {
  public:
    template<typename T, typename ...Ts>
    static _always_inline size_t printTo(Print * p, const T &arg, const Ts &...args) {
      // This might lead to infinite template instantion
      //return print(arg, DefaultFormatter<>(), args...);
      size_t n = DefaultFormatter().printTo(p, arg);
      return n + printTo(p, args...);
    }
/*
    template<typename T, typename T2, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &arg2, const Ts &...args)
    -> check_type_t<decltype(arg2.printTo(p, arg)), size_t> {
      size_t n = arg2.printTo(p, arg);
      return n + printTo(p, args...);
    }
*/
    static void accepts_formatter(const Print::Formatter*);

    template<typename T, typename T2, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &arg2, const Ts &...args)
    -> check_type_t<decltype(accepts_formatter(&arg2)), size_t> {
    //-> check_type_t<decltype(arg2.printTo(p, arg)), size_t> {
      size_t n = arg2.printTo(p, arg);
      return n + printTo(p, args...);
    }


    template<typename T, typename T2, typename T3, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &arg2, const T3 &arg3, const Ts &...args)
    -> check_type_t<decltype(arg2.addOption(arg3)), size_t> {
      return printTo(p, arg, arg2.addOption(arg3), args...);
    }

    static void accepts_option(const Print::FormatterOptionBase*);

    template<typename T, typename T2, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &arg2, const Ts &...args)
    //-> check_type_t<decltype(typename T2::Formatter().addOption(arg2)), size_t> {
    -> check_type_t<decltype(accepts_option(&arg2)), size_t> {
      //accepts_option(&arg2);
      return printTo(p, arg, typename T2::Formatter().addOption(arg2), args...);
    }

    static _always_inline size_t printTo(Print *) { return 0; }
};

template<typename ...Ts>
_always_inline size_t Print::print(const Ts &...args) { return PrintHelper::printTo(this, args...); }
#undef _always_inline

#endif

// Idea: Raise errors when DefaultFormatter is used with int-only
// options (e.g. integer base), but prints a string or otherwise
// incompatible type. This can be done by extending DefaultFormatter
// with a bitmask or bool template argument(s) that track what options
// have been set, so they can be static_asserted against in specific
// printTo versions. This does require that *all* methods in
// DefaultFormatter are always_inline, to prevent duplicates. This means
// the actual printing code must again live elsewhere, which might not
// be ideal. Also, error messages are then complicated with these
// template arguments. This can be solved by doing just the checks and
// forwarding *all* printTo calls to another class (including a
// catch-all template), so that when you try printing any unsupported
// types you still get the proper "no such method to call, candiates
// are..." error message.
