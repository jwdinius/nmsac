//! c/c++ headers
#include <iostream>
#include <memory>
#include <utility>
//! dependency headers
#include <mlpack/core.hpp>
#include "transforms/common/utilities.hpp"
#include "transforms/svd/svd.hpp"
#include "correspondences/common/base.hpp"
#include "correspondences/qap/qap.hpp"
//! project headers
#include "SysSetup.h"  // to define which algorithms to build and link (created by CMake)
#include "nmsac/helper.hpp"
#include "nmsac/registration.hpp"

namespace cor = correspondences;
namespace xfrm = transforms;

/**
 * @brief Run full point-set registration pipeline: including subsampling, correspondence solution, and homogenous transformation
 * identification.
 *
 * @param [in] src_sub subsampled source points
 * @param [in] tgt_sub subsampled target points
 * @param [in] config NMSAC configuration struct
 * @param [in][out] optimal_rot best rotation between source and target points, identified in translate-then-rotate mapping
 * @param [in][out] optimal_trans best translation between source and target points, identified in translate-then-rotate mapping
 * @param [in][out] src_corr_ids indices of points in source that were matched
 * @param [in][out] tgt_corr_ids indices of points in target that were matched
 * @return true if all algorithm stages were successful, false otherwise
 */
bool nmsac::registration(arma::mat const & src_sub, arma::mat const & tgt_sub,
    nmsac::ConfigNMSAC const & config, arma::mat33 & optimal_rot, arma::vec3 & optimal_trans,
    arma::uvec & src_corr_ids, arma::uvec & tgt_corr_ids) noexcept {
  //! check input validity
  if (src_sub.n_rows != 3) {
    std::cout << static_cast<std::string>(__func__) <<
      ": First argument must be a matrix with 3 rows" << std::endl;
    return false;
  } else if (tgt_sub.n_rows != 3) {
    std::cout << static_cast<std::string>(__func__) <<
      ": Second argument must be a matrix with 3 rows" << std::endl;
    return false;
  }

  /**
   * core computation is done by external lib function call;
   */
  cor::correspondences_t corrs;
  std::unique_ptr<cor::CorrespondencesBase> corr_object;

  if constexpr (BUILD_QAP) {
    /**
     * setup QAP - optimization of quadratic assignment problem
     */
    cor::qap::Config reg_config;
    reg_config.epsilon = config.epsilon;
    reg_config.pairwise_dist_threshold = config.pair_dist_thresh;
    reg_config.n_pair_threshold = config.n_pair_thresh;
    corr_object = std::make_unique<cor::QAP>(src_sub, tgt_sub, reg_config);
  }

  /**
   * calculate correspondences
   */
  if (corr_object->calc_correspondences(corrs) != cor::status_e::success) {
    std::cout << static_cast<std::string>(__func__) <<
      ": Correspondence solver failed" << std::endl;
    return false;
  }

  /**
   * calculate best homography from correspondences
   */
  arma::mat44 H_opt;
  if (!xfrm::best_fit_transform(src_sub, tgt_sub, corrs, H_opt)) {
    std::cout << static_cast<std::string>(__func__) <<
      ": Homography from correspondences failed" << std::endl;
    return false;
  }

  auto const & m = corrs.size();  //! == config.k
  src_corr_ids.resize(m);
  tgt_corr_ids.resize(m);
  for (auto [i, c] : xfrm::enumerate(corrs)) {
    auto const p = c.first;
    src_corr_ids(i) = p.first;
    tgt_corr_ids(i) = p.second;
  }

  //! convert from homogeneous transform to (R, t)
  from_homog(optimal_rot, optimal_trans, H_opt);
  return true;
}
