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
* @brief     C++ API:variadic utils
*
* @file
*/

#pragma once

// --------------------------------------------------------------------------
//
// Provides access to specific type in a variadic.
//
namespace util {

  // --------------------------------------------------------------------------
  template <int N, typename... T>
  struct variadic_element;

  template <typename T0, typename... T>
  struct variadic_element<0, T0, T...> {
    typedef T0 type;
  };

  template <int N, typename T0, typename... T>
  struct variadic_element<N, T0, T...> {
    typedef typename variadic_element<N - 1, T...>::type type;
  };

} // namespace util
