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

#include "persistent/persistent_json.h"
#include "persistent/persistent_xml.h"
#include <testing/testing.h>

using namespace persistent;

struct MyStruct : private persistent_struct {  // persistent structs must be subclasses of a persistent_struct

  MyStruct (double d_ = {}, int64_t i_ = {}, const std::string& s_ = {}, const std::array<int, 5>& a_ = {}, const std::vector<std::string>& v_ = {})
    : d(d_)                       // A standard initializer list
    , i(i_)
    , s(s_)
    , a(a_)
    , v(v_)
  {}

  double d;                       // A simple C/C++ struct
  int i;
  std::string s;
  std::array<int, 5> a;
  std::vector<std::string> v;

  template<typename T> using attr = persistent::attribute<T>;

  auto attributes () {            // Each persistent structs must provide an attributes method that returns a tuple with attributes of its members.
    return std::make_tuple(attr(d, "d"), attr(i, "i"), attr(s, "s"), attr(a, "a"), attr(v, "v"));
  }

  const auto attributes () const {// For convenience use a const cast
    return (const_cast<MyStruct&>(*this)).attributes();
  }

};

// --------------------------------------------------------------------------
void test_write () {

  MyStruct s{1.234, 4711, "Some text", {1, 2, 3, 4, 5}, {"One", "Two", "Three"}};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}");

  os.str({});
  persistent::io::write_json(os, s, true);
  testing::log_info(os.str());

  os.str({});
  persistent::io::write_xml(os, s, true);
  testing::log_info(os.str());
}

// --------------------------------------------------------------------------
void test_read () {

  MyStruct s;

  std::istringstream is("{\"d\":\"1.234\",\"i\":\"4711\",\"s\":\"Some text\",\"a\":[\"1\",\"2\",\"3\",\"4\",\"5\"],\"v\":[\"One\",\"Two\",\"Three\"]}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.d, 1.234);
  EXPECT_EQUAL(s.i, 4711);
  EXPECT_EQUAL(s.s, "Some text");
  std::array<int, 5> expected_a{1, 2, 3, 4, 5};
  EXPECT_EQUAL(s.a, expected_a);
  std::vector<std::string> expected_v{"One", "Two", "Three"};
  EXPECT_EQUAL(s.v, expected_v);
}

// --------------------------------------------------------------------------
struct MyStruct2 : private persistent_struct {   // make your struct a subclass of persistent_struct.

  MyStruct2 (int64_t i_ = {}, const std::string s_ = {})
    : i(i_)
    , s(s_)
  {}

  int64_t i;                                     // A simple int64 properties.
  std::string s;                                 // A string

  template<typename T> using attr = persistent::attribute<T>;

  auto attributes () {                           // give your properties a name
    return std::make_tuple(attr(i, "i"), attr(s, "s"));
  }

  const auto attributes () const {
    return (const_cast<MyStruct2&>(*this)).attributes();
  }

};

// --------------------------------------------------------------------------
void test_write2 () {

  MyStruct2 s{4711, "Some other text"};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"i\":\"4711\",\"s\":\"Some other text\"}");
}

// --------------------------------------------------------------------------
void test_read2 () {

  MyStruct2 s;

  std::istringstream is("{\"i\":\"4711\",\"s\":\"Some other text\"}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.i, 4711);
  EXPECT_EQUAL(s.s, "Some other text");
}

// --------------------------------------------------------------------------
void test_main (const testing::start_params& params) {
  testing::log_info("Running " __FILE__);
  run_test(test_write);
  run_test(test_read);

  run_test(test_write2);
  run_test(test_read2);
}

// --------------------------------------------------------------------------

