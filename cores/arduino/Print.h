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
struct enable_if_valid {
  using type = T;
};
template <typename Check, typename T> using enable_if_valid_t = typename enable_if_valid<Check, T>::type;

class Print
{
  private:
    int write_error;
  protected:
    void setWriteError(int err = 1) { write_error = err; }
  public:
    Print() : write_error(0) {}
  
    // TODO: Move these into the Formatters namespace?
    // TODO: Shorten / rethink these names?
    struct Formatter { };
    struct FormatterOption { };

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
    -> enable_if_valid_t<decltype(arg2.printTo(this, arg)), size_t> {
      size_t n = arg2.printTo(this, arg);
      return n + print(args...);
    }

    template<typename T, typename T2, typename T3, typename ...Ts>
    _always_inline auto print(const T &arg, const T2 &arg2, const T3 &arg3, const Ts &...args)
    -> enable_if_valid_t<decltype(arg2.addOption(arg3)), size_t> {
      return print(arg, arg2.addOption(arg3), args...);
    }

    template<typename T, typename T2, typename ...Ts>
    _always_inline auto print(const T &arg, const T2 &arg2, const Ts &...args)
    -> enable_if_valid_t<decltype(DefaultFormatter<>().addOption(arg2)), size_t> {
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
    -> enable_if_valid_t<decltype(doPrint(arg, arg2)), size_t> {
      size_t t = doPrint(arg, arg2);
      return t + print(args...);
    }
*/

    // Compatibility versions of print that take a second base or
    // precision argument that is an integer.
    _always_inline size_t print(  signed char , int);
    _always_inline size_t print(  signed short, int);
    _always_inline size_t print(  signed int  , int);
    _always_inline size_t print(  signed long , int);
    _always_inline size_t print(unsigned char , int);
    _always_inline size_t print(unsigned short, int);
    _always_inline size_t print(unsigned int  , int);
    _always_inline size_t print(unsigned long , int);
    _always_inline size_t print(    float     , int);
    _always_inline size_t print(    double    , int);
#else
    template<typename T> _always_inline size_t print(const T &arg)      { return doPrint(arg); }
    template<typename T, typename T2> _always_inline size_t print(const T &arg1, const T2& arg2) { return doPrint(arg1, arg2); }
#endif // __cplusplus >= 201103L
};

void accepts_formatter(const Print::Formatter*);
void accepts_option(const Print::FormatterOption*);

// Namespace for more advanced custom formatter stuff, to prevent
// cluttering the global namespace. Could be removed if needed.
namespace Formatters {

  
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

class DefaultFormatter : public Print::Formatter {
  public:
    // Common base class for our options
    struct FormatterOption : Print::FormatterOption { };

    // TODO: Defining option types and functions is a bit verbose, it
    // would be nice if there was less boilerplate. Also, it would be
    // nice if these types would be hidden and only a constructor
    // function would be visible.
    // To reduce verbosity:
    //  - Define a template superclass that allocates storage for a
    //    value with a templated type. Downside is that inheriting
    //    constructors is very verbose (needs repeating template
    //    parameters).
    //  - Define a template helper class that allocates storage, but also
    //    has a tag template parameter to make it unique (so you do not
    //    need to subclass, but can use a type alias to it instead).
    //    This is still a bit verbose because you need to define a tag
    //    and the alias.
    /*
          template <typename tag, typename TValue>
          struct FormatterOptionHelper : public FormatterOption  {
              TValue value;
              constexpr FormatterOptionHelper(TValue value) : value(value) { }
          };

          struct FormatOptionPrecisionTag;
          using FormatOptionPrecision = FormatterOptionHelper<FormatOptionPrecisionTag, uint8_t>;
    */
    // Both options might be slightly less verbose, but also more
    // complex, so might not be worth the trouble.
    //
    // To reduce visibility:
    //  - Make the option classes private, and make the global
    //    builder functions friend. Downside is an extra friend
    //    declaration.
    //    It seems that using a friend *definition* (instead of a
    //    declaration) is appropriate, which defines a global function
    //    and makes it a friend at the same time. However, it seems this
    //    creates a hidden function, only accessible through ADL, which
    //    does not apply to an argumentless function.
    //    A normal friend declaration can also be problematic, since it
    //    forwards declares the function in the same namespace as the
    //    enclosing class. If that is not where it should end up, the
    //    builder function should be forward declared before the
    //    formatter class. But since it references the formatter class,
    //    this gets complex with forward declarations. This might be ok
    //    if the formatter class is in the global namespace.
    //  - Make the option classes private, add a public static builder
    //    method and replace the global builder function by a constexpr
    //    function pointer. Downside is an extra variable, which *might*
    //    end up in the final binary.

