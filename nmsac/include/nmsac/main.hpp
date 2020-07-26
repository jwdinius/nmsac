#pragma once
//! c/c++ headers
//! dependency headers
//! project headers
#include "types.hpp"

namespace nmsac {
/**
 * @brief Algorithm 1 from Section 3.4 of paper
 *
 * @param [in] src_pts source points to transform
 * @param [in] tgt_pts target points
 * @param [in][out] optimal_rot rotation matrix of best transformation
 * @param [in][out] optimal_trans translation of best transformation
 * @param [in][out] max_inliers number of inlying point correspondences between src_pts and tgt_pts
 * @param [in][out] iter number of iterations
 * @return
 */
bool main(arma::mat src_pts, arma::mat tgt_pts, ConfigNMSAC const & config,
    arma::mat33 & optimal_rot, arma::vec3 & optimal_trans,
    size_t & max_inliers, size_t & iter) noexcept;
}  // namespace nmsac
