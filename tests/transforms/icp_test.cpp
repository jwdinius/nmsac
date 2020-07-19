//! c/c++ headers
#include <string>
#include <fstream>
#include <streambuf>
//! dependency headers
#include <nlohmann/json.hpp>
//! googletest
#include "gtest/gtest.h"
//! project headers
#include "transforms/icp/icp.hpp"
//! definitions for tests
#include "TestData.h"
#include "test_utilities.hpp"

using json = nlohmann::json;

//! The fixture for testing class svd.
class ICPTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   ICPTest() : data_path_(DATA_PATH) {
     // You can do set-up work for each test here.
   }

   ~ICPTest() override {
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

TEST_F(ICPTest, PerturbSourcePointsWithNoise) {
  //! set the random seed for repeatability
  arma::arma_rng::set_seed(11011);
  double const & spread = 10;
  arma::mat dst = spread * arma::randn(3, 20);
  arma::vec3 const dst_c = arma::mean(dst, 1);
  arma::vec3 const src_c = {2, 3, 0};

  //! yaw, pitch, roll
  double const & psi = M_PI / 4;
  double const & theta = M_PI / 3;
  double const & phi = M_PI / 8;

  //! rpy sequence
  arma::mat33 R;
  make_euler(psi, theta, phi, R);

  //! create source points: R*(dst - dst_c) + src_c
  arma::mat const src( R * (dst - arma::repmat(dst_c, 1, dst.n_cols))
      + arma::repmat(src_c, 1, dst.n_cols) );

  //! algorithm arguments
  size_t const & max_its = 20;
  double const & tol = FLOAT_TOL;
  double const & rej_ratio = 0.1;

  //! noise parameters
  double const & angle_noise = 0.05;  // ~3deg 1-sigma
  double const & pos_noise = 0.1;

  //! add noise to destination points
  //!  angle
  arma::vec3 angle_n(arma::fill::randn);
  angle_n *= angle_noise;
  arma::mat33 Rn;
  make_euler(angle_n(0), angle_n(1), angle_n(2), Rn);
  //!  position
  dst += pos_noise * arma::randn(dst.n_rows, dst.n_cols);

  //! initial guess for transformation - icp algorithm needs good initial guess to be successful
  arma::mat44 H_init(arma::fill::eye);
  H_init(arma::span(0, 2), arma::span(0, 2)) = Rn * R.t();
  H_init(arma::span(0, 2), 3) = dst_c - Rn * R.t() * src_c;

  //! allocate nominal output
  arma::mat44 H_opt;
  {
    //! TEST CASE 1: nominal (no reordering)
    ASSERT_TRUE( transforms::iterative_closest_point(src, dst, H_init, max_its,
          tol, rej_ratio, H_opt) );
    arma::vec3 angles;
    arma::mat33 R_opt = H_opt(arma::span(0, 2), arma::span(0, 2));
    arma::vec3 t_opt = H_opt(arma::span(0, 2), 3);
    find_euler_angles(R_opt.t(), angles);
    ASSERT_TRUE(std::abs(angles(0) - phi) < 3 * angle_noise);
    ASSERT_TRUE(std::abs(angles(1) - theta) < 3 * angle_noise);
    ASSERT_TRUE(std::abs(angles(2) - psi) < 3 * angle_noise);
    ASSERT_TRUE(arma::approx_equal(t_opt, dst_c - R.t() * src_c, "absdiff", 3 * pos_noise));
  }
  {
    //! TEST CASE 2: nominal test with random point ordering
    //! (removes implicit correspondence in construction)
    arma::uvec const ordering = arma::randperm(dst.n_cols, dst.n_cols);
    arma::mat const dst_shuffled = dst.cols( ordering );
    arma::vec3 angles;
    arma::mat33 R_opt = H_opt(arma::span(0, 2), arma::span(0, 2));
    arma::vec3 t_opt = H_opt(arma::span(0, 2), 3);
    ASSERT_TRUE( transforms::iterative_closest_point(src, dst_shuffled, H_init, max_its,
          tol, rej_ratio, H_opt) );
    find_euler_angles(R_opt.t(), angles);
    ASSERT_TRUE(std::abs(angles(0) - phi) < 3 * angle_noise);
    ASSERT_TRUE(std::abs(angles(1) - theta) < 3 * angle_noise);
    ASSERT_TRUE(std::abs(angles(2) - psi) < 3 * angle_noise);
    ASSERT_TRUE(arma::approx_equal(t_opt, dst_c - R.t() * src_c, "absdiff", 3 * pos_noise));
  }
}

TEST_F(ICPTest, CompareToMatlabImpl) {
  //! load unit test data from json
  std::ifstream ifs(data_path_ + "/icp.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! TM - source points
  auto const & rows_M = json_data["TM"].size();
  auto const & cols_M = json_data["TM"][0].size();
  size_t i = 0;
  arma::mat  src_pts(rows_M, cols_M);
  for (auto const & it : json_data["TM"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      src_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! B - destination points (to map src_pts onto)
  auto const & rows_B = json_data["B"].size();
  auto const & cols_B = json_data["B"][0].size();
  i = 0;
  arma::mat dst_pts(rows_B, cols_B);
  for (auto const & it : json_data["B"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      dst_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! Ricp - optimal rotation
  i = 0;
  arma::mat33 R_opt_matlab;
  for (auto const & it : json_data["Ricp"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      R_opt_matlab(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! Ticp - optimal translation
  i = 0;
  arma::vec3 t_opt_matlab;
  for (auto const & it : json_data["Ticp"]) {
    t_opt_matlab(i) = static_cast<double>(it[0]);
    ++i;
  }

  //! combine above into homogeneous representation
  arma::mat44 H_opt_matlab(arma::fill::eye);
  H_opt_matlab(arma::span(0, 2), arma::span(0, 2)) = R_opt_matlab;
  H_opt_matlab(arma::span(0, 2), 3) = t_opt_matlab;

  //! algorithm arguments
  size_t const & max_its = 100;
  double const & tol = 1e-12;
  double const & rej_ratio = 0.1;

  //! TEST CASE 1: nominal call from matlab implementation
  //! - unit test data has initial guess included, so pass identity as H_init
  arma::mat44 H_init(arma::fill::eye);
  //! allocate container for output
  arma::mat44 H_opt;
  ASSERT_TRUE( transforms::iterative_closest_point(src_pts, dst_pts,
        H_init, max_its, tol, rej_ratio, H_opt) );
  ASSERT_TRUE( arma::approx_equal(H_opt, H_opt_matlab, "absdiff", 5e-3) );
}

TEST_F(ICPTest, TestFromNMSAC) {
  //! load unit test data from json
  std::ifstream ifs(data_path_ + "/points.json");
  std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
      std::istreambuf_iterator<char>());
  json json_data = json::parse(json_str);

  //! source points
  auto const & rows_src = json_data["source_pts"].size();
  auto const & cols_src = json_data["source_pts"][0].size();
  size_t i = 0;
  arma::mat  src_pts(rows_src, cols_src);
  for (auto const & it : json_data["source_pts"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      src_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! target points (to map src_pts onto)
  auto const & rows_tgt = json_data["target_pts"].size();
  auto const & cols_tgt = json_data["target_pts"][0].size();
  i = 0;
  arma::mat dst_pts(rows_tgt, cols_tgt);
  for (auto const & it : json_data["target_pts"]) {
    size_t j = 0;
    for (auto const & jt : it) {
      dst_pts(i, j) = static_cast<double>(jt);
      ++j;
    }
    ++i;
  }

  //! CANDIDATE STARTING TRANSFORM FROM nmsac
  arma::mat44 H_init(arma::fill::eye);
  H_init << -0.5186 << -0.6399 << -0.5670 <<-0.4231 << arma::endr
    << 0.5552 << -0.7564 << 0.3459 << -1.8731 << arma::endr
    << -0.6502 << -0.1354 << 0.7476 << 2.8520 << arma::endr
    << 0 << 0 << 0 << 1.0000 << arma::endr;

  //! algorithm arguments
  size_t const & max_its = 50;
  double const & tol = 1e-8;
  double const & rej_ratio = 0.3;  // to converge in fewer iterations, increase this number

  //! TEST CASE 1: nominal call from matlab implementation
  //! allocate container for output
  arma::mat44 H_opt;
  ASSERT_TRUE( transforms::iterative_closest_point(src_pts, dst_pts, H_init, max_its,
        tol, rej_ratio, H_opt) );
}
