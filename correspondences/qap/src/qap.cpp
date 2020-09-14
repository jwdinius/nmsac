//! c/c++ headers
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>
//! dependency headers
#include <cppad/ipopt/solve.hpp>
//! project headers
#include "correspondences/qap/qap.hpp"

//! namespaces
namespace cor = correspondences;
namespace cq = cor::qap;

/** ConstrainedObjective::operator()
 * @brief operator overload for IPOPT
 *
 * @param[in][out] fgrad objective function evaluation (including constraints) at point `z`
 * @param[in] z point for evaluation
 * return
 */
void cq::ConstrainedObjective::operator()(cq::ConstrainedObjective::ADvector &fgrad,
    cq::ConstrainedObjective::ADvector const & z) noexcept {
  size_t curr_idx = 0;
  //! objective value
  fgrad[curr_idx] = 0.;
  for (size_t i = 0; i < m_; ++i) {
    for (size_t j = 0; j < n_; ++j) {
      for (size_t k = 0; k < m_; ++k) {
        for (size_t l = 0; l < n_; ++l) {
          if (i != k && j != l) {
            WeightKey_t const key = std::make_tuple(i, j, k, l);
            if (weights_.find(key) != weights_.end()) {
              fgrad[curr_idx] += weights_[key] * z[i*(n_+1) + j] * z[k*(n_+1) + l];
            }
          }
        }
      }
    }
  }

  //! constraints:
  for (size_t i = 0; i < m_; ++i) {
    ++curr_idx;
    for (size_t j = 0; j <= n_; ++j) {
      fgrad[curr_idx] += z[i*(n_+1) + j];
    }
  }

  ++curr_idx;
  for (size_t j = 0; j < n_; ++j) {
    fgrad[curr_idx] += z[m_*(n_+1) + j];
  }

  for (size_t j = 0; j < n_; ++j) {
    ++curr_idx;
    for (size_t i = 0; i <= m_; ++i) {
      fgrad[curr_idx] += z[i*(n_+1) + j];
    }
  }

  ++curr_idx;
  for (size_t i = 0; i < m_; ++i) {
    fgrad[curr_idx] += z[i*(n_+1) + n_];
  }
}

/** QAP::linear_projection
 * @brief Solve linear assignment problem:
 *  max c.t()*flatten(X) subject to linear constraints
 *
 * @note linear constraints are constructed within the function
 *
 * @param [in][out] opt_lp projection of optimal solution onto permutation matrices
 * @return true if converged, false otherwise
 */
bool cor::QAP::linear_projection(arma::colvec & opt_lp) const noexcept {
  auto const & m = ptr_obj_->num_source_pts();
  auto const & n = ptr_obj_->num_target_pts();
  auto const & n_con = ptr_obj_->num_constraints();
  auto const & state_len = ptr_obj_->state_length();

  //! make copy of optimum_ (with slack variables)
  arma::colvec c(optimum_);

  //! negate positive values
  c.transform( [&](double & val) { return (val < 0) ? static_cast<double>(0) : -val; } );

  arma::mat A(n_con, state_len, arma::fill::zeros);
  arma::colvec b(n_con, arma::fill::ones);

  //! \sum_i Xi,j = 1
  for (size_t i = 0; i <= m; ++i) {
    A(i, arma::span(i*(n+1), (i+1)*(n+1)-1)).fill(1);
  }

  //! \sum_j Xi,j = 1
  for (size_t i = m+1; i < n_con; ++i) {
    for (size_t j = 0; j < state_len; j+=n+1) {
      A(i, j) = 1;
    }
  }

  //! find projection
  return linear_programming(c, A, b, static_cast<double>(0), static_cast<double>(1), opt_lp);
}

/** QAP::calc_optimum()
 * @brief run IPOPT to find optimum for optimization objective:
 * argmin f(z) subject to constraints g_i(z) == 0, 0 <= i < num_constraints
 *
 * @param[in]
 * @return
 *
 * @note see Eqn. 48 in paper and subsequent section for details
 */