    struct FormatOptionBase : FormatterOption {
      constexpr FormatOptionBase(uint8_t value) : value(value) { }
      uint8_t value;
    };
    constexpr DefaultFormatter operator+(FormatOptionBase o) {
      return {o.value, this->min_width, this->precision};
    }

    struct FormatOptionPrecision : FormatterOption {
      constexpr FormatOptionPrecision(uint8_t value) : value(value) { }
      uint8_t value;
    };
    constexpr DefaultFormatter operator+(FormatOptionPrecision o) {
      return {this->base, o.value, this->precision};
    }

    struct FormatOptionMinWidth : FormatterOption {
      constexpr FormatOptionMinWidth(uint8_t value) : value(value) { }
      uint8_t value;
    };
    constexpr DefaultFormatter operator+(FormatOptionMinWidth o) {
      return {this->base, this->min_width, o.value};
    }

    constexpr DefaultFormatter(uint8_t base = 10, uint8_t min_width = 0, uint8_t precision = 2)
      : base(base), min_width(min_width), precision(precision)
    { }

    //static constexpr FormatOptionPrecision FORMAT_PRECISION(uint8_t prec) { return {prec}; }

    /* String printing, defined in cpp file */
    size_t printTo(Print*, const __FlashStringHelper *) const;
    size_t printTo(Print*, const String &) const;

    /* Stuff that is easy to print */
    _always_inline size_t printTo(Print* p, const char      str[]) const { return p->write(str); }
    _always_inline size_t printTo(Print* p, const char      c    ) const { return p->write(c); }
    _always_inline size_t printTo(Print* p, const Printable &x   ) const { return x.printTo(*p); }

    /* Integer printing, upcast to (unsigned) long and then printed usign
     * a shared print(Signed)Number function. */
    _always_inline size_t printTo(Print* p,   signed char  n) const { return printSignedNumber(p, n); }
    _always_inline size_t printTo(Print* p,   signed short n) const { return printSignedNumber(p, n); }
    _always_inline size_t printTo(Print* p,   signed int   n) const { return printSignedNumber(p, n); }
    _always_inline size_t printTo(Print* p,   signed long  n) const { return printSignedNumber(p, n); }
    _always_inline size_t printTo(Print* p, unsigned char  n) const { return printNumber(p, n); }
    _always_inline size_t printTo(Print* p, unsigned short n) const { return printNumber(p, n); }
    _always_inline size_t printTo(Print* p, unsigned int   n) const { return printNumber(p, n); }
    _always_inline size_t printTo(Print* p, unsigned long  n) const { return printNumber(p, n); }

    /* Float is converted to double first */
    _always_inline size_t printTo(Print* p,     float      n) const { return printFloat(p, n); }
    _always_inline size_t printTo(Print* p,     double     n) const { return printFloat(p, n); }

  private:
    size_t printNumber(Print*, unsigned long) const;
    size_t printSignedNumber(Print*, signed long) const;
    size_t printFloat(Print*, double) const;

    uint8_t base;
    uint8_t min_width;
    uint8_t precision;

};

/******************************************************************
 * OptionList                                                     */

template<typename THead, typename TTail = void>
class OptionList;

template<typename THead, typename TTail>
class OptionList : public Print::FormatterOption {
  public:
    THead head;
    TTail tail;

    constexpr OptionList(const THead& head, const TTail& tail) : head(head), tail(tail) { }

    // Apply our contained options to a formatter
    template<typename TFormatter>
    auto addToFormatter(const TFormatter& formatter) const
    -> enable_if_valid_t<decltype(accepts_formatter(&formatter)),
                         decltype(formatter + this->head + this->tail)>
    {
      return formatter + this->head + this->tail;
    }

    // Make all OptionLists friends of us
    template<typename T1, typename T2> friend class OptionList;

