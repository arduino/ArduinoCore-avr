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


namespace detail {
  // Returns a value of the given type. No implementation, so this is
  // meant for use in decltype only. Identical to std::declval.
  template <typename T> T declval();

  // Function that accept a pointer of the given type. Can be used to
  // detect if another type is convertible to T.
  template<typename T>
  void accepts_pointer_of_type(const T*);

  // Simplified and integrated version of std::enable_if_t and
  // std::is_base_of (which are not available on AVR). Integrating them
  // makes things more specific and thus simpler.
  // TODO: This check does not detect private base classes. To do so, a
  // more complicated check is needed, something like
  // https://stackoverflow.com/questions/2910979/how-does-is-base-of-work
  template<typename Base, typename Derived>
  using enable_if_base_of = decltype(accepts_pointer_of_type<Base>(declval<Derived*>()));
}

/* Forward declarations */
namespace Formatters {
  template<typename THead, typename TTail = void>
  class OptionList;

  class DefaultFormatter;
}

template<typename T>
Formatters::DefaultFormatter DefaultFormatterFor(T);

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

    template<typename ...Ts> _always_inline size_t print(const Ts &...args) { return printMultiple(args...); }
    template<typename ...Ts> _always_inline size_t println(const Ts &...args) { size_t t = print(args...); return t + println(); }

    //_always_inline size_t print() { return 0; }

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

  private:
    // Base case for the argument recursion - no more things to print
    _always_inline size_t printMultiple() { return 0; }

    // Simplest case: Just a value, no formatters or options (used when
    // none of the below overloads match): Look up the default formatter
    // for this value, and add it to the argument list.
    template<typename T, typename ...Ts>
    _always_inline size_t printMultiple(const T &arg, const Ts &...args) {
      // This might lead to infinite template instantion
      //return print(arg, DefaultFormatter<>(), args...);
      return printMultiple(arg, DefaultFormatterFor(arg), args...);
    }

    // A value and a formatter is specified without any options (or the
    // below overloads have applied the options): Let the formatter
    // print the value.
    template<
      typename T,
      typename TFormatter,
      typename ...Ts,
      detail::enable_if_base_of<Print::Formatter, TFormatter>* = nullptr
    >
    _always_inline size_t printMultiple(const T &arg, const TFormatter &formatter, const Ts &...args) {
      size_t n = formatter.printTo(this, arg);
      return n + printMultiple(args...);
    }

    // A value, a formatter and an option is specified: Apply the option
    // to the formatter.
    template<
      typename T,
      typename TFormatter,
      typename TOption,
      typename ...Ts,
      detail::enable_if_base_of<Print::Formatter, TFormatter>* = nullptr,
      detail::enable_if_base_of<Print::FormatterOption, TOption>* = nullptr
    >
    _always_inline size_t printMultiple(const T &arg, const TFormatter &formatter, const TOption &option, const Ts &...args) {
      return printMultiple(arg, formatter + option, args...);
    }

    // A value and an option is specified: Look up the default formatter
    // for this value and option and add it to the argument list.
    template<
      typename T,
      typename TOption,
      typename ...Ts,
      detail::enable_if_base_of<Print::FormatterOption, TOption>* = nullptr
    >
    _always_inline size_t printMultiple(const T &arg, const TOption &option, const Ts &...args) {
      auto formatter = DefaultFormatterFor(arg, option);
      return printMultiple(arg, formatter, option, args...);
    }

    // The next two overloads unpack an OptionList when it is
    // supplied. These are not strictly needed (OptionList implements
    // operator+ to apply each of its arguments to the formatter in
    // turn), but without these the error messages are less clear if
    // incompatible options are mixed (with the below overloads the
    // error shows the formatter and the incompatible option, while // without them only the formatter and the entire option list are
    // shown.
    //
    // If we keep these, OptionList::addToFormatter and the related
    // operator+ overload can be removed.
    //
    // If we add one more overload for (Value, OptionList, ...), we can
    // also remove the DefaultFormatterForm definition for OptionList.
    template<
      typename T,
      typename TFormatter,
      typename THead,
      typename TTail,
      typename ...Ts,
      detail::enable_if_base_of<Print::Formatter, TFormatter>* = nullptr
    >
    _always_inline size_t printMultiple(const T &arg, const TFormatter &formatter, const Formatters::OptionList<THead, TTail> &list, const Ts &...args) {
      return printMultiple(arg, formatter, list.head, list.tail, args...);
    }

    // Base case for the end of the OptionList
    template<
      typename T,
      typename TFormatter,
      typename THead,
      typename ...Ts,
      detail::enable_if_base_of<Print::Formatter, TFormatter>* = nullptr
    >
    _always_inline size_t printMultiple(const T &arg, const TFormatter &formatter, const Formatters::OptionList<THead, void> &list, const Ts &...args) {
      return printMultiple(arg, formatter, list.head, args...);
    }
};


