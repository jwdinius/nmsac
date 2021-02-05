//! c/c++ headers
//! dependency headers
//! project headers
#include "correspondences/mc/mc.hpp"

//! namespaces
namespace cor = correspondences;
namespace cg = cor::graph;
namespace cm = cor::mc;

// LCOV_EXCL_START
/** MC::~MC()
 * @brief destructor for constrained objective function
 *
 * @param[in]
 * @return
 *
 * @note nothing to do; resources are automatically deleted
 */
cor::MC::~MC() { }
// LCOV_EXCL_STOP

/** MC::calc_correspondences()
 * @brief identify pairwise correspondences between source and target set given optimum found
 * during optimization
 *
 * @param[in]
 * @return
 */
cor::status_e cor::MC::calc_correspondences(cor::correspondences_t & correspondences) noexcept {
  correspondences.clear();

  //! get maximum clique of consistency graph
  cg::vertices_t R_best;
  cg::find_max_clique(graph_, config_.algo, R_best);

  //! extract correspondence pairs from maximum clique
  for (auto const & c : R_best) {
    auto const key = std::pair<size_t, size_t>(c / n_, c % n_);
    correspondences[key] = 1;  // there is no "score" for correspondences via this method
  }
  return status_e::success;
}
