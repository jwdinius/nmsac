#pragma once
//! c/c++ headers
//! dependency headers
#include <armadillo>
#include "correspondences/common/types.hpp"  // for correspondences_t definition
//! project headers

namespace transforms {
/**
 * @brief Identify best transformation between two sets of points with known correspondences 
 *
 * @param [in] src_pts points to transform
 * @param [in] dst_pts target points
 * @param [in] corrs mapping of indices from src_pts to dst_pts
 * @param [in][out] H_optimal best-fit transformation to align points in homogeneous coordinates
 * @return
 *
 * @note src_pts and dst_pts must have the same number of columns
 */
bool best_fit_transform(arma::mat const & src_pts, arma::mat const & dst_pts,
    correspondences::correspondences_t const & corrs, arma::mat44 & H_optimal) noexcept;
}  // namespace transforms
