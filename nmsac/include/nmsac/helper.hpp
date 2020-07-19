#pragma once
//! c/c++ headers
#include <string>
//! dependency headers
//! project headers
#include "transforms/icp/icp.hpp"  // for KDTreeSearcher definition
#include "types.hpp"

namespace nmsac {
bool read_config(std::string const & config_file, ConfigNMSAC & config) noexcept;
/**
 * @brief Count correspondences between two sets of points 
 *
 * @param [in] src source points
 * @param [in][out] tgt_tree target points (as a KDTree object for nearest-neighbor search)
 * @param [in] epsilon threshold for correspondence counting
 * @return number of correspondences found
 */
size_t count_correspondences(arma::mat const & src, transforms::KDTreeSearcher & tgt_tree,
    double const & epsilon) noexcept;

void to_homog(arma::mat33 const & R, arma::vec3 const & t, arma::mat44 & H) noexcept;
void from_homog(arma::mat33 & R, arma::vec3 & t, arma::mat44 const & H) noexcept;
};  // end namespace nmsac
