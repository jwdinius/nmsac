#pragma once
//! c/c++ headers
#include <functional>
#include <map>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <utility>
//! dependency headers
#include <boost/functional/hash.hpp>
//! project headers

namespace correspondences {

/** @enum status_e
 * @brief return status for call to calc_correspondences
 *
 * @todo add more fields for better status resolution
 */
enum class status_e {
  failure,
  success
};

/** @typedef WeightKey_t
 * @brief key definition for weight associative container (an unordered map)
 * (i, j, k, l): i, k are indices for points in source distribution, and j, l
 * are points in target distribution
 */
using WeightKey_t = std::tuple<size_t, size_t, size_t, size_t>;

/** @struct key_hash : public std::unary_function<WeightKey_t, size_t>
 * @brief hash function for unordered_map with WeightKey_t as key
 * see https://stackoverflow.com/questions/3611951/building-an-unordered-map-with-tuples-as-keys
 */
struct key_hash : public std::unary_function<WeightKey_t, size_t> {
  /** key_hash::operator()
   * @brief operator overload for ()
   *
   * @param[in] k key to obtain hash for
   * @return seed hash value for key k
   */
  size_t operator()(const WeightKey_t& k) const {
    size_t seed = 0;
    boost::hash_combine(seed, std::get<0>(k));
    boost::hash_combine(seed, std::get<1>(k));
    boost::hash_combine(seed, std::get<2>(k));
    boost::hash_combine(seed, std::get<3>(k));
    return seed;
  }
};

/** @struct key_equal : public std::binary_function<WeightKey_t, WeightKey_t, bool>
 * @brief check if two WeightKey_t instances are equal
 */
struct key_equal : public std::binary_function<WeightKey_t, WeightKey_t, bool> {
  /** key_hash::operator()
   * @brief operator overload for ()
   *
   * @param[in] v0 first key for equality check
   * @param[in] v1 second key for equality check
   * @return flag indicating whether two keys are equal (=true) or not (=false)
   */
  bool operator()(const WeightKey_t& v0, const WeightKey_t& v1) const {
    return std::get<0>(v0) == std::get<0>(v1) &&
      std::get<1>(v0) == std::get<1>(v1) &&
      std::get<2>(v0) == std::get<2>(v1) &&
      std::get<3>(v0) == std::get<3>(v1);
  }
};

/** @typedef WeightTensor
 * @brief unordered map for storing weights associated with pairwise correspondences
 */
using WeightTensor = std::unordered_map<WeightKey_t, double, key_hash, key_equal>;

/** @struct correspondences::key_lthan
 * @brief less than comparator for correspondence map
 */
struct key_lthan :
  public std::binary_function<std::pair<size_t, size_t>, std::pair<size_t, size_t>, bool> {
  /** key_lthan::operator()
   * @brief operator overload for ()
   *
   * @param[in] left left-hand key for "<" comparison
   * @param[in] right right-hand key for "<" comparison
   * @return left < right
   *
   * @note assumes row-major ordering:  if two keys have the same row index, column index is used
   * for comparison, otherwise row index is used for comparison
   */
  bool operator()(const std::pair<size_t, size_t> & left,
      const std::pair<size_t, size_t> & right) const {
    if (left.first == right.first) {
      return left.second < right.second;
    } else {
      return left.first < right.first;
    }
  }
};

/** @typedef correspondences::correspondences_t
 * @brief container for correspondences - (src idx, tgt idx) -> correspondence weighting \in [0, 1]
 *
 * @note stores the strength of the correspondence as the value in the map container
 */
using correspondences_t = std::map<std::pair<size_t, size_t>, double, key_lthan>;
}  // namespace correspondences
