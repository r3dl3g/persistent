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

  prop::type<double> d;                     // Each member is now a prop<Type> memeber.
  prop::type<int> i;
  prop::type<std::string> s;
  prop::type<std::array<int, 5>> a;
  prop::type<std::vector<std::string>> v;

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
struct MyStruct2 : public basic_struct<prop::int64, prop::text> {   // make your struct a subclass of basic_struct and tell it your property types.
  typedef basic_struct<prop::int64, prop::text> super;              // just for more convenince...

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

  prop::int64 i;                                              // predefined properties for build in types.
  prop::text s;                                               // A string is now a text to avoid nameing collisions
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

// ----------------------------------------------------------------------------------------------------------------------------------------------------
struct easy_test_base {
  prop::int64 i;
  prop::text t;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------
struct easy_test_t : public basic_container {

  easy_test_t (int64_t i_ = {}, const std::string& t_ = {})
    : i("i", i_)
    , t("t", t_)
  {}

  prop::int64 i;
  prop::text t;

  auto get_members () { return std::tie(i, t); }
  const auto get_members () const { return std::tie(i, t); }

};

// --------------------------------------------------------------------------
void easy_test () {

  easy_test_t s{4711, "Some easy text"};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"i\":\"4711\",\"t\":\"Some easy text\"}");
}

// --------------------------------------------------------------------------
void easy_test_2 () {

  easy_test_t s{4711, "Some easy text"};
  easy_test_t s2 = s;

  std::ostringstream os;
  persistent::io::write_json(os, s2, false);

  EXPECT_EQUAL(os.str(), "{\"i\":\"4711\",\"t\":\"Some easy text\"}");
}

// --------------------------------------------------------------------------
struct fixed_test_t : public basic_container {

  static constexpr char s_i[] = "i";
  static constexpr char s_t[] = "t";

  fixed_test_t (int64_t i_ = {}, double t_ = {})
    : i(i_)
    , t(t_)
  {}

  prop_t::type<int64_t, s_i> i;
  prop_t::type<double, s_t> t;

  auto get_members () { return std::tie(i, t); }
  const auto get_members () const { return std::tie(i, t); }

};

// --------------------------------------------------------------------------
void fixed_write_test () {

  fixed_test_t s{4711, 8.15};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"i\":\"4711\",\"t\":\"8.15\"}");
}

// --------------------------------------------------------------------------
void fixed_read_test () {

  fixed_test_t s;

  std::istringstream is("{\"i\":\"4711\",\"t\":\"8.15\"}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.i(), 4711);
  EXPECT_EQUAL(s.t(), 8.15);
}

// --------------------------------------------------------------------------
void fixed_write_test_2 () {

  fixed_test_t s{4711, 8.15};
  fixed_test_t s2 = s;

  std::ostringstream os;
  persistent::io::write_json(os, s2, false);

  EXPECT_EQUAL(os.str(), "{\"i\":\"4711\",\"t\":\"8.15\"}");
}

// --------------------------------------------------------------------------
struct primitive_test_t : private basic_container {

  primitive_test_t (int64_t i_ = {}, double t_ = {})
    : i(i_)
    , t(t_)
  {}

  int64_t i;
  double t;

  template<typename T> using member = persistent::io::member<T>;

  auto get_members () {
    return std::make_tuple(member(i, "i"), member(t, "t"));
  }

  const auto get_members () const {
    return (const_cast<primitive_test_t&>(*this)).get_members();
  }

};

// --------------------------------------------------------------------------
void primitive_write_test () {

  primitive_test_t s{4711, 8.15};

  std::ostringstream os;
  persistent::io::write_json(os, s, false);

  EXPECT_EQUAL(os.str(), "{\"i\":\"4711\",\"t\":\"8.15\"}");
}

// --------------------------------------------------------------------------
void primitive_read_test () {

  primitive_test_t s;

  std::istringstream is("{\"i\":\"4711\",\"t\":\"8.15\"}");
  persistent::io::read_json(is, s);

  EXPECT_EQUAL(s.i, 4711);
  EXPECT_EQUAL(s.t, 8.15);
}
// --------------------------------------------------------------------------
void test_main (const testing::start_params& params) {
  testing::log_info("Running " __FILE__);
  run_test(test_write);
  run_test(test_read);

  run_test(test_write2);
  run_test(test_read2);
  run_test(easy_test);
  run_test(easy_test_2);
  run_test(fixed_write_test);
  run_test(fixed_write_test_2);
  run_test(fixed_read_test);
  run_test(primitive_write_test);
  run_test(primitive_read_test);

  testing::log_info(persistent::io::msg_fmt() << "Size of easy_test_t: " << sizeof(easy_test_t)
                                              << ", size of easy_test_2: " << sizeof(easy_test_base));
  testing::log_info(persistent::io::msg_fmt() << "Size of fixed_test_t: " << sizeof(fixed_test_t));
}

// --------------------------------------------------------------------------

