//! c/c++ headers
//! dependency headers
//! googletest
#include "gtest/gtest.h"
//! project headers
//! unit-under-test header
#include "nmsac/sampling.hpp"
//! definition of test data
#include "TestData.h"

using namespace nmsac;

//! The fixture for testing class.
class SamplingTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   SamplingTest() {
     // You can do set-up work for each test here.
   }

   ~SamplingTest() override {
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
   const std::string data_path_;
};

TEST_F(SamplingTest, Reordering) {
  arma::arma_rng::set_seed(11011);
  arma::mat X(3, 6);
  X << 1 << 2 << 3 << 4 << 5 << 6 << arma::endr
    << 7 << 8 << 9 << 10 << 11 << 12 << arma::endr
    << 13 << 14 << 15 << 16 << 17 << 18 << arma::endr;

  //! no. of columns to sample
  size_t const n_cols = 4;

  //! the chosen seed maps indices {0, 1, 2, 3, 4, 5} -> {3, 1, 4, 0, 5, 2}
  //! the matrix X with columns reordered is then
  arma::mat X_reord(3, 6);
  X_reord << 4 << 2 << 5 << 1 << 6 << 3 << arma::endr
    << 10 << 8 << 11 << 7 << 12 << 9 << arma::endr
    << 16 << 14 << 17 << 13 << 18 << 15 << arma::endr;

  //! randomly sample specified number of columns based upon rng state
  arma::mat const ord_mat = sample_cols(X, n_cols);

  //! TEST CASE 1: check that output of call is correct
  arma::mat const X_sampled = X_reord.cols( arma::span(0, n_cols-1) );
  ASSERT_TRUE(arma::approx_equal(X_sampled, ord_mat, "absdiff", FLOAT_TOL));

  //! TEST CASE 2: check that remaining columns reference is updated correctly
  //! check that size is correct
  ASSERT_TRUE(X.n_rows == X_reord.n_rows && X.n_cols == X_reord.n_cols-n_cols);
  //! check that output is correct
  arma::mat const X_removed = X_reord.cols( arma::span(n_cols, X_reord.n_cols-1) );
  ASSERT_TRUE(arma::approx_equal(X_removed, X, "absdiff", FLOAT_TOL));
}
