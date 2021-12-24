/**
* @copyright (c) 2015-2021 Ing. Buero Rothfuss
*                          Riedlinger Str. 8
*                          70327 Stuttgart
*                          Germany
*                          http://www.rothfuss-web.de
*
* @author    <a href="mailto:armin@rothfuss-web.de">Armin Rothfuss</a>
*
* Project    persistent lib
*
* @brief     C++ struct persistence test
*
* @license   MIT license. See accompanying file LICENSE.
*/

#pragma once

#include <memory>
#include "persistent/persistent.h"

using namespace persistent;

// --------------------------------------------------------------------------
struct names {
  static constexpr char i[] = "i";
  static constexpr char j[] = "j";
  static constexpr char v[] = "v";

  static constexpr char i1[] = "i1";
  static constexpr char t1[] = "t1";
  static constexpr char i2[] = "i2";
};

// --------------------------------------------------------------------------
template<typename T>
constexpr std::pair<char const*, T> get_test_data ();

template<> constexpr std::pair<char const*, bool>         get_test_data<bool> ()         { return { "1", true }; }
template<> constexpr std::pair<char const*, char>         get_test_data<char> ()         { return { "\"A\"", 'A' }; }
template<> constexpr std::pair<char const*, int8_t>       get_test_data<int8_t> ()       { return { "-127", -127 }; }
template<> constexpr std::pair<char const*, uint8_t>      get_test_data<uint8_t> ()      { return { "255", 255 }; }
template<> constexpr std::pair<char const*, int16_t>      get_test_data<int16_t> ()      { return { "-32767", -32767 }; }
template<> constexpr std::pair<char const*, uint16_t>     get_test_data<uint16_t> ()     { return { "65535", 65535 }; }
template<> constexpr std::pair<char const*, int32_t>      get_test_data<int32_t> ()      { return { "-2147483647", -2147483647 }; }
template<> constexpr std::pair<char const*, uint32_t>     get_test_data<uint32_t> ()     { return { "4294967295", 4294967295 }; }
template<> constexpr std::pair<char const*, int64_t>      get_test_data<int64_t> ()      { return { "-549755813887", -549755813887 }; }
template<> constexpr std::pair<char const*, uint64_t>     get_test_data<uint64_t> ()     { return { "549755813887", 549755813887 }; }
template<> constexpr std::pair<char const*, float>        get_test_data<float> ()        { return { "12345.12345", 12345.12345 }; }
template<> constexpr std::pair<char const*, double>       get_test_data<double> ()       { return { "12345678.12345678", 12345678.12345678 }; }
template<> inline    std::pair<char const*, std::string>  get_test_data<std::string> ()  { return { "Some text", "Some text" }; }
template<> inline    std::pair<char const*, std::unique_ptr<int64_t>> get_test_data<std::unique_ptr<int64_t>> () { return { "-549755813887", std::make_unique<int64_t>(-549755813887) }; }
template<> inline    std::pair<char const*, std::shared_ptr<int64_t>> get_test_data<std::shared_ptr<int64_t>> () { return { "-549755813886", std::make_shared<int64_t>(-549755813886) }; }

// --------------------------------------------------------------------------
struct test_int64 : private persistent_struct {

  int64_t i = 0;
  int64_t j = 0;

  auto attributes () {
    return make_attributes(attribute(i, names::i), attribute(j, names::j));
  }

  auto attributes () const {
    return (const_cast<test_int64&>(*this)).attributes();
  }

};

// ----------------------------------------------------------------------------------------------------------------------------------------------------
struct test2 : private persistent_struct {

  int64_t i1 = 0;
  test_int64 t1;
  std::unique_ptr<int64_t> i2;

  auto attributes () {
    return std::make_tuple(attribute(i1, names::i1), attribute(t1, names::t1), attribute(i2, names::i2));
  }

  const auto attributes () const {
    return (const_cast<test2&>(*this)).attributes();
  }

};

// --------------------------------------------------------------------------
struct test3 : private persistent_struct {
private:
  std::vector<test_int64> m_v;

public:
  auto attributes () {
    return std::make_tuple(attribute(m_v, names::v));
  }

  const auto attributes () const {
    return std::make_tuple(attribute(m_v, names::v));
  }

  const std::vector<test_int64>& v () const {
    return m_v;
  }

  std::vector<test_int64>& v () {
    return m_v;
  }
};

// --------------------------------------------------------------------------
struct test4 : private persistent_struct {
  test4 (int64_t i_ = {}, const std::vector<int>& l_ = {})
    : i(i_)
    , l(l_)
  {}

  auto attributes () {
    return std::make_tuple(attribute(i, names::i), attribute(l, names::i));
  }

  const auto attributes () const {
    return (const_cast<test4&>(*this)).attributes();
  }

  int64_t i;
  std::vector<int> l;
};

// --------------------------------------------------------------------------
struct test5 : private persistent_struct {
  test5 (const std::string& i_ = {}, const std::vector<std::string>& l_ = {})
    : i(i_)
    , l(l_)
  {}

  auto attributes () {
    return std::make_tuple(attribute(i, names::i), attribute(l, names::i));
  }

  const auto attributes () const {
    return (const_cast<test5&>(*this)).attributes();
  }

  std::string i;
  std::vector<std::string> l;
};

// --------------------------------------------------------------------------
struct test6 : private persistent_struct {
  test6 (const std::string& i_ = {}, const std::vector<std::string>& l_ = {})
    : i(i_)
    , l(l_)
  {}

  auto attributes () {
    return std::make_tuple(attribute(l, names::i), attribute(i, names::i));
  }

  const auto attributes () const {
    return (const_cast<test6&>(*this)).attributes();
  }

  std::vector<std::string> l;
  std::string i;
};

// --------------------------------------------------------------------------

