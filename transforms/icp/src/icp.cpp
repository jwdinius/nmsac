//! c/c++ headers
#include <iostream>
//! dependency headers
//! project headers
#include "transforms/icp/icp.hpp"
#include "transforms/svd/svd.hpp"  // for best_fit_transform declaration

/**
 * @brief Iterative closest point algorithm: Perform point-set alignment on two sets of points with outlier rejection.
 *
 * @param [in] src_pts points to transform
 * @param [in] dst_pts target points
 * @param [in] H_init initial guess for best-fit homogeneous transformation
 * @param [in] max_its maximum number of iterations
 * @param [in] tolerance criteria for convergence, in terms of mean error between iterations
 * @param [in] reject_ratio ratio of worst-matches to reject in fit
 * @param [in][out] H_optimal best-fit transformation to align points in homogeneous coordinates
 * @return
 */
bool transforms::iterative_closest_point(arma::mat const & src_pts, arma::mat const & dst_pts,
        arma::mat44 & H_init, size_t const & max_its, double const & tolerance, double const & reject_ratio,
        arma::mat44 & H_optimal) noexcept {
    //! input checking
    if (src_pts.n_rows != 3) {
        std::cout << static_cast<std::string>(__func__) << ": First argument must be a matrix with 3 rows" << std::endl;
        return false;
    } else if (dst_pts.n_rows != 3) {
        std::cout << static_cast<std::string>(__func__) << ": Second argument must be a matrix with 3 rows" << std::endl;
        return false;
    } else if (tolerance < std::numeric_limits<double>::epsilon()) {
        std::cout << static_cast<std::string>(__func__) << ": Fifth argument must be a positive scalar" << std::endl;
        return false;
    } else if (reject_ratio < std::numeric_limits<double>::epsilon() ||
            reject_ratio > static_cast<double>(1) - std::numeric_limits<double>::epsilon()) {
        std::cout << static_cast<std::string>(__func__) <<
            ": Sixth argument must be a positive scalar inside the interval (0, 1), non-inclusive" << std::endl;
        return false;
    }

    //! transform src points by initial homogeneous transformation
    size_t const & src_npts = src_pts.n_cols;
    arma::mat ones(1, src_npts, arma::fill::ones);
    arma::mat aug_src_pts = arma::join_vert(src_pts, ones);
    arma::mat src_xform_full = H_init * aug_src_pts;
    arma::mat src_xform = src_xform_full.rows(0, 2);

    //! identify first index to start discarding from sorted index list
    size_t const reject_idx = std::round( (static_cast<double>(1) - reject_ratio) * src_npts );

    //! setup nearest neighbor search
    KDTreeSearcher dst_searcher(dst_pts);
    arma::Mat<size_t> neighbors;
    arma::mat distances;

    //! loop until converged
    double error = 0;
    size_t counter = 0;
    while (counter++ < max_its) {
        //! find nearest neighbors and distances - neighbors come from searcher
        dst_searcher.Search(src_xform, 1, neighbors, distances);

        //! throw away worst matches
        arma::uvec const best_to_worst_idx = arma::sort_index(distances, "ascend");
        arma::uvec const nbr_idx = arma::conv_to<arma::uvec>::from(neighbors.row(0));
        arma::uvec const sorted_nbr_idx = nbr_idx.elem(best_to_worst_idx);
        arma::uvec const btw_idx_src_rej = best_to_worst_idx( arma::span(0, reject_idx-1) );
        arma::uvec const btw_idx_dst_rej = sorted_nbr_idx( arma::span(0, reject_idx-1) );

        //! compute mean error and check for convergence
        double const mean_error = arma::mean( distances.elem(btw_idx_src_rej) );

        //! if converged, break out of the loop
        if (std::abs(error - mean_error) < tolerance) {
            break;
        }

        //! reset error for next pass
        error = mean_error;

        //! compute best transformation between the current src and nearest dst points
        //! having rejected worst matches
        static_cast<void>( best_fit_transform(src_xform.cols( btw_idx_src_rej ), dst_pts.cols( btw_idx_dst_rej ),
                 {}, H_optimal) );

        //! transform src points by current best transformation
        aug_src_pts = arma::join_vert(src_xform, ones);
        src_xform_full = H_optimal * aug_src_pts;
        src_xform = src_xform_full.rows(0, 2);
    }

    if (counter > max_its)
        //! algorithm didn't converge
        return false;

    //! find best transform from source to icp transformed points
    return best_fit_transform(src_pts, src_xform, {}, H_optimal);
}
