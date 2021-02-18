#pragma once
//! c/c++ headers
#include <algorithm>
#include <map>
#include <string>
#include <utility>
//! dependency headers
#include <nlohmann/json.hpp>
//! project headers
#include "types.hpp"


namespace json_utils {
  inline void convert_keys(nlohmann::json &config) {
    nlohmann::json config_copy = config;
    for (auto& [key, value] : config_copy.items()) {
      auto key_str = static_cast<std::string>(key);
      std::string lower;
      std::transform(key.begin(), key.end(), std::back_inserter(lower),
          [&](char c){ return std::tolower(c); });
      if (lower.compare(key) != 0) {
        config[lower] = value;
        config.erase(key);
      }
    }
    return;
  }

  template<typename Type>
  void check_for_param(nlohmann::json const & config, std::string const & key, Type &field) {
    if (config.find(key) != config.end()) {
      field = static_cast<Type>(config[key]);
    }
    return;
  }
}  // namespace json_utils

namespace correspondences {
/**
 * @interface CorrespondencesBase
 *
 * @brief very thin interface for correspondences algorithms
 */
class CorrespondencesBase {
 public:
   CorrespondencesBase() { }
   ~CorrespondencesBase() { }  // must define this for abstract base classes
   //! every child must implement the following method
   virtual status_e calc_correspondences(correspondences_t & correspondences) = 0;
};


/**
 * @struct CorrespondencesConfigBase
 *
 * @brief parameters common to all correspondences algorithms
 */
struct CorrespondencesConfigBase {
  CorrespondencesConfigBase() {
    set_defaults();
  }

  explicit CorrespondencesConfigBase(nlohmann::json & config) {
    set_defaults();
    json_utils::convert_keys(config);
    json_utils::check_for_param(config, "epsilon", epsilon);
    json_utils::check_for_param(config, "pairwise_dist_threshold", pairwise_dist_threshold);
  }

  virtual void set_defaults() noexcept {
    epsilon = 1e-1;
    pairwise_dist_threshold = 1e-1;
  }

  double epsilon;
  double pairwise_dist_threshold;
};
}  // namespace correspondences
