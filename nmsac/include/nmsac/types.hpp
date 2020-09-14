#pragma once
//! c/c++ headers
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
//! dependency headers
#include <nlohmann/json.hpp>
//! project headers

namespace nmsac {
struct ConfigNMSAC {
  using json = nlohmann::json;
  ConfigNMSAC() : random_seed(11011),
    print_status(false),
    ps(0.99),
    max_iter(1e4),
    min_iter(5),
    k(4),
    points_per_sample(12),
    epsilon(0.015),
    n_pair_thresh(5),
    pair_dist_thresh(1e-2),
    max_iter_icp(100),
    tol_icp(1e-8),
    outlier_rej_icp(0.2) { }

  explicit ConfigNMSAC(std::string const & config_file) {
    std::ifstream ifs(config_file);
    std::string json_str = std::string((std::istreambuf_iterator<char>(ifs)),
    std::istreambuf_iterator<char>());
    json json_data = json::parse(json_str);
    auto const & nmsac_config = json_data["NMSAC"]["Config"];
    random_seed = static_cast<uint64_t>(nmsac_config["random_seed"]);  // NOLINT [runtime/int]
    print_status = static_cast<bool>(nmsac_config["print_status"]);
    ps = static_cast<double>(nmsac_config["ps"]);
    max_iter = static_cast<size_t>(nmsac_config["max_iter"]);
    min_iter = static_cast<size_t>(nmsac_config["min_iter"]);
    k = static_cast<size_t>(nmsac_config["k"]);
    points_per_sample = static_cast<size_t>(nmsac_config["points_per_sample"]);
    epsilon = static_cast<double>(nmsac_config["epsilon"]);
    n_pair_thresh = static_cast<size_t>(nmsac_config["n_pair_thresh"]);
    pair_dist_thresh = static_cast<double>(nmsac_config["pair_dist_thresh"]);
    max_iter_icp = static_cast<size_t>(nmsac_config["max_iter_icp"]);
    tol_icp = static_cast<double>(nmsac_config["tol_icp"]);
    outlier_rej_icp = static_cast<double>(nmsac_config["outlier_rej_icp"]);
  }

  uint64_t random_seed;
  bool print_status;
  double ps;
  size_t max_iter;
  size_t min_iter;
  size_t k;
  size_t points_per_sample;
  double epsilon;
  size_t n_pair_thresh;
  double pair_dist_thresh;
  size_t max_iter_icp;
  double tol_icp;
  double outlier_rej_icp;
};
}  // namespace nmsac
