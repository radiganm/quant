// test1.cxx
// Mac Radigan

#define BOOST_TEST_MODULE test1
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(set1)

BOOST_AUTO_TEST_CASE(test1) {
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test2) {
  BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

// *EOF*
