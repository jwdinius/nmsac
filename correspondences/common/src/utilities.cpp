//! c/c++ headers
#include <map>
#include <vector>
//! dependency headers
#include <ortools/linear_solver/linear_solver.h>  // NOLINT [build/include_order]
//! project headers
#include "correspondences/common/types.hpp"
#include "correspondences/common/utilities.hpp"

//! namespaces
namespace gor = operations_research;  // from ORTools
namespace cor = correspondences;

/**
 * @brief compute pairwise consistency score; see Eqn. 49 from reference
 *
 * @param [in] si ith point in source distribution
 * @param [in] tj jth point in target distribution
 * @param [in] sk kth point in source distribution
 * @param [in] tl lth point in target distribution
 * @return pairwise consistency score for (i, j, k, l)
 */
double cor::consistency(arma::vec3 const & si, arma::vec3 const & tj, arma::vec3 const & sk,
    arma::vec3 const & tl) noexcept {
  double const dist_si_to_sk = arma::norm(si - sk, 2);
  double const dist_tj_to_tl = arma::norm(tj - tl, 2);
  return std::abs(dist_si_to_sk - dist_tj_to_tl);
}

/**
 * @brief generate weight tensor for optimation objective; see Eqn. 48 from reference
 *
 * @param [in] src_pts points to transform
 * @param [in] dst_pts target points
 * @return weight tensor for optimization objective; `w_{ijkl}` from reference
 */
cor::WeightTensor cor::generate_weight_tensor(arma::mat const & source_pts,
      arma::mat const & target_pts, double const & eps, double const & pw_thresh) noexcept {
  WeightTensor weight = {};
  size_t const & m = source_pts.n_cols;
  size_t const & n = target_pts.n_cols;
  for (size_t i = 0; i < m; ++i) {
    for (size_t j = 0; j < n; ++j) {
      for (size_t k = 0; k < m; ++k) {
        for (size_t l = 0; l < n; ++l) {
          if (i != k && j != l) {
            arma::vec3 const si = arma::vec3(source_pts.col(i));
            arma::vec3 const tj = arma::vec3(target_pts.col(j));
            arma::vec3 const sk = arma::vec3(source_pts.col(k));
            arma::vec3 const tl = arma::vec3(target_pts.col(l));
            double const c = consistency(si, tj, sk, tl);
            if (c <= eps
                && arma::norm(si - sk, 2) >= pw_thresh
                && arma::norm(tj - tl, 2) >= pw_thresh) {
              weight[std::make_tuple(i, j, k, l)] = -std::exp(-c);
            }
          }
        }
      }
    }
  }
  return weight;
}

/**
 * @brief Finds x that minimizes inner product <c, x> subject to:
 * (1) bounds constraints lb <= x <= ub, and
 * (2) equality constraints A*x==b
 * using Google's ORTools
 *
 * @param [in] c vector c in <c, x> above
 * @param [in] A matrix A in Ax==b above
 * @param [in] b vector b in Ax==b above
 * @param [in] lower_bound (scalar) lower bound on (individual components of) x
 * @param [in] upper_bound (scalar) upper bound on (individual components of) x
 * @param [in][out] x_opt value of x that minimizes <c, x> subject to defined constraints
 * @return
 */
bool cor::linear_programming(arma::colvec const & c, arma::mat const & A, arma::colvec const & b,
  double const & lower_bound, double const & upper_bound, arma::colvec & x_opt) noexcept {
  // LCOV_EXCL_START
  //! check correct size
  if (c.n_rows != A.n_cols) {
    std::cout << static_cast<std::string>(__func__)
      << ": First and third arguments must have the same number of columns" << std::endl;
    return false;
  } else if (b.n_rows != A.n_rows) {
    std::cout << static_cast<std::string>(__func__)
      << ": Second and third arguments must have the same number of columns" << std::endl;
    return false;
  } else if (c.n_rows != x_opt.n_rows) {
    std::cout << static_cast<std::string>(__func__)
      << ": First and sixth arguments must have the same number of columns" << std::endl;
    return false;
  }
  // LCOV_EXCL_STOP

  //! overwrite x_opt with infeasible values
  auto const infeasible_val = lower_bound - 1.;
  x_opt.fill(infeasible_val);

  //! setup solver
  gor::MPSolver solver("NULL", gor::MPSolver::GLOP_LINEAR_PROGRAMMING);
  std::vector<gor::MPVariable*> opt_sol;
  gor::MPObjective* const objective = solver.MutableObjective();

  //! setup optimal solution array: lower_bound le xi le upper_bound for all 0 le i lt size
  solver.MakeNumVarArray(/* size */ c.n_rows,
      /* lb */ lower_bound,
      /* ub */ upper_bound,
      /* name prefix */ "X",
      /* opt var ref */ &opt_sol);

  //! c.t * x
  for (size_t i = 0; i < c.n_rows; ++i) {
    objective->SetCoefficient(opt_sol[i], c(i));
  }

  //! minimization is the goal : min c.t * x
  objective->SetMinimization();

  //! constraints: A*x le b
  std::vector<gor::MPConstraint*> constraints;
  for (size_t i = 0; i < A.n_rows; ++i) {
    constraints.push_back( solver.MakeRowConstraint( /* lower bound */ -solver.infinity(),
          /* upper bound */ b(i) ) );
    for (size_t j = 0; j < A.n_cols; ++j) {
      constraints.back()->SetCoefficient(opt_sol[j], A(i, j));
    }
  }

  //! find the optimal value
  // XXX(jwd) can increase num threads to be > 1 with solver.setNumThreads to speed up computation
  gor::MPSolver::ResultStatus const result_status = solver.Solve();
  //! check solution
  if (result_status != gor::MPSolver::OPTIMAL) {
    // LCOV_EXCL_START
    LOG(INFO) << "The problem does not have an optimal solution!";
    if (result_status == gor::MPSolver::FEASIBLE) {
      LOG(INFO) << "A potentially suboptimal solution was found";
    } else {
      LOG(INFO) << "The solver failed.";
    }
    return false;
    // LCOV_EXCL_STOP
  }

  //! solver was successful - write output and return true
  for (size_t i = 0; i < x_opt.n_rows; ++i) {
    x_opt(i) = opt_sol[i]->solution_value();
  }
  return true;
}
