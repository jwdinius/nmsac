//! c/c++ headers
#include <string>
#include <fstream>
#include <streambuf>
//! dependency headers
#include <nlohmann/json.hpp>
//! boost boilerplate
#define BOOST_TEST_MODULE TestMain test
#include <boost/test/unit_test.hpp>
//! project headers
#include "nmsac/main.hpp"

using json = nlohmann::json;

/**
 * constants for test
 */
static const std::string data_path("../tests/data");  // NOLINT [runtime/string]
static const double FLOAT_TOL(1e-7);

//! this is a special test-case - there are no outliers by construction
BOOST_AUTO_TEST_CASE( cube_test ) {
    //! load unit test data from json
    //! NOTE: this test data was generated without adding noise
    std::ifstream ifs(data_path + "/cube-test.json");
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
    auto const config = nmsac::ConfigNMSAC();
    arma::mat33 R_opt;
    arma::vec3 t_opt;
    size_t num_inliers, its;
    //! TEST 1: check that the call was successful
    BOOST_CHECK( nmsac::main(src_pts, tgt_pts, config, R_opt, t_opt, num_inliers, its) );

    //! TEST 2: check that transformations are close to truth
    BOOST_CHECK(arma::approx_equal(R_opt, _R, "absdiff", FLOAT_TOL));
    BOOST_CHECK(arma::approx_equal(t_opt, _t, "absdiff", FLOAT_TOL));

    //! TEST 3: check that num_inliers = size(src_pts)
    BOOST_CHECK(num_inliers == src_pts.n_cols);
}
