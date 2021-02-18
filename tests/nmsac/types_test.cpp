//! c/c++ headers
#include <string>
#include <fstream>
#include <streambuf>
//! googletest
#include "gtest/gtest.h"
//! dependency headers
#include <nlohmann/json.hpp>
//! unit-under-test header
#include "nmsac/types.hpp"

using json = nlohmann::json;

//! The fixture for testing class.
class TypesTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   TypesTest() {
     // You can do set-up work for each test here.
   }

   ~TypesTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
   }

   // If the constructor and destructor are not enough for setting up
   // and cleaning up each test, you can define the following methods:

   void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
   }

   void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
   }

   // Class members declared here can be used by all tests in the test suite
   // for Foo.
};

//! this is a special test-case - there are no outliers by construction
TEST_F(TypesTest, ConfigDefault) {
  //! make the call
  nmsac::Config const config = nmsac::Config();

  //! TEST 1: check that default algorithm is qap
  ASSERT_TRUE(config.algorithm == nmsac::algorithms_e::qap);
}
