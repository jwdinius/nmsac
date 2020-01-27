//! c/c++ headers
#include <fstream>
#include <streambuf>
#include <string>
//! dependency headers
#include <nlohmann/json.hpp>
//! boost boilerplate
#define BOOST_TEST_MODULE TestHelperUtils test
#include <boost/test/unit_test.hpp>
//! project headers
#include "nmsac/types.hpp"
//! unit-under-test header
#include "nmsac/helper-utils.hpp"

using namespace nmsac;

using json = nlohmann::json;

/**
 * constants for test
 */
static const std::string data_path("../tests/data");  // NOLINT [runtime/string]
static const double FLOAT_TOL(1e-7);

BOOST_AUTO_TEST_CASE( count_correspondences_test ) {
    //! load unit test data from json
    std::ifstream ifs(data_path + "/countCorrespondences.json");
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

    KDTreeSearcher tgt_searcher(tgt_pts_matlab);

    //! TEST CASE 1: nominal call
    auto const n_inliers = count_correspondences(src_pts_xform_matlab, tgt_searcher, eps);

    //! check equality of output
    BOOST_CHECK(n_inliers == n_inliers_matlab);
}