cor::QAP::ipopt_status_t cor::QAP::calc_optimum() noexcept {
  auto const & m = ptr_obj_->num_source_pts();
  auto const & n = ptr_obj_->num_target_pts();
  auto const & k = ptr_obj_->num_min_corr();
  auto const & n_vars = ptr_obj_->state_length();
  auto const & n_constraints = ptr_obj_->num_constraints();

  Dvec z(n_vars);

  //! setup inequality constraints on variables: 0 <= z_{ij} <= 1 for all i,j
  Dvec z_lb(n_vars);
  Dvec z_ub(n_vars);
  for (size_t i = 0; i < n_vars; ++i) {
    z_lb[i] = 0;
    z_ub[i] = 1;
  }
  //! overwrite last value to avoid checking conditional for all iterations
  z_ub[n_vars-1] = 0;

  //! setup constraints l_i <= g_i(z) <= u_i
  Dvec constraints_lb(n_constraints);
  Dvec constraints_ub(n_constraints);
  size_t ctr = 0;
  for (size_t i = 0; i < m; ++i) {
    constraints_lb[ctr] = 1;
    constraints_ub[ctr++] = 1;
  }

  constraints_lb[ctr] = n - k;
  constraints_ub[ctr++] = n - k;
  for (size_t j = 0; j < n; ++j) {
    constraints_lb[ctr] = 1;
    constraints_ub[ctr++] = 1;
  }
  constraints_lb[ctr] = m - k;
  constraints_ub[ctr] = m - k;

  //! options for IPOPT solver
  std::string options;
  options += "Integer print_level  0\n";
  /**
   * NOTE: Setting sparse to true allows the solver to take advantage
   * of sparse routines, this makes the computation MUCH FASTER. If you
   * can uncomment 1 of these and see if it makes a difference or not but
   * if you uncomment both the computation time should go up in orders of
   * magnitude.
   */
  options += "Sparse  true        forward\n";
  options += "Sparse  true        reverse\n";
  options += "Numeric tol         0.1\n";
  options += "Numeric acceptable_tol 0.1\n";

  //! timeout period (sec).
  options += "Numeric max_cpu_time          1000.0\n";

  //! solve the problem
  CppAD::ipopt::solve_result<Dvec> solution;
  CppAD::ipopt::solve<Dvec, qap::ConstrainedObjective>(
      options, z, z_lb, z_ub, constraints_lb,
      constraints_ub, *ptr_obj_, solution);
  //! if solver was not successful, return early
  if (solution.status != ipopt_status_t::success) {
    return solution.status;
  }
  //! overwrite private member optimum_ with result and
  //! return success
  for (size_t i = 0; i < n_vars; ++i) {
    optimum_(i) = solution.x[i];
  }

  //! project onto permutation matrices; see Section 3.3 of the SDRSAC paper
  arma::colvec proj_opt(n_vars);
  if (linear_projection(proj_opt)) {
    //! only update optimum_ if linear_projection was successful
    optimum_ = proj_opt;
  }
  return ipopt_status_t::success;
}

/** QAP::calc_correspondences()
 * @brief identify pairwise correspondences between source and target set given optimum found
 * during optimization
 *
 * @param[in]
 * @return
 */
cor::status_e cor::QAP::calc_correspondences(cor::correspondences_t & correspondences) noexcept {
  //! find optima
  if (calc_optimum() != ipopt_status_t::success) {
    return status_e::failure;
  }

  correspondences.clear();
  auto const & m = ptr_obj_->num_source_pts();
  auto const & n = ptr_obj_->num_target_pts();

  //! find best correspondences i->j (includes slack variables)
  arma::uvec const ids = arma::find(optimum_ >= config_.corr_threshold);
  std::list<size_t> corr_ids;
  for (size_t i = 0; i < ids.n_elem; ++i) {
    corr_ids.emplace_back(static_cast<size_t>(ids(i)));
  }

  //! i = divide(id, n+1), j = remainder(id, n+1) for correspondence i->j
  //! value is the strength of the correspondence (0 <= z_{ij} <= 1, closer to 1
  //! is better)
  for (auto const & c : corr_ids) {
    auto const key = std::pair<size_t, size_t>(c / (n+1), c % (n+1));
    double const value = optimum_(c);
    //! discount slack variables
    if (key.first != m && key.second != n) {
      correspondences[key] = value;
    }
  }
  return status_e::success;
}
