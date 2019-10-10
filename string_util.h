/**
* @copyright (c) 2016-2019 Ing. Buero Rothfuss
*                          Riedlinger Str. 8
*                          70327 Stuttgart
*                          Germany
*                          http://www.rothfuss-web.de
*
* @author    <a href="mailto:armin@rothfuss-web.de">Armin Rothfuss</a>
*
* Project    standard lib
*
* Customer   -
*
* @brief     C++ API: std::string utilities
*
* @file
*/

#pragma once

// --------------------------------------------------------------------------
//
// Common includes
//
#include <string>
#include <vector>
#include <iterator>
#include <sstream>


// --------------------------------------------------------------------------
//
// Library includes
//
#include <persistent-export.h>

/**
* Provides an API to stream into OutputDebugString.
*/
namespace util {

  namespace string {

    // --------------------------------------------------------------------------
    PERSISTENT_EXPORT std::string utf16_to_utf8 (const std::wstring&);
    PERSISTENT_EXPORT std::string utf16_to_utf8 (wchar_t wc);
    PERSISTENT_EXPORT std::wstring utf8_to_utf16 (const std::string&);

    // --------------------------------------------------------------------------
    PERSISTENT_EXPORT bool starts_with (const std::string& str, const std::string& prefix);
    PERSISTENT_EXPORT bool ends_with (const std::string& str, const std::string& suffix);

    // --------------------------------------------------------------------------
    PERSISTENT_EXPORT std::string::size_type find_left_space (const std::string& text, std::size_t cursor_pos);
    PERSISTENT_EXPORT std::string::size_type find_right_space (const std::string& text, std::size_t cursor_pos);

    // --------------------------------------------------------------------------
    template<char delimiter>
    class delimited : public std::string {};

    template<char delimiter>
    std::istream& operator>> (std::istream& is, delimited<delimiter>& output) {
      std::getline(is, output, delimiter);
      return is;
    }

    template<char delimiter>
    std::vector<std::string> split (const std::string& t) {
      std::istringstream iss(t);
      typedef std::istream_iterator<delimited<delimiter>> iterator;
      std::vector<std::string> v(iterator{iss}, iterator{});
      if (!t.empty() && (t.back() == '\n')) {
        v.emplace_back(std::string());
      }
      return v;
    }

    PERSISTENT_EXPORT std::string merge (const std::vector<std::string>& v, const std::string& delimiter);

    template<char delimiter>
    std::string merge (const std::vector<std::string>& v) {
      return merge(v, std::string(1, delimiter));
    }

    // trim front
    PERSISTENT_EXPORT void ltrim (std::string& s);
    PERSISTENT_EXPORT std::string ltrimed (std::string s);

    // trim end
    PERSISTENT_EXPORT void rtrim (std::string& s);
    PERSISTENT_EXPORT std::string rtrimed (std::string s);

    // trim both ends
    PERSISTENT_EXPORT void trim (std::string& s);
    PERSISTENT_EXPORT std::string trimed (std::string s);
    
    // replace sequence in string
    PERSISTENT_EXPORT void replace (std::string& s, const std::string& from, const std::string& to);
    PERSISTENT_EXPORT void replace (std::string& s, const char* from, const char* to);

    PERSISTENT_EXPORT std::string replaced (std::string s, const std::string& from, const std::string& to);
    PERSISTENT_EXPORT std::string replaced (std::string s, const char* from, const char* to);

#if __cplusplus <= 201103L

    struct quoted_out {
      inline quoted_out (const std::string& t)
        : text(t)
      {}
      inline quoted_out (const char* t)
        : text(t)
      {}

      std::string text;
    };

    struct quoted_in {
      inline quoted_in (std::string& t)
        : text(t)
      {}

      std::string& text;
    };

    PERSISTENT_EXPORT std::ostream& operator<< (std::ostream& os, const quoted_out&);
    PERSISTENT_EXPORT std::istream& operator>> (std::istream& is, quoted_in&&);

    inline quoted_out quoted (const std::string& t) {
      return quoted_out(t);
    }

    inline quoted_out quoted (const char* t) {
      return quoted_out(t);
    }

    inline quoted_in quoted (std::string& t) {
      return quoted_in(t);
    }

#else

    inline auto quoted (const std::string& t) -> decltype(std::quoted(const std::string&)) {
      return std::quoted(t);
    }

    inline auto quoted (const char* t) -> decltype(std::quoted(const char*)) {
      return std::quoted(t);
    }

    inline auto quoted (std::string& t) -> decltype(std::quoted(std::string&)) {
      return std::quoted(t);
    }

#endif

    struct name_in {
      name_in (std::string& t)
        : text(t)
      {}

      std::string& text;
    };

    PERSISTENT_EXPORT std::istream& operator >> (std::istream& is, name_in&&);

    inline name_in name (std::string& t) {
      return name_in(t);
    }

    template<class _InIt, class _OutIt, class _Pr>
    inline void copy_until (_InIt _First, _InIt _Last, _OutIt _Dest, _Pr _Pred) {
      while ((_First != _Last) && _Pred(*_First)) {
        *_Dest++ = *_First++;
      }
    }

  } // namespace string

  namespace utf8 {

    PERSISTENT_EXPORT bool is_continuation_char (char ch);

    PERSISTENT_EXPORT std::size_t get_left_char (const std::string& s, std::size_t pos);
    PERSISTENT_EXPORT std::size_t get_right_char (const std::string& s, std::size_t pos);

  } // namespace utf8

  namespace win32 {

    PERSISTENT_EXPORT std::string get_system_error_message (uint32_t msg);
    PERSISTENT_EXPORT std::string get_last_system_error_message ();

  }

} // namespace util
