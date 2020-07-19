//! c/c++ headers
#include <string>
#include <fstream>
#include <streambuf>
#include <utility>
//! dependency headers
#include <nlohmann/json.hpp>
//! googletest
#include "gtest/gtest.h"
//! project headers
#include "correspondences/common/types.hpp"  // for correspondences_t defn
#include "transforms/common/utilities.hpp"  // for enumerate defn
#include "transforms/svd/svd.hpp"
//! definitions for tests
#include "TestData.h"
#include "test_utilities.hpp"

namespace cor = correspondences;
using json = nlohmann::json;

//! The fixture for testing class svd.
class SVDTest : public ::testing::Test {
 protected:
  /**
   * constants for test
   */
  // You can remove any or all of the following functions if their bodies would
  // be empty.

  SVDTest() {
     // You can do set-up work for each test here.
  }

  ~SVDTest() override {
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

TEST_F(SVDTest, EmptyCorrespondences) {
    arma::arma_rng::set_seed(11011);
    double const & spread = 10;
    arma::mat const dst = spread * arma::randn(3, 20);
    arma::vec3 const dst_c = arma::vec3(arma::mean(dst, 1));
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

    //! allocate nominal output
    arma::mat44 H_opt;

    //! TEST CASE 1: nominal
    ASSERT_TRUE( transforms::best_fit_transform(src, dst, {}, H_opt) );

    //! transform source points by the found transformation H = [R; t; 0 1] and check for (near) equality
    arma::mat33 R_opt = H_opt(arma::span(0, 2), arma::span(0, 2));
    arma::vec3 t_opt = H_opt(arma::span(0, 2), 3);
    arma::mat const src_xform( R_opt * src + arma::repmat(t_opt, 1, dst.n_cols) );
    ASSERT_TRUE( arma::approx_equal(dst, src_xform, "absdiff", FLOAT_TOL) );
}

TEST_F(SVDTest, Correspondences) {
    arma::arma_rng::set_seed(11011);
    double const & spread = 10;

    arma::mat const dst = spread * arma::randn(3, 20);
    arma::vec3 const dst_c = arma::vec3(arma::mean(dst, 1));
    arma::vec3 const src_c = {2, 3, 0};

    //! yaw, pitch, roll
    double const & psi = M_PI / 4;
    double const & theta = M_PI / 3;
    double const & phi = M_PI / 8;

    //! rpy sequence
    arma::mat33 R;
    make_euler(psi, theta, phi, R);

    //! create source points (before reordering): R*(dst - dst_c) + src_c
    arma::mat const src_before( R * (dst - arma::repmat(dst_c, 1, dst.n_cols))
            + arma::repmat(src_c, 1, dst.n_cols) );

    //! create random order for source points (and shuffle columns into that order)
    arma::uvec const ordering = arma::randperm(20);
    arma::mat const src = src_before.cols(ordering);

    //! create correspondences array
    cor::correspondences_t corrs;
    for (auto [src_idx, dst_idx] : transforms::enumerate(ordering)) {
       auto const p = std::pair(src_idx, dst_idx);  // remember, src was reordered
       corrs[p] = 1.0;  // insert arbitrary correspondence score
    }

    //! allocate nominal output
    arma::mat44 H_opt;

    //! TEST CASE 1: nominal
    ASSERT_TRUE( transforms::best_fit_transform(src, dst, corrs, H_opt) );

    //! transform source points by the found transformation H = [R; t; 0 1] and check for (near) equality
    arma::mat33 R_opt = H_opt(arma::span(0, 2), arma::span(0, 2));
    arma::vec3 t_opt = H_opt(arma::span(0, 2), 3);
    arma::mat const src_xform( R_opt * src_before + arma::repmat(t_opt, 1, dst.n_cols) );  // NOTE(jwd): use pre-shuffle ordering for comparison
    ASSERT_TRUE( arma::approx_equal(dst, src_xform, "absdiff", FLOAT_TOL) );
}
