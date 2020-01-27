#pragma once
//! c/c++ headers
//! dependency headers
//! project headers
#include "types.hpp"

namespace nmsac {
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
bool nonmin_registration(arma::mat const & src_sub, arma::mat const & tgt_sub, ConfigNMSAC const & config,
        arma::mat33 & optimal_rot, arma::vec3 & optimal_trans, arma::uvec & src_corr_ids,
        arma::uvec & tgt_corr_ids) noexcept;
};  // namespace nmsac
