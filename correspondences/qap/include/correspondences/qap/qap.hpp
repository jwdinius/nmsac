#pragma once
//! c/c++ headers
#include <cmath>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>
//! dependency headers
#include <armadillo>  // NOLINT [build/include_order]
#include <boost/functional/hash.hpp>
#include <cppad/cppad.hpp>
#include <cppad/ipopt/solve.hpp>
#include "nlohmann/json.hpp"
//! project headers
#include "correspondences/common/base.hpp"
#include "correspondences/common/types.hpp"
#include "correspondences/common/utilities.hpp"

namespace correspondences {
namespace qap {
/** @struct Config
 * @brief configuration parameters for optimization algorithm 
 * @var Config::epsilon
 * threshold below which to declare a pairwise correspondence
 * @var Config::pairwise_dist_threshold
 * threshold above which to allow pairwise correspondence; this prevents considering correspondences
 * that may have high ambiguity
 * @var Config::corr_threshold
 * value in optimum solution to declare a correspondence as valid {\in (0, 1)}
 * @var Config::n_pair_threshold
 * minimum number of pairwise consistencies 
 */
struct Config {
  Config() :
    epsilon(1e-1),
    pairwise_dist_threshold(1e-1),
    corr_threshold(0.9),
    n_pair_threshold(std::numeric_limits<size_t>::signaling_NaN()),
    min_corr(5) { }
  explicit Config(nlohmann::json const & config) :
    epsilon(static_cast<double>(config.at("epsilon"))),
    pairwise_dist_threshold(static_cast<double>(config.at("pairwise_dist_threshold"))),
    corr_threshold(static_cast<double>(config.at("corr_threshold"))),
    n_pair_threshold(static_cast<size_t>(config.at("n_pair_threshold"))),
    min_corr(static_cast<size_t>(config.at("min_corr"))) { }
  double epsilon, pairwise_dist_threshold, corr_threshold;
  size_t n_pair_threshold, min_corr;
};

/** @class ConstrainedObjective
 * @brief class definition for point set registration relaxation objective
 */
class ConstrainedObjective {
 public:
   /** @typedef ConstrainedObjective::ADVector
    * @brief typedef for CppAD automatic differentiation during optimization execution
    */
   using ADvector = CPPAD_TESTVECTOR(CppAD::AD<double>);

   /** ConstrainedObjective::ConstrainedObjective(source_pts, target_pts, config)
    * @brief constructor for constrained objective function
    *
    * @param[in] source_pts distribution of (columnar) source points
    * @param[in] target_pts distribution of (columnar) target points
    * @param[in] config `Config` instance with optimization parameters; see `Config` definition
    * @return
    */
   explicit ConstrainedObjective(arma::mat const & source_pts,
       arma::mat const & target_pts, Config const & config) :
     m_(static_cast<size_t>(source_pts.n_cols)), n_(static_cast<size_t>(target_pts.n_cols)),
     min_corr_(config.min_corr) {
     weights_ = generate_weight_tensor(source_pts, target_pts, config.epsilon, config.pairwise_dist_threshold);
     n_constraints_ = m_ + n_ + 2;
   }

   /** ConstrainedObjective::~ConstrainedObjective()
    * @brief destructor for constrained objective function
    *
    * @param[in]
    * @return
    */
   ~ConstrainedObjective();

   /** ConstrainedObjective::operator()
    * @brief operator overload for IPOPT
    *
    * @param[in][out] fgrad objective function evaluation (including constraints) at point `z`
    * @param[in] z point for evaluation
    * @return
    */
   void operator()(ADvector &fgrad, ADvector const & z) noexcept;

   /** ConstrainedObjective::num_constraints()
    * @brief return number of constraints for optimization objective
    *
    * @param[in]
    * @return copy of private member `n_constraints_`
    */
   size_t num_constraints() const noexcept { return n_constraints_; }

   /** ConstrainedObjective::num_source_pts()
    * @brief return number of source points for optimization objective
    *
    * @param[in]
    * @return copy of private member `m_`
    */
   size_t num_source_pts() const noexcept { return m_; }

