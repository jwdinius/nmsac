//! c/c++ headers
#include <string>
#include <fstream>
#include <streambuf>
//! googletest
#include "gtest/gtest.h"
//! dependency headers
#include "TestData.h"  // unit test configuration data (generated by CMake)
#include <nlohmann/json.hpp>
//! unit-under-test header
#include "nmsac/main.hpp"

using json = nlohmann::json;

//! The fixture for testing class.
class MainTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   MainTest() : data_path_(DATA_PATH) {
     // You can do set-up work for each test here.
   }

   ~MainTest() override {
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

//! this is a special test-case - there are no outliers by construction
TEST_F(MainTest, CubeTestQap) {
  //! load unit test data from json
  //! NOTE: this test data was generated without adding noise
  std::ifstream ifs(data_path_ + "/cube-test.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! source pts
  auto const rows_S = json_data["source_pts"].size();
  auto const cols_S = json_data["source_pts"][0].size();
  size_t i = 0;
  arma::mat src_pts(rows_S, cols_S);
  for (auto const & it : json_data["source_pts"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      src_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! target pts
  auto const rows_T = json_data["target_pts"].size();
  auto const cols_T = json_data["target_pts"][0].size();
  i = 0;
  arma::mat tgt_pts(rows_T, cols_T);
  for (auto const & it : json_data["target_pts"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      tgt_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! true rotation
  i = 0;
  arma::mat33 _R;
  for (auto const & it : json_data["R_true"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      _R(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! true translation
  i = 0;
  arma::vec3 _t;
  for (auto const & it : json_data["t_true"]) {
    _t(i) = static_cast<double>(it);
    ++i;
  }

  //! make the call
  nlohmann::json json_config = {
    { "qAp", { /* this will test json_utils::convert_keys */
               {"epsilon", 0.015},
               {"pairwise_dist_threshold", 1e-2},
               {"corr_threshold", 0.9},
               {"n_pair_threshold", 5},
               {"min_corr", 4}
             }
    }
  };
  arma::mat33 R_opt;
  arma::vec3 t_opt;
  size_t num_inliers, its;
  //! TEST 1: check that the call was successful
  ASSERT_TRUE( nmsac::main(src_pts, tgt_pts, json_config, R_opt, t_opt, num_inliers, its) );

  //! TEST 2: check that transformations are close to truth
  ASSERT_TRUE(arma::approx_equal(R_opt, _R, "absdiff", FLOAT_TOL));
  ASSERT_TRUE(arma::approx_equal(t_opt, _t, "absdiff", FLOAT_TOL));

  //! TEST 3: check that num_inliers = size(src_pts)
  ASSERT_TRUE(num_inliers == src_pts.n_cols);
}

//! this is a special test-case - there are no outliers by construction
TEST_F(MainTest, CubeTestMc) {
  //! load unit test data from json
  //! NOTE: this test data was generated without adding noise
  std::ifstream ifs(data_path_ + "/cube-test.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! source pts
  auto const rows_S = json_data["source_pts"].size();
  auto const cols_S = json_data["source_pts"][0].size();
  size_t i = 0;
  arma::mat src_pts(rows_S, cols_S);
  for (auto const & it : json_data["source_pts"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      src_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! target pts
  auto const rows_T = json_data["target_pts"].size();
  auto const cols_T = json_data["target_pts"][0].size();
  i = 0;
  arma::mat tgt_pts(rows_T, cols_T);
  for (auto const & it : json_data["target_pts"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      tgt_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! true rotation
  i = 0;
  arma::mat33 _R;
  for (auto const & it : json_data["R_true"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      _R(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! true translation
  i = 0;
  arma::vec3 _t;
  for (auto const & it : json_data["t_true"]) {
    _t(i) = static_cast<double>(it);
    ++i;
  }

  //! make the call
  nlohmann::json json_config = {
    { "mc", {
               {"epsilon", 0.015},
               {"pairwise_dist_threshold", 1e-2},
               {"algorithm", 0}
             }
    }
  };
  arma::mat33 R_opt;
  arma::vec3 t_opt;
  size_t num_inliers, its;
  //! TEST 1: check that the call was successful
  ASSERT_TRUE( nmsac::main(src_pts, tgt_pts, json_config, R_opt, t_opt, num_inliers, its) );

  //! TEST 2: check that transformations are close to truth
  ASSERT_TRUE(arma::approx_equal(R_opt, _R, "absdiff", FLOAT_TOL));
  ASSERT_TRUE(arma::approx_equal(t_opt, _t, "absdiff", FLOAT_TOL));

  //! TEST 3: check that num_inliers = size(src_pts)
  ASSERT_TRUE(num_inliers == src_pts.n_cols);
}
