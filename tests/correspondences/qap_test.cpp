//! c/c++ headers
#include <string>
#include <fstream>
#include <memory>
#include <streambuf>
//! dependency headers
#include <nlohmann/json.hpp>
//! googletest
#include "gtest/gtest.h"
//! unit-under-test header
#include "correspondences/qap/qap.hpp"
//! definition of test data
#include "TestData.h"

namespace cor = correspondences;
namespace cq = cor::qap;

using json = nlohmann::json;

//! The fixture for testing class qap.
class QAPTest : public ::testing::Test {
 protected:
  /**
   * constants for test
   */
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  QAPTest() : data_path_(DATA_PATH) {
     // You can do set-up work for each test here.
  }

  ~QAPTest() override {
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

TEST_F(QAPTest, FullSourceMatching)
{
  //! load unit test data from json
  //! NOTE: this test data was generated without adding noise
  std::ifstream ifs(data_path_ + "/registration-data.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! source pts (read first so that we can use min_corr == no. of source points)
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

  //! setup configuration struct for test
  cq::Config config;
  config.epsilon = 0.1;
  config.pairwise_dist_threshold = 0.1;
  config.corr_threshold = 0.9;
  config.n_pair_threshold = 100;
  config.min_corr = src_pts.n_cols;

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

  //! correspondences
  i = 0;
  cor::correspondences_t _corrs;
  for (auto const & it : json_data["correspondences"]) {
    auto key = std::make_pair(i, static_cast<size_t>(it));
    _corrs[key] = 1;
    ++i;
  }

  cor::correspondences_t corrs;
  std::unique_ptr<cor::CorrespondencesBase> qap = std::make_unique<cor::QAP>(src_pts, tgt_pts, config);
  ASSERT_TRUE( qap->calc_correspondences(corrs) == cor::status_e::success );

  //! checking that key is present in both correspondence sets is enough; see `find_correspondences` implementation
  for (auto const & c : _corrs) {
    auto key = c.first;
    ASSERT_TRUE(corrs.find(key) != corrs.end());
  }
}

TEST_F(QAPTest, PartialSourceMatching)
{
  //! load unit test data from json
  //! NOTE: this test data was generated without adding noise
  std::ifstream ifs(data_path_ + "/registration-data-mincorr.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! setup configuration struct for test
  cq::Config config;
  config.epsilon = 0.1;
  config.pairwise_dist_threshold = 0.1;
  config.corr_threshold = 0.9;
  config.n_pair_threshold = 100;
  config.min_corr = static_cast<size_t>(json_data["min_corr"]);

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

  //! correspondences
  i = 0;
  cor::correspondences_t _corrs;
  for (auto const & it : json_data["correspondences"]) {
    auto key = std::make_pair(i, static_cast<size_t>(it));
    _corrs[key] = 1;
    ++i;
  }

  cor::correspondences_t corrs;
  std::unique_ptr<cor::CorrespondencesBase> qap = std::make_unique<cor::QAP>(src_pts, tgt_pts, config);
  ASSERT_TRUE( qap->calc_correspondences(corrs) == cor::status_e::success );

  //! checking that key is present in both correspondence sets is enough; see `find_correspondences` implementation
  for (auto const & c : _corrs) {
    auto key = c.first;
    ASSERT_TRUE(corrs.find(key) != corrs.end());
  }
}
