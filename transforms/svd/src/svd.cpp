//! c/c++ headers
#include <iostream>
//! dependency headers
//! project headers
#include "transforms/common/utilities.hpp"  // for enumerate definition
#include "transforms/svd/svd.hpp"

namespace cor = correspondences;

bool transforms::best_fit_transform(arma::mat const & src_pts, arma::mat const & dst_pts,
      cor::correspondences_t const & corrs, arma::mat44 & H_optimal) noexcept {
    //! input checking
    if (src_pts.n_rows != 3) {
        std::cout << static_cast<std::string>(__func__) << ": First argument must be a matrix with 3 rows" << std::endl;
        return false;
    } else if (dst_pts.n_rows != 3) {
        std::cout << static_cast<std::string>(__func__) << ": Second argument must be a matrix with 3 rows" << std::endl;
        return false;
    } else if (src_pts.n_cols != dst_pts.n_cols) {
        std::cout << static_cast<std::string>(__func__) <<
            ": First and second arguments must have same number of columns" << std::endl;
        return false;
    }

    arma::mat source_pts_align(arma::size(src_pts));
    arma::mat target_pts_align(arma::size(dst_pts));

    if (corrs.size() > 0) {
       source_pts_align.set_size(src_pts.n_rows, corrs.size());
       target_pts_align.set_size(dst_pts.n_rows, corrs.size());
       arma::uvec src_inds(corrs.size());
       arma::uvec tgt_inds(corrs.size());
       for (auto [i, c] : enumerate(corrs)) {
          auto const p = c.first; 
          src_inds(i) = p.first;
          tgt_inds(i) = p.second;
       }

       //! order points based on correspondences
       source_pts_align = src_pts.cols(src_inds);
       target_pts_align = dst_pts.cols(tgt_inds);
    } else {
       source_pts_align = src_pts;
       target_pts_align = dst_pts;
    }

    arma::mat33 optimal_rot;
    arma::vec3 optimal_trans;

    //! make const ref to size
    size_t const & n_pts = source_pts_align.n_cols;

    //! compute weighted centroids
    arma::vec3 const src_centroid = arma::vec3(arma::mean(source_pts_align, 1));
    arma::vec3 const dst_centroid = arma::vec3(arma::mean(target_pts_align, 1));
    
    //! translate centroid to origin
    //! - Note the use of the decrement operator: this is why the function signature uses pass by value, not ref
    arma::mat const src_crep = arma::repmat(src_centroid, 1, n_pts);
    arma::mat const dst_crep = arma::repmat(dst_centroid, 1, n_pts);
    source_pts_align -= src_crep;
    target_pts_align -= dst_crep;

    //! compute tensor product
    arma::mat33 const C = arma::mat33(source_pts_align * target_pts_align.t());

    //! compute the singular value decomposition
    arma::mat33 U, V;
    arma::vec3 s;
    arma::svd(U, s, V, C);

    //! compute optimal rotation and translation
    arma::mat33 I(arma::fill::eye);
    if (arma::det(U * V.t()) < 0)
        I(2, 2) *= -1;
    optimal_rot = V * I * U.t();
    optimal_trans = dst_centroid - optimal_rot * src_centroid;

    H_optimal.zeros();
    H_optimal(arma::span(0, 2), arma::span(0, 2)) = optimal_rot;
    H_optimal(arma::span(0, 2), 3) = optimal_trans;
    H_optimal(3, 3) = 1;
    return true;
}
