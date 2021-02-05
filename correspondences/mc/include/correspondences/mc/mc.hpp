#pragma once
//! c/c++ headers
//! dependency headers
#include <armadillo>
//! project headers
#include "correspondences/common/base.hpp"
#include "correspondences/common/types.hpp"
#include "correspondences/graph/graph.hpp"

namespace correspondences {
namespace mc {

/** @struct Config
 * @brief configuration parameters for optimization algorithm 
 * @var Config::epsilon
 * threshold below which to declare a pairwise correspondence
 * @var Config::pairwise_dist_threshold
 * threshold above which to allow pairwise correspondence; this prevents considering correspondences
 * that may have high ambiguity
 * @var Config::algo
 * max clique algorithm implementation to call
 */
struct Config {
  double epsilon;
  double pairwise_dist_threshold;
  graph::max_clique_algo_e algo;
};
}  // namespace mc

/** @class MC : public CorrespondencesBase
 * @brief wrapper class definition for correspondence calculation by solving
 * a quadratic assignment problem (MC)
 */
class MC : public CorrespondencesBase {
 public:
   /** MC::MC(source_pts, target_pts, config)
    * @brief constructor for optimization wrapper class
    *
    * @param[in] source_pts distribution of (columnar) source points
    * @param[in] target_pts distribution of (columnar) target points
    * @param[in] config `Config` instance with optimization parameters; see `Config` definition
    * @return
    */
   explicit MC(arma::mat const & source_pts,
       arma::mat const & target_pts, mc::Config const & config)
     : config_(config)
     , graph_(source_pts, target_pts,
         config_.epsilon, config_.pairwise_dist_threshold)
     , n_(target_pts.n_cols) { }

   /** MC::~MC()
    * @brief destructor for optimization wrapper class
    *
    * @param[in]
    * @return
    */
   ~MC();

   /** MC::calc_correspondences()
    * @brief identify pairwise correspondences between source and target set given optimum found
    * during optimization
    *
    * @param[in]
    * @return solution status 
    * @see status_e definition in types.hpp
    */
   status_e calc_correspondences(correspondences_t & corr) noexcept final;

   /** MC::num_consistent_pairs()
    * @brief get identified correspondences
    *
    * @param[in]
    * @return number of pairwise consistencies identified
    */
   size_t num_consistent_pairs() const noexcept { return graph_.get_edges().size(); }

 private:
   mc::Config config_;  //! initialization data struct
   graph::UndirectedGraph graph_;  //! NOLINT(linelength) pairwise correspondences consistency graph
   size_t n_;  //! NOLINT(linelength) no. of target points (this is for extracting correspondence pairs from vertices)
};
}  // namespace correspondences
