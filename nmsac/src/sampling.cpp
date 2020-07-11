//! c/c++ headers
//! dependency headers
//! project headers
#include "nmsac/sampling.hpp"

/**
 * @brief randomly sample a desired number of columns from a matrix and return
 * sampled columns-matrix.  To ensure repeatability, be sure to set the random seed (with arma::rng::set_seed)
 * _before_ the calling this routine.
 *
 * @note modifies original matrix by removing sampled columns
 *
 * @param [in][out] remaining_cols input matrix to sample from and output remaining columns
 * @param [in] num_samples number of columns to sample from
 * @return matrix with sampled columns
 */
arma::mat const nmsac::sample_cols(arma::mat & remaining_cols, size_t const & num_samples) noexcept {
    //! reorder indices
    arma::uvec const ordering = arma::randperm(remaining_cols.n_cols);
    //! reorder remaining_cols by shuffled order
    arma::mat reord = remaining_cols.cols( ordering );
    //! sample num_samples columns from shuffled columns
    arma::mat output = reord.cols( arma::span(0, num_samples-1) );
    //! remove sampled columns from remaining_cols
    remaining_cols = remaining_cols.cols( ordering(arma::span(num_samples, remaining_cols.n_cols-1)) );
    //! return sampled columns
    return output;
}
