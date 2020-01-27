//! c/c++ headers
#include <iostream>
//! dependency headers (from Registration project)
#include <mlpack/core.hpp>  //XXX (jwd) addresses strange compile-time issue
#include <correspondences/correspondences.hpp>
#include <registration/registration.hpp>
//! project headers
#include "nmsac/helper-utils.hpp"
#include "nmsac/nonmin-reg-utils.hpp"

namespace cor = correspondences;
namespace reg = registration;

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
bool nmsac::nonmin_registration(arma::mat const & src_sub, arma::mat const & tgt_sub,
        nmsac::ConfigNMSAC const & config, arma::mat33 & optimal_rot, arma::vec3 & optimal_trans,
        arma::uvec & src_corr_ids, arma::uvec & tgt_corr_ids) noexcept {
    //! check input validity
    if (src_sub.n_rows != 3) {
        std::cout << static_cast<std::string>(__func__) << ": First argument must be a matrix with 3 rows" << std::endl;
        return false;
    } else if (tgt_sub.n_rows != 3) {
        std::cout << static_cast<std::string>(__func__) << ": Second argument must be a matrix with 3 rows" << std::endl;
        return false;
    }

    cor::Config reg_config;
    reg_config.epsilon = config.epsilon;
    reg_config.pairwise_dist_threshold = config.pair_dist_thresh;
    reg_config.n_pair_threshold = config.n_pair_thresh;
    reg_config.do_warm_start = true;

    /**
     * core computation is done by external lib function call;
     * see Register package usage in CMakeLists.txt
     */
    arma::colvec optimum;
    cor::PointRegRelaxation::correspondences_t correspondences;
    arma::mat44 H_opt;
    auto const solver_status = reg::registration(src_sub, tgt_sub, reg_config, config.k,
            optimum, correspondences, H_opt);

    if (solver_status == reg::Status::TooFewPairs) {
        std::cout << static_cast<std::string>(__func__) << ": Too few correspondence pairs found." << std::endl;
        return false;
    } else if (solver_status != reg::Status::Success) {
        std::cout << static_cast<std::string>(__func__) << ": Optimizer returned bad status." << std::endl;
        return false;
    }

    auto const & m = correspondences.size();  //! == config.k
    src_corr_ids.resize(m);
    tgt_corr_ids.resize(m);
    size_t i = 0;
    for (auto const & c : correspondences) {
        //! see original implementation: correspondences is an ordered map with the ordering
        //! determined by ascending src_pt index
        src_corr_ids(i) = c.first.first;
        tgt_corr_ids(i) = c.first.second;
        ++i;
    }
    //! convert from homogeneous transform to (R, t)
    from_homog(optimal_rot, optimal_trans, H_opt);
    return true;
}
