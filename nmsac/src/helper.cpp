//! c/c++ headers
#include <fstream>
#include <iostream>
#include <limits>
#include <streambuf>
#include <string>
//! dependency headers
#include <nlohmann/json.hpp>
//! project headers
#include "nmsac/helper.hpp"

using json = nlohmann::json;
namespace xfrm = transforms;

bool nmsac::read_config(std::string const & config_file, nmsac::ConfigNMSAC & config) noexcept {
    std::ifstream ifs(config_file);
    std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
            std::istreambuf_iterator<char>());
    json json_data = json::parse(json_str);
    try {
        config.random_seed = static_cast<uint64_t>(json_data["random_seed"]);  // NOLINT [runtime/int]
        config.print_status = static_cast<bool>(json_data["print_status"]);
        config.ps = static_cast<double>(json_data["ps"]);
        config.max_iter = static_cast<size_t>(json_data["max_iter"]);
        config.min_iter = static_cast<size_t>(json_data["min_iter"]);
        config.k = static_cast<size_t>(json_data["k"]);
        config.points_per_sample = static_cast<size_t>(json_data["points_per_sample"]);
        config.epsilon = static_cast<double>(json_data["epsilon"]);
        config.n_pair_thresh = static_cast<size_t>(json_data["n_pair_thresh"]);
        config.pair_dist_thresh = static_cast<double>(json_data["pair_dist_thresh"]);
        config.max_iter_icp = static_cast<size_t>(json_data["max_iter_icp"]);
        config.tol_icp = static_cast<double>(json_data["tol_icp"]);
        config.outlier_rej_icp = static_cast<double>(json_data["outlier_rej_icp"]);
        return true;
    } catch (...) {
        std::cout << "Invalid config file" << std::endl;
        return false;
    }
}

/**
 * @brief Count correspondences between two sets of points 
 *
 * @param [in] src source points
 * @param [in][out] tgt_tree target points (as a KDTree object for nearest-neighbor search)
 * @param [in] epsilon threshold for correspondence counting
 * @return number of correspondences found
 */
size_t nmsac::count_correspondences(arma::mat const & src, xfrm::KDTreeSearcher & tgt_tree, double const & epsilon) noexcept {
    //! if epsilon <= 0, return NaN
    if (epsilon < std::numeric_limits<double>::epsilon()) {
        std::cout << static_cast<std::string>(__func__) << ": Third argument must be a positive number." << std::endl;
        return std::numeric_limits<size_t>::quiet_NaN();
    }
    //! perform nearest neighbor search
    arma::Mat<size_t> neighbors;
    arma::mat distances;
    tgt_tree.Search(src, 1, neighbors, distances);

    //! find indices where nearest neighbor distance is smaller than threshold
    arma::uvec const idx_inliers = arma::find(distances <= epsilon);

    //! the no. of correspondences is the size of the index array from above
    return idx_inliers.n_elem;
}

void nmsac::to_homog(arma::mat33 const & R, arma::vec3 const & t, arma::mat44 & H) noexcept {
    H.eye();
    H( arma::span(0, 2), arma::span(0, 2) ) = R;
    H( arma::span(0, 2), 3 ) = t;
    return;
}

void nmsac::from_homog(arma::mat33 & R, arma::vec3 & t, arma::mat44 const & H) noexcept {
    R = H( arma::span(0, 2), arma::span(0, 2) );
    t = H( arma::span(0, 2), 3 );
    return;
}
