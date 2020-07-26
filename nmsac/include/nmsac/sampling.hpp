#pragma once
//! c/c++ headers
#include <algorithm>
//! dependency headers
#include <mlpack/core.hpp>
//! project headers

namespace nmsac {
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
arma::mat const sample_cols(arma::mat & remaining_cols, size_t const & num_samples) noexcept;
}  // namespace nmsac
