
#include "persistent/persistent_ini.h"
#include <testing/testing.h>

#include "test_structs.h"

// --------------------------------------------------------------------------
void test_read_empty () {

  test_int64 t1;
  std::istringstream is("");
  persistent::io::read_ini(is, t1);

  EXPECT_EQUAL(t1.i(), 0);
  EXPECT_EQUAL(t1.j(), 0);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_array () {

  persistent::fix_list<int64_t, 5> a("a");
  std::istringstream is("a.0=1\n"
                        "a.1=2\n"
                        "a.2=3\n"
                        "a.3=4\n"
                        "a.4=5\n");
  persistent::io::read_ini(is, a);

  std::array<int64_t, 5> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(a(), expected);
}
// --------------------------------------------------------------------------
void test_read_vector () {

  persistent::list<int64_t> v("v");
  std::istringstream is("v.0=1\n"
                        "v.1=2\n"
                        "v.2=3\n"
                        "v.3=4\n"
                        "v.4=5\n");
  persistent::io::read_ini(is, v);

  std::vector<int64_t> expected = {1, 2, 3, 4, 5};
  EXPECT_EQUAL(v(), expected);
}

// --------------------------------------------------------------------------
void test_read_1 () {

  persistent::int64 i("i");
  std::istringstream is("i=4711\n");
  persistent::io::read_ini(is, i);

  EXPECT_EQUAL(i(), 4711);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_2 () {

  test_int64 t1;
  std::istringstream is("i=4711\n"
                        "j=815\n");
  persistent::io::read_ini(is, t1);

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_EQUAL(t1.j(), 815);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_3 () {

  test_int64 t1;
  std::istringstream is("i=4711\n");
  persistent::io::read_ini(is, t1);

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_4 () {

  test_int64 t1;
  std::istringstream is(" \n \t \n \t i \t = \t 4711 \n \t \n \t ");
  persistent::io::read_ini(is, t1);

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_EQUAL(t1.j(), 0);
  EXPECT_TRUE(is.eof());
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_5 () {
  test2 t2;
  std::istringstream is("i1=815\n"
                        "t1.i=911\n"
                        "t1.j=203\n"
                        "i2=4711\n");
  persistent::io::read_ini(is, t2);

  EXPECT_EQUAL(t2.i1(), 815);
  EXPECT_EQUAL(t2.t1().i(), 911);
  EXPECT_EQUAL(t2.t1().j(), 203);
  EXPECT_EQUAL(t2.i2(), 4711);
}

// --------------------------------------------------------------------------
void test_read_6 () {

  test_int64 t1;
  std::istringstream is("i=4711\n"
                        "k=815\n");
  try {
    persistent::io::read_ini(is, t1);
    EXPECT_FALSE("Exception expected");
  } catch (std::exception& ex) {
    EXPECT_TRUE("Exception expected");
  }

  EXPECT_EQUAL(t1.i(), 4711);
  EXPECT_EQUAL(t1.j(), 0);
  EXPECT_FALSE(is.good());
}

// --------------------------------------------------------------------------
void test_read_7 () {

  test3 t3;
  std::istringstream is("v.0.i=1\n"
                        "v.0.j=2\n"
                        "v.1.i=3\n"
                        "v.1.j=4\n"
                        "v.2.i=5\n"
                        "v.2.j=6\n");
  persistent::io::read_ini(is, t3);

  EXPECT_EQUAL(t3.v().size(), 3);
  EXPECT_EQUAL(t3.v()[0].i(), 1);
  EXPECT_EQUAL(t3.v()[0].j(), 2);
  EXPECT_EQUAL(t3.v()[1].i(), 3);
  EXPECT_EQUAL(t3.v()[1].j(), 4);
  EXPECT_EQUAL(t3.v()[2].i(), 5);
  EXPECT_EQUAL(t3.v()[2].j(), 6);
}

// --------------------------------------------------------------------------
void test_read_8 () {

  test4 t4;
  std::istringstream is("i=4711\n"
                        "i.0=1\n"
                        "i.1=2\n"
                        "i.2=3\n");
  persistent::io::read_ini(is, t4);

  EXPECT_EQUAL(t4.i(), 4711);
  EXPECT_EQUAL(t4.l()[0], 1);
  EXPECT_EQUAL(t4.l()[1], 2);
  EXPECT_EQUAL(t4.l()[2], 3);
}

// --------------------------------------------------------------------------
void test_write_1 () {
  persistent::int64 i("i", 4711);
  std::ostringstream os;
  persistent::io::write_ini(os, i);

  EXPECT_EQUAL(os.str(), "i=4711\n");
}

// --------------------------------------------------------------------------
void test_write_2 () {
  test_int64 t1;
  std::ostringstream os;
  persistent::io::write_ini(os, t1);

  EXPECT_EQUAL(os.str(), "i=0\n"
                         "j=0\n");
}

// --------------------------------------------------------------------------
void test_write_3 () {
  test2 t2;
  std::ostringstream os;
  persistent::io::write_ini(os, t2);

  EXPECT_EQUAL(os.str(), "i1=0\n"
                         "t1.i=0\n"
                         "t1.j=0\n"
                         "i2=0\n");
}

// --------------------------------------------------------------------------
void test_write_4 () {
  test3 t3;
  t3.v().push_back(test_int64());
  std::ostringstream os;
  persistent::io::write_ini(os, t3);

  EXPECT_EQUAL(os.str(), "v.0.i=0\n"
                         "v.0.j=0\n");
}

// --------------------------------------------------------------------------
void test_write_5 () {
  test4 t4(4711, {1, 2, 3});
  std::ostringstream os;
  persistent::io::write_ini(os, t4);

  EXPECT_EQUAL(os.str(), "i=4711\n"
                         "i.0=1\n"
                         "i.1=2\n"
                         "i.2=3\n"
               );
}

// --------------------------------------------------------------------------
void test_write_array () {

  persistent::fix_list<int64_t, 5> a("a", {1, 2, 3, 4, 5});
  std::ostringstream os;
  persistent::io::write_ini(os, a);
  EXPECT_EQUAL(os.str(), "a.0=1\n"
                         "a.1=2\n"
                         "a.2=3\n"
                         "a.3=4\n"
                         "a.4=5\n");
}
// --------------------------------------------------------------------------
void test_write_vector () {

  persistent::list<int64_t> v("v", {1, 2, 3, 4, 5});
  std::ostringstream os;
  persistent::io::write_ini(os, v);
  EXPECT_EQUAL(os.str(), "v.0=1\n"
                         "v.1=2\n"
                         "v.2=3\n"
                         "v.3=4\n"
                         "v.4=5\n");
}

// --------------------------------------------------------------------------
void test_main (const testing::start_params& params) {
  testing::log_info("Running " __FILE__);

  run_test(test_write_array);
  run_test(test_write_vector);
  run_test(test_write_1);
  run_test(test_write_2);
  run_test(test_write_3);
  run_test(test_write_4);
  run_test(test_write_5);

  run_test(test_read_empty);
  run_test(test_read_1);
  run_test(test_read_array);
  run_test(test_read_vector);
  run_test(test_read_2);
  run_test(test_read_3);
  run_test(test_read_4);
  run_test(test_read_5);
  run_test(test_read_6);
  run_test(test_read_7);
  run_test(test_read_8);

}

// --------------------------------------------------------------------------

