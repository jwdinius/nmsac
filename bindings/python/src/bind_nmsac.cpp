//! c/c++ headers
#include <chrono>  // NOLINT [build/c++11]
#include <tuple>
#include <utility>
//! dependency headers
#include <pybind11/pybind11.h>
#include <carma/carma.h>  // to convert armadillo arrays to/from numpy arrays
//! project headers
#include "nmsac/main.hpp"
#include "nmsac/types.hpp"

namespace py = pybind11;
namespace ns = nmsac;

namespace pyNMSAC {
  /**
   * @brief pybind11 wrapper around nmsac main method
   *
   * @param [in] src_np source points to transform (as numpy array)
   * @param [in] tgt_np target points (as numpy array)
   * @return tuple with the following:
   *         (1) return status from nmsac
   *         (2) optimal rotation from src_np to tgt_np
   *         (3) optimal translation from src_np to tgt_np
   *         (4) number of inliers found
   *         (5) number of iterations executed
   *         (6) nmsac execution time (in ms)
   */
  std::tuple<bool, py::array_t<double>, py::array_t<double>, size_t, size_t, double> execute(
      py::array_t<double> & src_np, py::array_t<double> & tgt_np, ns::ConfigNMSAC const & config) {
    //! convert to armadillo arrays (no copy)
    arma::mat const src_pts = carma::arr_to_mat<double>(src_np);
    arma::mat const tgt_pts = carma::arr_to_mat<double>(tgt_np);

    arma::mat33 rot;
    arma::vec3 trans;
    size_t max_inliers, num_iters;

    //! call main
    auto ts = std::chrono::high_resolution_clock::now();
    auto status = ns::main(src_pts, tgt_pts, config, rot, trans, max_inliers, num_iters);
    auto te = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> dur = te - ts;

    //! convert to numpy arrays (** copy required to avoid double free**)
    py::array_t<double> rot_npy = carma::mat_to_arr(rot, /* copy? */ true);
    py::array_t<double> trans_npy = carma::col_to_arr(trans, /* copy? */ true);

    return std::make_tuple(status, rot_npy, trans_npy, max_inliers, num_iters, dur.count());
  }
}  // namespace pyNMSAC

PYBIND11_MODULE(pyNMSAC, m) {
  //! expose nmsac::ConfigNMSAC to Python as "pyNMSAC.Config"
  py::class_<ns::ConfigNMSAC>(m, "Config")
    .def(py::init<>())
    .def(py::init<std::string const &>())
    .def_readwrite("randomSeed", &ns::ConfigNMSAC::random_seed)
    .def_readwrite("printStatus", &ns::ConfigNMSAC::print_status)
    .def_readwrite("ps", &ns::ConfigNMSAC::ps)
    .def_readwrite("maxIter", &ns::ConfigNMSAC::max_iter)
    .def_readwrite("minIter", &ns::ConfigNMSAC::min_iter)
    .def_readwrite("k", &ns::ConfigNMSAC::k)
    .def_readwrite("pointsPerSample", &ns::ConfigNMSAC::points_per_sample)
    .def_readwrite("epsilon", &ns::ConfigNMSAC::epsilon)
    .def_readwrite("nPairThresh", &ns::ConfigNMSAC::n_pair_thresh)
    .def_readwrite("pairDistThresh", &ns::ConfigNMSAC::pair_dist_thresh)
    .def_readwrite("maxIterIcp", &ns::ConfigNMSAC::max_iter_icp)
    .def_readwrite("tolIcp", &ns::ConfigNMSAC::tol_icp)
    .def_readwrite("outlierRejRatioIcp", &ns::ConfigNMSAC::outlier_rej_icp);
  m.def("execute", &pyNMSAC::execute, "Python wrapper around nmsac::main function");
}
