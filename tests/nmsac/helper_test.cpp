//! c/c++ headers
#include <fstream>
#include <streambuf>
#include <string>
//! dependency headers
#include <nlohmann/json.hpp>
//! googletest
#include "gtest/gtest.h"
//! project headers
#include "nmsac/types.hpp"
//! unit-under-test header
#include "nmsac/helper.hpp"
//! definition of test data
#include "TestData.h"

using namespace nmsac;

using json = nlohmann::json;

//! The fixture for testing class.
class HelperTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   HelperTest() : data_path_(DATA_PATH) {
     // You can do set-up work for each test here.
   }

   ~HelperTest() override {
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

TEST_F(HelperTest, CountCorrespondences) {
  //! load unit test data from json
  std::ifstream ifs(data_path_ + "/count-correspondences.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! tgt points
  auto const & tgt_rows = json_data["B"].size();
  auto const & tgt_cols = json_data["B"][0].size();
  arma::mat tgt_pts_matlab(tgt_rows, tgt_cols);
  size_t i = 0;
  for (auto const & it : json_data["B"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      tgt_pts_matlab(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! transformed source points (after icp application)
  auto const & src_rows = json_data["TMICP"].size();
  auto const & src_cols = json_data["TMICP"][0].size();
  arma::mat src_pts_xform_matlab(src_rows, src_cols);
  i = 0;
  for (auto const & it : json_data["TMICP"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      src_pts_xform_matlab(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! epsilon
  auto const eps = static_cast<double>(json_data["epsilon"]);

  //! no. of inliers from matlab
  auto const n_inliers_matlab = static_cast<size_t>(json_data["inls_icp"]);

  transforms::KDTreeSearcher tgt_searcher(tgt_pts_matlab);

  //! TEST CASE 1: nominal call
  auto const n_inliers = count_correspondences(src_pts_xform_matlab, tgt_searcher, eps);

  //! check equality of output
  ASSERT_TRUE(n_inliers == n_inliers_matlab);
}
