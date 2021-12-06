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

struct MyStruct : public basic_container {  // persistent structs must be subclasses of a basic_container

  MyStruct (double d_ = {}, int64_t i_ = {}, const std::string& s_ = {}, const std::array<int, 5>& a_ = {}, const std::vector<std::string>& v_ = {})
    : members(d, i, s, a, v)          // each member hast zo be registered in the members suple.
    , d("d", d_)                      // each member hast to be initialized with a name and optional with a value.
    , i("i", i_)
    , s("s", s_)
    , a("a", a_)
    , v("v", v_)
  {}

  MyStruct (const MyStruct& rhs)      // The copy operator hast to be rewritten that way,
    : MyStruct () {
    members = rhs.members;
  }

  prop<double> d;                     // Each member is now a prop<Type> memeber.
  prop<int> i;
  prop<std::string> s;
  prop<std::array<int, 5>> a;
  prop<std::vector<std::string>> v;

  // A typedef for the typesafe tuple that holds the member information.
  typedef member_variables_t<decltype(d), decltype(i), decltype(s), decltype(a), decltype(v)> member_variables;

  member_variables& get_members () {  // Accessors for the member information
    return members;
  }

  const member_variables& get_members () const {  // If only this exists, the struct can only be written, but not readen.
    return members;
  }

private:
  member_variables members;           // the tuple, that holds the member information

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

  EXPECT_EQUAL(s.d(), 1.234);
  EXPECT_EQUAL(s.i(), 4711);
  EXPECT_EQUAL(s.s(), "Some text");
  std::array<int, 5> expected_a{1, 2, 3, 4, 5};
  EXPECT_EQUAL(s.a(), expected_a);
  std::vector<std::string> expected_v{"One", "Two", "Three"};
  EXPECT_EQUAL(s.v(), expected_v);
}

// --------------------------------------------------------------------------
struct MyStruct2 : public basic_struct<int64, text> {   // make your struct a subclass of basic_struct and tell it your property types.
  typedef basic_struct<int64, text> super;              // just for more convenince...

  MyStruct2 (int64_t i_ = {}, const std::string s_ = {})
    : super(i, s)                                       // register your properties
    , i("i", i_)                                        // give your properties a name
    , s("s", s_)
  {}

  MyStruct2 (const MyStruct2& rhs)                      // The copy operator also has to be redefined
    : MyStruct2()
  {
    super::operator=(rhs);
  }

  int64 i;                                              // predefined properties for build in types.
  text s;                                               // A string is now a text to avoid nameing collisions
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

  EXPECT_EQUAL(s.i(), 4711);
  EXPECT_EQUAL(s.s(), "Some other text");
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