   /** ConstrainedObjective::num_target_pts()
    * @brief return number of target points for optimization objective
    *
    * @param[in]
    * @return copy of private member `n_`
    */
   size_t num_target_pts() const noexcept { return n_; }

   /** ConstrainedObjective::num_min_corr()
    * @brief return minimum number of correspondences for optimization objective
    *
    * @param[in]
    * @return copy of private member `min_corr_`
    */
   size_t num_min_corr() const noexcept { return min_corr_; }

   /** ConstrainedObjective::state_length()
    * @brief get size of state vector for optimization objective
    *
    * @param[in]
    * @return (m_ + 1)*(n_ + 1)
    * @note includes slack variables
    */
   size_t state_length() const noexcept { return (m_ + 1) * (n_ + 1); }

   /** ConstrainedObjective::get_weight_tensor()
    * @brief get weight_tensor
    *
    * @param[in]
    * @return copy of private member `weights_`
    */
   WeightTensor get_weight_tensor() const noexcept { return weights_; }

 private:
   WeightTensor weights_;
   size_t m_, n_, min_corr_, n_constraints_;
};
}  // namespace qap

/** @class QAP : public CorrespondencesBase
 * @brief wrapper class definition for correspondence calculation by solving
 * a quadratic assignment problem (QAP)
 */
class QAP : public CorrespondencesBase {
 public:
   /** @typedef QAP::DVec
    * @brief typedef for CppAD test vector
    */
   using Dvec = CPPAD_TESTVECTOR(double);

   /** @typedef QAP::ipopt_status_t
    * @brief typedef for IPOPT return code
    */
   using ipopt_status_t = CppAD::ipopt::solve_result<Dvec>::status_type;

   /** QAP::QAP(source_pts, target_pts, config)
    * @brief constructor for optimization wrapper class
    *
    * @param[in] source_pts distribution of (columnar) source points
    * @param[in] target_pts distribution of (columnar) target points
    * @param[in] config `Config` instance with optimization parameters; see `Config` definition
    * @return
    */
   explicit QAP(arma::mat const & source_pts,
       arma::mat const & target_pts, qap::Config const & config) : config_(config) {
     ptr_obj_ = std::make_unique<qap::ConstrainedObjective>(source_pts, target_pts, config);
     optimum_.resize(ptr_obj_->state_length());
   }

   /** QAP::~QAP()
    * @brief destructor for optimization wrapper class
    *
    * @param[in]
    * @return
    */
   ~QAP();

   /** QAP::calc_optimum()
    * @brief run IPOPT to find optimum for optimization objective
    *
    * @param[in]
    * @return solver status returned by IPOPT
    */
   ipopt_status_t calc_optimum() noexcept;

   /** PointRegRelaxation::find_correspondences()
    * @brief identify pairwise correspondences between source and target set given optimum found
    * during optimization
    *
    * @param[in]
    * @return solution status 
    * @see status_e definition in types.hpp
    */
   status_e calc_correspondences(correspondences_t & corr) noexcept override final;

   /** QAP::get_optimum()
    * @brief get optimization
    *
    * @param[in]
    * @return
    */
   arma::colvec get_optimum() const noexcept { return optimum_; }

   /** QAP::linear_projection
    * @brief Solve linear assignment problem:
    *  max c.t()*flatten(X) subject to linear constraints
    *
    * @note linear constraints are constructed within the function
    *
    * @param [in][out] opt_lp projection of optimal solution onto permutation matrices
    * @return true if converged, false otherwise
    */
   bool linear_projection(arma::colvec & opt_lp) const noexcept;

   /** QAP::num_consistent_pairs()
    * @brief get identified correspondences
    *
    * @param[in]
    * @return number of pairwise consistencies identified
    */
   size_t num_consistent_pairs() const noexcept { return ptr_obj_->get_weight_tensor().size(); }

 private:
   qap::Config config_;
   std::unique_ptr<qap::ConstrainedObjective> ptr_obj_;
   arma::colvec optimum_;
};
}  // namespace correspondences
