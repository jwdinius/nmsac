#pragma once
//! c/c++ headers
//! dependency headers
#include <armadillo>
//! project headers
#include "types.hpp"

namespace correspondences {

/**
 * @brief compute pairwise consistency score; see Eqn. 49 from reference
 *
 * @param [in] si ith point in source distribution
 * @param [in] tj jth point in target distribution
 * @param [in] sk kth point in source distribution
 * @param [in] tl lth point in target distribution
 * @return pairwise consistency score for (i, j, k, l)
 */
double consistency(arma::vec3 const & si, arma::vec3 const & tj,
    arma::vec3 const & sk, arma::vec3 const & tl) noexcept;

/**
 * @brief populate weight tensor for optimization problem; see `w` in Eqn. 48 from paper
 *
 * @param[in] source_pts distribution of (columnar) source points
 * @param[in] target_pts distribution of (columnar) target points
 * @return  weight tensor with weights for pairwise correspondences in optimization objective
 */
WeightTensor generate_weight_tensor(arma::mat const & source_pts,
    arma::mat const & target_pts, double const & eps, double const & pw_thresh) noexcept;

/**
 * @brief Find vector x that minimizes inner product <c, x> subject to bounds constraints
 * lb <= x <= ub and equality constraint A*x==b
 *
 * @param [in] c vector c in <c, x> above
 * @param [in] A matrix A in Ax==b above
 * @param [in] b vector b in Ax==b above
 * @param [in] lower_bound lower bound on (individual components of) x
 * @param [in] upper_bound upper bound on (individual components of) x
 * @param [in][out] x_opt value of x that minimizes <c, x> subject to defined constraints
 * @return
 *
 * @note Uses Google's ORTools
 */
bool linear_programming(arma::colvec const & c, arma::mat const & A, arma::colvec const & b,
    double const & lower_bound, double const & upper_bound, arma::colvec & x_opt) noexcept;
}  // namespace correspondences
