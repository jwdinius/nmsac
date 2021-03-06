//! c/c++ headers
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>
//! dependency headers
#include "transforms/icp/icp.hpp"
//! project headers
#include "nmsac/helper.hpp"
#include "nmsac/sampling.hpp"
#include "nmsac/registration.hpp"
#include "nmsac/main.hpp"

namespace xfrm = transforms;

/**
 * @brief Algorithm 1 from Section 3.4 of paper
 *
 * @param [in] src_pts source points to transform
 * @param [in] tgt_pts target points
 * @param [in] json_config configurable parameters in json format
 * @param [in][out] optimal_rot rotation matrix of best transformation
 * @param [in][out] optimal_trans translation of best transformation
 * @param [in][out] max_inliers number of inlying point correspondences between src_pts and tgt_pts
 * @param [in][out] iter number of iterations
 * @return
 */
bool nmsac::main(arma::mat src_pts, arma::mat tgt_pts, nlohmann::json & json_config,
    arma::mat33 & optimal_rot, arma::vec3 & optimal_trans,
    size_t & max_inliers, size_t & iter) noexcept {
  // LCOV_EXCL_START
  //! check input size
  if (src_pts.n_rows != 3) {
    std::cout << static_cast<std::string>(__func__) <<
      ": First argument must be a matrix with 3 rows" << std::endl;
    return false;
  } else if (tgt_pts.n_rows != 3) {
    std::cout << static_cast<std::string>(__func__) <<
      ": Second argument must be a matrix with 3 rows" << std::endl;
    return false;
  }
  // LCOV_EXCL_STOP

  //! make a copy of src_pts and tgt_pts
  arma::mat const src_pts_orig = src_pts;
  arma::mat const tgt_pts_orig = tgt_pts;

  //! read config data
  Config config(json_config);
  std::cout << "CONFIGURATION FOR NMSAC:" << std::endl;
  std::cout << config << std::endl;
  config.print_status = true;

  //! initializations
  auto const & n = config.points_per_sample;

  //! update in/out reference values
  iter = 0;
  max_inliers = 0;

  //! for repeatable sampling
  arma::arma_rng::set_seed(config.random_seed);

  //! setup KDTreeSearcher for finding nearest-neighbor points between
  //! transformed source and target point clouds
  xfrm::KDTreeSearcher tgt_tree( tgt_pts_orig );

  //! outer loop
  bool stop = false;
  auto Tmax = std::numeric_limits<double>::max();
  while (!stop && iter < config.max_iter) {
    arma::mat const src_smpl = sample_cols(src_pts, n);
    //! reset tgt_pts back to original for next set of inner passes
    tgt_pts = tgt_pts_orig;

    while (tgt_pts.n_cols > 2*n) {  // XXX(jwd): re-eval what this condition should be
      arma::mat const tgt_smpl = sample_cols(tgt_pts, n);

      // LCOV_EXCL_START
      if (config.print_status) {
        std::cout << "Remaining target points to sample from: " <<
          tgt_pts.n_cols << std::endl;
      }
      // LCOV_EXCL_STOP

      //! solve the nonminimal registration problem
      arma::mat33 R_nmr;
      arma::vec3 t_nmr;
      arma::uvec src_corr_ids, tgt_corr_ids;
      if (registration(src_smpl, tgt_smpl, config, R_nmr, t_nmr,
            src_corr_ids, tgt_corr_ids)) {
          //! do icp - note Ricp, ticp are total transform between src_pts_orig and tgt_pts_orig
          //! R_nmr, t_nmr gives initial guess for coarse alignment between point clouds
          arma::mat44 H_nmr;
          to_homog(R_nmr, t_nmr, H_nmr);
          arma::mat44 H_icp;
          if (!xfrm::iterative_closest_point(src_pts_orig, tgt_pts_orig, H_nmr,
                config.max_iter_icp, config.tol_icp, config.outlier_rej_icp, H_icp)) {
            std::cout << static_cast<std::string>(__func__) <<
              ": iterative_closest_point failed." << std::endl;
            continue;
          }

          //! decompose H into rotation, R, and translation, t, components
          arma::mat33 R_icp;
          arma::vec3 t_icp;
          from_homog(R_icp, t_icp, H_icp);

          //! transform source points onto target points and count the number of inliers
          arma::mat const src_pts_orig_xform = R_icp * src_pts_orig +
            arma::repmat(t_icp, 1, src_pts_orig.n_cols);
          auto const num_inliers = count_correspondences(src_pts_orig_xform,
              tgt_tree, config.algo_config->epsilon);

          //! increase iteration count
          ++iter;

          //! check if the current fit is better than previous fits
          if (num_inliers > max_inliers) {
            max_inliers = num_inliers;
            optimal_rot = R_icp;
            optimal_trans = t_icp;
            // LCOV_EXCL_START
            if (config.print_status) {
              std::cout << "////////////////////////////////" << std::endl;
              std::cout << "Best so-far consensus size: " << max_inliers << std::endl;
              std::cout << "////////////////////////////////" << std::endl;
            }
            // LCOV_EXCL_STOP
            //! compute stopping criteria
            auto const prob_I = static_cast<double>(max_inliers) /
              static_cast<double>(src_pts_orig.n_cols);
            Tmax = std::log(1. - config.ps) / std::log(1. - std::pow(prob_I, config.k));
          }
      } else {
        //! solver failed, go to next iteration
        continue;
      }

      if (iter >= static_cast<size_t>(Tmax)) {
        // LCOV_EXCL_START
        if (config.print_status) {
          std::cout << "Algorithm converged.  Exiting..." << std::endl;
        }
        // LCOV_EXCL_STOP
        stop = true;
        break;
      }
    }
  }
  return true;
}