  public:
    // Append another option
    template<typename TOption>
    constexpr auto operator +(const TOption& option) const
    -> enable_if_valid_t<decltype(accepts_option(&option)),
                         OptionList<TOption, decltype(this->tail + option)>>
    {
      return {this->head, this->tail + option};
    }
};

template<typename THead>
struct OptionList<THead, void> : public Print::FormatterOption {
  public:
    THead head;

    constexpr OptionList(const THead& head) : head(head) { }

    // Apply our contained options to a formatter
    template<typename TFormatter>
    constexpr auto addToFormatter(const TFormatter& formatter) const
    -> enable_if_valid_t<decltype(accepts_formatter(&formatter)),
                         decltype(formatter + this->head)>
    {
      return formatter + this->head;
    }

    // Make all OptionLists friends of us
    template<typename T1, typename T2> friend class OptionList;

  public:
    // Append another option
    template<typename TOption>
    constexpr auto operator +(const TOption& option) const
    -> enable_if_valid_t<decltype(accepts_option(&option)),
                         OptionList<THead, OptionList<TOption, void>>>
    {
      return {this->head, option};
    }

};

// Apply an option list by adding it to a formatter (e.g. by passing it
// to print())
template<typename TFormatter, typename THead, typename TTail>
constexpr auto operator +(const TFormatter& formatter, const OptionList<THead, TTail>& list)
-> decltype(list.addToFormatter(formatter))
{
  return list.addToFormatter(formatter);
}

// Start an OptionList by adding two options
template<typename TOption1, typename TOption2>
constexpr auto operator+(const TOption1& first, const TOption2& second)
-> enable_if_valid_t<decltype(accepts_option(&first), accepts_option(&second)),
                     OptionList<TOption1, OptionList<TOption2, void>>>
{
  return {first, second};
}

// The DefaultFormatter for an option list is the DefaultFormatter for
// the first option
template<typename TValue, typename THead, typename TTail>
auto DefaultFormatterFor(TValue value, OptionList<THead, TTail> list)
-> decltype(DefaultFormatterFor(value, list.head)) {
  return DefaultFormatterFor(value, list.head);
}

/* End of OptionList stuff                                        *
 ******************************************************************/

#if 0
inline DefaultFormatter DefaultFormatterFor(Any /* value */, FormatOptionMinWidth) {
  return {};
}

inline DefaultFormatter DefaultFormatterFor(Any /* value */, FormatOptionBase) {
  return {};
}
#endif
template<typename T>
inline DefaultFormatter DefaultFormatterFor(T, DefaultFormatter::FormatterOption) {
  return {};
}

/*
template<typename T>
T declval();

template<typename TValue,
         typename TOption,
         typename Test1 = decltype(declval<DefaultFormatter>().addOption(declval<TOption>())),
         typename Test2 = decltype(declval<DefaultFormatter>().printTo(declval<Print*>(), declval<TValue>()))
>
inline DefaultFormatter DefaultFormatterFor(TValue, TOption) {
  return {};
}
*/

} // namespace Formatters

#undef HEX
inline constexpr Formatters::DefaultFormatter::FormatOptionMinWidth FORMAT_MIN_WIDTH(uint8_t min_width) { return {min_width}; }
inline constexpr Formatters::DefaultFormatter::FormatOptionBase FORMAT_BASE(uint8_t base) { return {base}; }
inline constexpr Formatters::DefaultFormatter::FormatOptionPrecision FORMAT_PRECISION(uint8_t prec) { return {prec}; }
constexpr Formatters::DefaultFormatter::FormatOptionBase HEX = FORMAT_BASE(16);

//constexpr auto FORMAT_PRECISION = Formatters::DefaultFormatter::FORMAT_PRECISION;

inline size_t Print::print(  signed char  n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(  signed short n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(  signed int   n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(  signed long  n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(unsigned char  n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(unsigned short n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(unsigned int   n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(unsigned long  n, int base) { return print(n, FORMAT_BASE(base)); }
inline size_t Print::print(    float      n, int prec) { return print(n, FORMAT_PRECISION(prec)); }
inline size_t Print::print(    double     n, int prec) { return print(n, FORMAT_PRECISION(prec)); }

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
      size_t n = Formatters::DefaultFormatter().printTo(p, arg);
      return n + printTo(p, args...);
    }
/*
    template<typename T, typename T2, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &arg2, const Ts &...args)
    -> enable_if_valid_t<decltype(arg2.printTo(p, arg)), size_t> {
      size_t n = arg2.printTo(p, arg);
      return n + printTo(p, args...);
    }
*/

    template<typename T, typename T2, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &formatter, const Ts &...args)
    -> enable_if_valid_t<decltype(accepts_formatter(&formatter)), size_t> {
    //-> enable_if_valid_t<decltype(formatter.printTo(p, arg)), size_t> {
      size_t n = formatter.printTo(p, arg);
      return n + printTo(p, args...);
    }


    template<typename T, typename TFormatter, typename TOption, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const TFormatter &formatter, const TOption &option, const Ts &...args)
    //-> enable_if_valid_t<decltype(formatter.addOption(option)), size_t> {
    -> enable_if_valid_t<decltype(accepts_formatter(&formatter), accepts_option(&option)), size_t> {
      //return printTo(p, arg, formatter.addOption(option), args...);
      return printTo(p, arg, formatter + option, args...);
    }


    template<typename T, typename T2, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const T2 &option, const Ts &...args)
    //-> enable_if_valid_t<decltype(typename T2::Formatter().addOption(option)), size_t> {
    -> enable_if_valid_t<decltype(accepts_option(&option)), size_t> {
      auto formatter = DefaultFormatterFor(arg, option);
      //return printTo(p, arg, formatter + option, args...);
      return printTo(p, arg, formatter, option, args...);
    }

    // The next two overloads unpack an OptionList when it is
    // supplied. These are not strictly needed (OptionList implements
    // operator+ to apply each of its arguments to the formatter in
    // turn), but without these the error messages are less clear if
    // incompatible options are mixed (with the below overloads the
    // error shows the formatter and the incompatible option, while
    // without them only the formatter and the entire option list are
    // shown.
    //
    // If we keep these, OptionList::addToFormatter and the related
    // operator+ overload can be removed.
    template<typename T, typename TFormatter, typename THead, typename TTail, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const TFormatter &formatter, const Formatters::OptionList<THead, TTail> &list, const Ts &...args)
    //-> enable_if_valid_t<decltype(formatter.addOption(list.head)), size_t> {
    -> enable_if_valid_t<decltype(accepts_formatter(&formatter)), size_t> {
      return printTo(p, arg, formatter, list.head, list.tail, args...);
    }

    // Base case for the end of the OptionList
    template<typename T, typename TFormatter, typename THead, typename ...Ts>
    static _always_inline auto printTo(Print * p, const T &arg, const TFormatter &formatter, const Formatters::OptionList<THead, void> &list, const Ts &...args)
    //-> enable_if_valid_t<decltype(formatter.addOption(list.head)), size_t> {
    -> enable_if_valid_t<decltype(accepts_formatter(&formatter)), size_t> {
      return printTo(p, arg, formatter, list.head, args...);
    }

    // Base case for the argument recursion - no more things to print
    static _always_inline size_t printTo(Print *) { return 0; }
};

template<typename ...Ts>
inline _always_inline size_t Print::print(const Ts &...args) { return PrintHelper::printTo(this, args...); }

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
//
// Idea: Instead of using a Formatter and FormatterOption superclass,
// create them as wrapper classes, so you can match them directly. Then
// unpack and repack the values inside whenever you work with them.
//
// Idea: Expose accepts_option and accepts_formatter to debug problems
// when formatters are not accepted (which typically leads to errors
// that only say printTo(..., formatter/option) is not defined. A typical
// error is no (or private) Formatter/FormatterOption base class.
//
// Mail
//
// For adding options to formatters, I'm using the overloaded addition
// operator. This has the advantage that you can provide it inside the
// formatter class, but also outside of it. This means you can
// potentially define more formatter options, and apply them to an
// existing formatter, without having to modify the formatter. If we
// prefer a normal method/function, the same could probably be obtained
// using a global overloaded addOption function (with a template version
// that defers to an addOption method if it exists).
//
// Formatters and options are currently passed around as const
// references, meaning that their printTo methods and addition operators
// must be const methods. We can probably also provide non-const
// reference and/or rvalue-reference versions of the variadic print
// methods (probably even automatically template-generated by using &&
// and std::forward), in case we want to allow implementing formatters
// than modify themselves rather than return a new formatter (though I
// can't really come up with a usecase for this - maybe a formatter that
// counts characters written for alignment?).