void accepts_formatter(const Print::Formatter*);
void accepts_option(const Print::FormatterOption*);

// Namespace for more advanced custom formatter stuff, to prevent
// cluttering the global namespace. Could be removed if needed.
namespace Formatters {

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
      return {this->base, this->min_width, o.value};
    }

    struct FormatOptionMinWidth : FormatterOption {
      constexpr FormatOptionMinWidth(uint8_t value) : value(value) { }
      uint8_t value;
    };
    constexpr DefaultFormatter operator+(FormatOptionMinWidth o) {
      return {this->base, o.value, this->precision};
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

    /* Integer printing, upcast to (unsigned) long and then printed using
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

template<typename THead, typename TTail>
class OptionList : public Print::FormatterOption {
  public:
    THead head;
    TTail tail;

    constexpr OptionList(const THead& head, const TTail& tail) : head(head), tail(tail) { }

    // Apply our contained options to a formatter
    template<
      typename TFormatter,
      detail::enable_if_base_of<Print::Formatter, TFormatter>* = nullptr
    >
    auto addToFormatter(const TFormatter& formatter) const
    -> decltype(formatter + this->head + this->tail)
    {
      return formatter + this->head + this->tail;
    }

    // Make all OptionLists friends of us
    template<typename T1, typename T2> friend class OptionList;

  public:
    // Append another option
    template<
      typename TOption,
      detail::enable_if_base_of<Print::FormatterOption, TOption>* = nullptr
    >
    constexpr auto operator +(const TOption& option) const
    -> OptionList<TOption, decltype(this->tail + option)>
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
    template<
      typename TFormatter,
      detail::enable_if_base_of<Print::Formatter, TFormatter>* = nullptr
    >
    constexpr auto addToFormatter(const TFormatter& formatter) const
    -> decltype(formatter + this->head)
    {
      return formatter + this->head;
    }

    // Make all OptionLists friends of us
    template<typename T1, typename T2> friend class OptionList;

  public:
    // Append another option
    template<
      typename TOption,
      detail::enable_if_base_of<Print::FormatterOption, TOption>* = nullptr
    >
    constexpr auto operator +(const TOption& option) const
    -> OptionList<THead, OptionList<TOption, void>>
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
template<
  typename TOption1,
  typename TOption2,
  detail::enable_if_base_of<Print::FormatterOption, TOption1>* = nullptr
  detail::enable_if_base_of<Print::FormatterOption, TOption2>* = nullptr
>
constexpr auto operator+(const TOption1& first, const TOption2& second)
-> OptionList<TOption1, OptionList<TOption2, void>>
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

} // namespace Formatters

template<typename T>
inline Formatters::DefaultFormatter DefaultFormatterFor(T, Formatters::DefaultFormatter::FormatterOption) {
  return {};
}

template<typename T>
inline Formatters::DefaultFormatter DefaultFormatterFor(T) {
  return {};
}

#undef HEX
inline constexpr Formatters::DefaultFormatter::FormatOptionMinWidth FORMAT_MIN_WIDTH(uint8_t min_width) { return {min_width}; }
inline constexpr Formatters::DefaultFormatter::FormatOptionBase FORMAT_BASE(uint8_t base) { return {base}; }
inline constexpr Formatters::DefaultFormatter::FormatOptionPrecision FORMAT_PRECISION(uint8_t prec) { return {prec}; }
constexpr Formatters::DefaultFormatter::FormatOptionBase HEX = FORMAT_BASE(16);
//constexpr auto FORMAT_PRECISION = Formatters::DefaultFormatter::FORMAT_PRECISION;

// Compatibility versions of print that take a second base or
// precision argument that is an integer. Defined here, since they need
// to refer to DefaultFormatter options.
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
// types you still get the proper "no such method to call, candidates
// are..." error message.
//
// Idea: Instead of using a Formatter and FormatterOption superclass,
// create them as wrapper classes, so you can match them directly. Then
// unpack and repack the values inside whenever you work with them. This
// avoids the need for these somewhat clunky checks using
// enable_if_base_of. Declaring an option would then be a bit more
// involved and less intuitive, though. Note that we cannot just declare
// the parameters to be of the superclass type, since we need to know
// the actual formatter/option class too.
//
// Idea: Because DefaultFormatter::printTo accesses options through
// this, the compiler seems to force the options onto the stack and
// passes a pointer. It would probably be more efficient to just load
// the options directly into registers instead. To do this,
// all DefaultFormatter::printTo could be changed to static methods and
// accept the DefaultFormatter instance by-value, and a generic
// non-static printTo could be added that forwards all calls to the
// static versions. Doing this in DefaultFormatter instead of coding
// this in Print, leaves control over by-value/by-ref at the
// formatter author and probably gives the cleanest Formatter interface
// (an alternative would to just have Print call a static printTo,
// in which case the formatter could still decide to accept the
// formatter instance by reference or by value).
//
// Limitation: Default formatters for a value type and/or option type
// are specified using overloads of the DefaultFormatterFor function.
// You can use templates to get wildcard overloads (e.g. specify a
// default formatter for all options passed to a specific type, or a
// default formatter for any type combined with a specific formatter).
//
// If both overloads exist, this might cause ambiguity. e.g. when you
// have a (FooT value, *) overload and a (*, BarOption) overload, doing
// print(FooT(), BarOption()) is ambiguous. Usually, this will also be
// invalid (the formatter belonging to BarOption will likely not know
// how to print a FooT anyway). There might be cases where it is not,
// but it is not clear which of the two to favor in this case. If
// needed, some kind of priority tag argument could later be added, with
// a fallback to the regular tag-less version). Also, even in the
// invalid case, the "ambiguous" error message is not so nice.
//
// Note that the current approach of using a formatter-specific
// superclass (e.g. DefaultFormatter::FormatterOption) between the
// actual option class and Print::FormatterOption already makes
// overloads that use it (and thus need parent class conversion) less
// specific than templated overloads (which match their arguments
// exactly). This resolves the ambiguity, but I'm not sure if this is
// the right resolution.

// Limitation: DefaultFormatterFor relies on argument-dependent-lookup
// (ADL) to work properly. Normally, when a function is called, only
// overloads defined up to where the function call is defined are
// considered (e.g. the call the DefaultFormatterFor in Print.h). In
// this case, we want to allow defining more overloads later. We can
// make this work because DefaultFormatterFor is only called from
// template functions, and for those ADL happens at template
// *instantiation time*, rather than *definition time*.
//
// ADL happens for all types in a namespace (including the root
// namespace). Notably, this means ADL happens for class types (options
// and custom value types), but not for native types (e.g.  int does not
// live in a namespace). In practice, this means that replacing the
// default formatter for a native type (without any options) is not
// possible, since the reference to e.g. DefaultFormatterFor(int) is
// looked up at template definition time, not instantiation time.
//
// Two possible workarounds for this would be to add a wrapper class
// around values before passing them to DefaultFormatterFor, or adding
// an unused dummy argument that forces ADL. The latter is probably
// easiest, and if the dummy argument is called NoOption and is the
// second argument, that might actually be fairly easy to work with as
// well.
//
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
//
// Compatibility with C++ <= 11
//
// ADL needed for DefaultFormatterFor - no primitive types
//
// TODO: Suggest workaround for ADL/primitive types using wrapper type
// (not in mail). Probably requires priority tagging to try wrapped type
// before unwrapped type, or some kind of primitive/nonprimitive
// detection for SFINAE. Or add Dummy argument, to force (meaningless)
// ADL.
//
// TODO: See if accepts_formatter can be called in default template
// argument as well (these are filled in after deduction, right? We only
// depend on the type, not the argument value)?
//
// TODO: Use NoOption dummy argument to DefaultFormatterFor to force
// ADL?
