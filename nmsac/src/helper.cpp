//! c/c++ headers
#include <fstream>
#include <iostream>
#include <limits>
#include <streambuf>
#include <string>
//! dependency headers
#include <nlohmann/json.hpp>
//! project headers
#include "nmsac/helper.hpp"

using json = nlohmann::json;
namespace xfrm = transforms;

/**
 * @brief Count correspondences between two sets of points 
 *
 * @param [in] src source points
 * @param [in][out] tgt_tree target points (as a KDTree object for nearest-neighbor search)
 * @param [in] epsilon threshold for correspondence counting
 * @return number of correspondences found
 */
size_t nmsac::count_correspondences(arma::mat const & src, xfrm::KDTreeSearcher & tgt_tree,
    double const & epsilon) noexcept {
  // LCOV_EXCL_START
  //! if epsilon <= 0, return NaN
  if (epsilon < std::numeric_limits<double>::epsilon()) {
    std::cout << static_cast<std::string>(__func__) <<
      ": Third argument must be a positive number." << std::endl;
    return std::numeric_limits<size_t>::quiet_NaN();
  }
  // LCOV_EXCL_STOP

  //! perform nearest neighbor search
  arma::Mat<size_t> neighbors;
  arma::mat distances;
  tgt_tree.Search(src, 1, neighbors, distances);

  //! find indices where nearest neighbor distance is smaller than threshold
  arma::uvec const idx_inliers = arma::find(distances <= epsilon);

  //! the no. of correspondences is the size of the index array from above
  return idx_inliers.n_elem;
}

void nmsac::to_homog(arma::mat33 const & R, arma::vec3 const & t, arma::mat44 & H) noexcept {
  H.eye();
  H( arma::span(0, 2), arma::span(0, 2) ) = R;
  H( arma::span(0, 2), 3 ) = t;
  return;
}

void nmsac::from_homog(arma::mat33 & R, arma::vec3 & t, arma::mat44 const & H) noexcept {
  R = H( arma::span(0, 2), arma::span(0, 2) );
  t = H( arma::span(0, 2), 3 );
  return;
}
