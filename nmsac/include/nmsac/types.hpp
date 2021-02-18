#pragma once
//! c/c++ headers
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <streambuf>
#include <string>
//! dependency headers
#include <nlohmann/json.hpp>
//! project headers
#include "correspondences/common/base.hpp"
#include "correspondences/graph/graph.hpp"
#include "correspondences/qap/qap.hpp"
#include "correspondences/mc/mc.hpp"

namespace nmsac {
enum class algorithms_e {
  qap = 0,
  mc = 1
};

struct Config {
  using json = nlohmann::json;
  Config() {
    set_defaults();
  }

  inline std::string double_prec_str(double const& val,
      size_t const& prec) {
    std::ostringstream str_rep;
    str_rep.precision(prec);
    str_rep << std::scientific << val;
    return str_rep.str();
  }

  explicit Config(json & nmsac_config) {
    set_defaults();
    json_utils::convert_keys(nmsac_config);
    json_utils::check_for_param(nmsac_config, "random_seed", random_seed);
    key_val["random_seed"] = std::to_string(random_seed);
    json_utils::check_for_param(nmsac_config, "print_status", print_status);
    key_val["print_status"] = print_status ? "true" : "false";
    json_utils::check_for_param(nmsac_config, "ps", ps);
    key_val["ps"] = double_prec_str(ps, 3);
    json_utils::check_for_param(nmsac_config, "k", k);
    key_val["k"] = std::to_string(k);
    json_utils::check_for_param(nmsac_config, "max_iter", max_iter);
    key_val["max_iter"] = std::to_string(max_iter);
    json_utils::check_for_param(nmsac_config, "min_iter", min_iter);
    key_val["min_iter"] = std::to_string(min_iter);
    json_utils::check_for_param(nmsac_config, "points_per_sample", points_per_sample);
    key_val["points_per_sample"] = std::to_string(points_per_sample);
    json_utils::check_for_param(nmsac_config, "max_iter_icp", max_iter_icp);
    key_val["max_iter_icp"] = std::to_string(max_iter_icp);
    json_utils::check_for_param(nmsac_config, "tol_icp", tol_icp);
    key_val["tol_icp"] = double_prec_str(tol_icp, 3);
    json_utils::check_for_param(nmsac_config, "outlier_rej_icp", outlier_rej_icp);
    key_val["outlier_rej_icp"] = double_prec_str(outlier_rej_icp, 3);
    setup_algorithm(nmsac_config);
  }

  void setup_algorithm(json & config) noexcept {
    if (config.find("qap") != config.end()) {
      algorithm = algorithms_e::qap;
      auto derived_ptr = std::make_shared<correspondences::qap::Config>(config["qap"]);
      key_val["algorithm"] = "qap";
      key_val["algo_config::epsilon"] = double_prec_str(derived_ptr->epsilon, 3);
      key_val["algo_config::pairwise_dist_threshold"] =
        double_prec_str(derived_ptr->pairwise_dist_threshold, 3);
      key_val["algo_config::corr_threshold"] =
        double_prec_str(derived_ptr->corr_threshold, 3);
      key_val["algo_config::n_pair_threshold"] = std::to_string(
          derived_ptr->n_pair_threshold);
      key_val["algo_config::min_corr"] = std::to_string(derived_ptr->min_corr);
      algo_config = std::static_pointer_cast<correspondences::CorrespondencesConfigBase>(derived_ptr);  // NOLINT [linelength]
    } else if (config.find("mc") != config.end()) {
      algorithm = algorithms_e::mc;
      auto derived_ptr = std::make_shared<correspondences::mc::Config>(config["mc"]);
      key_val["algorithm"] = "mc";
      key_val["algo_config::epsilon"] = double_prec_str(derived_ptr->epsilon, 3);
      key_val["algo_config::pairwise_dist_threshold"] =
        double_prec_str(derived_ptr->pairwise_dist_threshold, 3);
      key_val["algo_config::algo"] =
        (derived_ptr->algo == correspondences::graph::max_clique_algo_e::bnb_basic)
        ? "bnb_basic" : "bnb_color";
      algo_config = std::static_pointer_cast<correspondences::CorrespondencesConfigBase>(derived_ptr);  // NOLINT [linelength]
    } else {
      algorithm = algorithms_e::qap;
      auto derived_ptr = std::make_shared<correspondences::qap::Config>();
      key_val["algorithm"] = "qap";
      key_val["algo_config::epsilon"] = double_prec_str(derived_ptr->epsilon, 3);
      key_val["algo_config::pairwise_dist_threshold"] =
        double_prec_str(derived_ptr->pairwise_dist_threshold, 3);
      key_val["algo_config::corr_threshold"] = double_prec_str(derived_ptr->corr_threshold, 3);
      key_val["algo_config::n_pair_threshold"] = std::to_string(derived_ptr->n_pair_threshold);
      key_val["algo_config::min_corr"] = std::to_string(derived_ptr->min_corr);
      algo_config = std::static_pointer_cast<correspondences::CorrespondencesConfigBase>(derived_ptr);  // NOLINT [linelength]
    }
    return;
  }

  void set_defaults() {
    random_seed = 11011;
    print_status = false;
    ps = 0.99;
    k = 4;
    max_iter = 1e4;
    min_iter = 5;
    points_per_sample = 12;
    max_iter_icp = 100;
    tol_icp = 1e-8;
    outlier_rej_icp = 0.2;
    algorithm = algorithms_e::qap;
    algo_config = std::make_shared<correspondences::CorrespondencesConfigBase>();
  }

  uint64_t random_seed;
  bool print_status;
  double ps;
  size_t max_iter;
  size_t min_iter;
  size_t k;
  size_t points_per_sample;
  size_t n_pair_thresh;
  size_t max_iter_icp;
  double tol_icp;
  double outlier_rej_icp;
  algorithms_e algorithm;
  std::shared_ptr<correspondences::CorrespondencesConfigBase> algo_config;
  std::map<std::string, std::string> key_val;
};

inline std::ostream& operator << (std::ostream& o, Config const& config) {
  for (auto &it : config.key_val) {
    o << it.first << ": " << it.second << "\n";
  }
  return o;
}
}  // namespace nmsac
