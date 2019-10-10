/**
* @copyright (c) 2015-2018 Ing. Buero Rothfuss
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
* @brief     C++ API:vector utils
*
* @file
*/

#pragma once

// --------------------------------------------------------------------------
//
// Common includes
//
#include <vector>
#include <limits>
#include <algorithm>

namespace util {

  // --------------------------------------------------------------------------
  //
  // helper for specialization
  //
  template <typename T>
  struct is_vector {
    enum {
      value = false
    };
  };

  template <typename T, typename A>
  struct is_vector<std::vector<T, A>> {
    enum {
      value = true
    };
  };

  template <typename T, typename A>
  struct is_vector<const std::vector<T, A>> {
    enum {
      value = true
    };
  };

  template <typename T>
  using is_vector_t = typename is_vector<T>::value;

  // --------------------------------------------------------------------------
  template <typename T>
  struct is_array {
    enum {
      value = false
    };
  };

  template <typename T, size_t S>
  struct is_array<std::array<T, S>> {
    enum {
      value = true
    };
  };

  template <typename T, size_t S>
  struct is_array<const std::array<T, S>> {
    enum {
      value = true
    };
  };

  template <typename T>
  using is_array_t = typename is_array<T>::value;

  // --------------------------------------------------------------------------
  template<typename T>
  T max_element (const std::vector<T>& v) {
    typedef typename std::vector<T>::const_iterator iterator;

    const iterator e = v.end();
    const iterator i = std::max_element(v.begin(), e);
    return (i != e) ? *i : T();
  }

  template<typename T, class Compare>
  T max_element (const std::vector<T>& v, Compare compare) {
    typedef typename std::vector<T>::const_iterator iterator;

    const iterator e = v.end();
    const iterator i = std::max_element(v.begin(), e, compare);
    return (i != e) ? *i : T();
  }

  template<typename T>
  T min_element (const std::vector<T>& v) {
    typedef typename std::vector<T>::const_iterator iterator;

    const iterator e = v.end();
    const iterator i = std::min_element(v.begin(), e);
    return (i != e) ? *i : T();
  }

  template<typename T, class Compare>
  T min_element (const std::vector<T>& v, Compare compare) {
    typedef typename std::vector<T>::const_iterator iterator;

    const iterator e = v.end();
    const iterator i = std::min_element(v.begin(), e, compare);
    return (i != e) ? *i : T();
  }

} // namespace util
