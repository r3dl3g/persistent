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

#include "persistent/basic_struct.h"

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

// --------------------------------------------------------------------------
struct test_int64 : public basic_struct<prop::int64, prop::int64> {
  typedef basic_struct<prop::int64, prop::int64> super;

  test_int64 ()
    : super(i, j)
    , i(names::i)
    , j(names::j)
  {}

  test_int64 (const test_int64& rhs)
    : test_int64 () {
    operator=(rhs);
  }

  prop::int64 i;
  prop::int64 j;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------
struct test2 : public basic_container {

  prop_t::type<int64_t, names::i1> i1;
  prop_t::type<test_int64, names::t1> t1;
  prop_t::type<int64_t, names::i2> i2;

  auto get_members () -> decltype(std::tie(i1, t1, i2)) {
    return std::tie(i1, t1, i2);
  }

  auto get_members () const -> decltype(std::tie(i1, t1, i2)) {
    return std::tie(i1, t1, i2);
  }

};

// --------------------------------------------------------------------------
struct test3 : public basic_container {
private:
  prop::type<std::vector<test_int64>> m_v;

public:
  typedef member_variables_t<decltype(m_v)> member_variables;

  test3 ()
    : members(m_v)
    , m_v(names::v)
  {}

  test3 (const test3& rhs)
    : test3()
  {
    members=rhs.members;
  }

  member_variables& get_members () {
    return members;
  }

  const member_variables& get_members () const {
    return members;
  }

  const std::vector<test_int64>& v () const {
    return m_v();
  }

  std::vector<test_int64>& v () {
    return m_v();
  }

private:
  member_variables members;
};

// --------------------------------------------------------------------------
struct test4 : public basic_struct<prop_t::type<int64_t, names::i>, prop_t::type<std::vector<int>, names::i>> {
  typedef basic_struct<prop_t::type<int64_t, names::i>, prop_t::type<std::vector<int>, names::i>> super;

  test4 (int64_t i_ = {}, const std::vector<int>& l_ = {})
    : super(i, l)
    , i(i_)
    , l(l_)
  {}

  test4 (const test4& rhs)
    : test4 () {
    operator=(rhs);
  }

  prop_t::type<int64_t, names::i> i;
  prop_t::type<std::vector<int>, names::i> l;
};

// --------------------------------------------------------------------------
struct test5 : public basic_struct<prop::text, prop::list<std::string>> {
  typedef basic_struct<prop::text, prop::list<std::string>> super;

  test5 (const std::string& i_ = {}, const std::vector<std::string>& l_ = {})
    : super(i, l)
    , i(names::i, i_)
    , l(names::i, l_)
  {}

  test5 (const test5& rhs)
    : test5 () {
    operator=(rhs);
  }

  prop::text i;
  prop::list<std::string> l;
};

// --------------------------------------------------------------------------
struct test6 : public basic_struct<prop::list<std::string>, prop::text> {
  typedef basic_struct<prop::list<std::string>, prop::text> super;

  test6 (const std::string& i_ = {}, const std::vector<std::string>& l_ = {})
    : super(l, i)
    , l(names::i, l_)
    , i(names::i, i_)
  {}

  test6 (const test6& rhs)
    : test6 () {
    operator=(rhs);
  }

  prop::list<std::string> l;
  prop::text i;
};

// --------------------------------------------------------------------------

