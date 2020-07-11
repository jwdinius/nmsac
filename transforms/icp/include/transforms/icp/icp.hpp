#pragma once
//! c/c++ headers
//! dependency headers
#include <mlpack/core.hpp>
#include <mlpack/methods/neighbor_search/neighbor_search.hpp>
//! project headers

namespace transforms {

using KDTreeSearcher = mlpack::neighbor::NeighborSearch<
   mlpack::neighbor::NearestNeighborSort,
   mlpack::metric::EuclideanDistance,
   arma::mat,
   mlpack::tree::KDTree
>;

/**
 * @brief Iterative closest point algorithm: Perform point-set alignment two sets of points with outlier rejection.
 *
 * @param [in] src_pts points to transform
 * @param [in] dst_pts target points
 * @param [in] H_init initial guess for best-fit homogeneous transformation
 * @param [in] max_its maximum number of iterations
 * @param [in] tolerance criteria for convergence, in terms of mean error between iterations
 * @param [in] reject_ratio ratio of worst-matches to reject in fit
 * @param [in][out] H_optimal best-fit transformation to align points in homogeneous coordinates
 * @return
 */
bool iterative_closest_point(arma::mat const & src_pts, arma::mat const & dst_pts,
        arma::mat44 & H_init, size_t const & max_its, double const & tolerance, double const & reject_ratio,
        arma::mat44 & H_optimal) noexcept;
}  // namespace transforms
