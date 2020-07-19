//! c/c++ headers
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
//! dependency headers
#include <nlohmann/json.hpp>
//! googletest
#include "gtest/gtest.h"
//! project headers
#include "correspondences/common/utilities.hpp"
//! definition of test data
#include "TestData.h"

namespace cor = correspondences;

using json = nlohmann::json;

//! The fixture for testing class Common.
class CommonTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   CommonTest() : data_path_(DATA_PATH) {
     // You can do set-up work for each test here.
   }

   ~CommonTest() override {
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

TEST_F(CommonTest, LinearProgrammingTest) {
  //! load unit test data from json
  std::ifstream ifs(data_path_ + "/linear-programming.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! c
  auto const & rows_c = json_data["c"].size();
  size_t i = 0;
  arma::colvec c(rows_c);
  for (auto const & it : json_data["c"]) {
    c(i) = static_cast<double>(it[0]);
    ++i;
  }

  //! A
  auto const & rows_A = json_data["A"].size();
  auto const & cols_A = json_data["A"][0].size();
  i = 0;
  arma::mat A(rows_A, cols_A);
  for (auto const & it : json_data["A"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      A(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! b
  auto const & rows_b = json_data["b"].size();
  i = 0;
  arma::colvec b(rows_b);
  for (auto const & it : json_data["b"]) {
    b(i) = static_cast<double>(it[0]);
    ++i;
  }

  //! lower- and upper-bounds
  auto const lb = static_cast<double>(json_data["lb"]);
  auto const ub = static_cast<double>(json_data["ub"]);

  //! TEST 1: nominal call
  arma::colvec x_opt(rows_c);
  ASSERT_TRUE( cor::linear_programming(c, A, b, lb, ub, x_opt) );

  //! solution (from matlab)
  auto const & rows_x_matlab = json_data["Xlp"].size();
  i = 0;
  arma::colvec x_opt_matlab(rows_x_matlab);
  for (auto const & it : json_data["Xlp"]) {
    x_opt_matlab(i) = static_cast<double>(it[0]);
    ++i;
  }

  /**
   * the objective value found is a better measure of solution quality than
   * a value-for-value comparison due to truncated precision on unit test input
   */
  double const obj_val_matlab = arma::dot(c, x_opt_matlab);
  double const obj_val = arma::dot(c, x_opt);
  EXPECT_DOUBLE_EQ(obj_val, obj_val_matlab);
}
