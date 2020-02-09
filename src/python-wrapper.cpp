#include <chrono>
#include <boost/make_shared.hpp>
#include <boost/python.hpp>

#include "nmsac/main.hpp"

namespace boopy = boost::python;

/** @struct PyConfigNMSAC
 * @brief wrapper `ConfigNMSAC`; see "types.hpp" for `ConfigNMSAC` definition
 * @var PyConfigNMSAC::random_seed
 * @var PyConfigNMSAC::print_status
 * @var PyConfigNMSAC::ps
 * @var PyConfigNMSAC::max_iter
 * @var PyConfigNMSAC::min_iter
 * @var PyConfigNMSAC::k
 * @var PyConfigNMSAC::points_per_sample
 * @var PyConfigNMSAC::epsilon
 * @var PyConfigNMSAC::n_pair_thresh
 * @var PyConfigNMSAC::pair_dist_thresh
 * @var PyConfigNMSAC::max_iter_icp
 * @var PyConfigNMSAC::tol_icp
 * @var PyConfigNMSAC::outlier_rej_icp
 *
 * @note follows PEP-8 naming convention
 */
struct PyConfigNMSAC {
    PyConfigNMSAC() : randomSeed(11011), printStatus(false),
        ps(0.99), maxIter(1e4), minIter(5), k(4), pointsPerSample(16),
        epsilon(0.015), nPairThresh(5), pairDistThresh(1e-2), maxIterIcp(20),
        tolIcp(1e-8), outlierRejIcp(0.1) { }
    size_t randomSeed;
    bool printStatus;
    double ps;
    size_t maxIter;
    size_t minIter;
    size_t k;
    size_t pointsPerSample;
    double epsilon;
    size_t nPairThresh;
    double pairDistThresh;
    size_t maxIterIcp;
    double tolIcp;
    double outlierRejIcp;
};

/** @struct PythonNMSAC
 * @brief python wrapper for nmsac::main function
 */
struct PythonNMSAC {
     /** PythonNMSAC(sourcePts, targetPts, pyConfig)
      * @brief constructor for optimization wrapper class
      *
      * @param[in] sourcePts distribution of (columnar) source points, as python list
      * @param[in] targetPts distribution of (columnar) target points, as python list
      * @param[in] pyConfig `PyConfigNMSAC` instance with desired optimization parameters;
      *                     see `PyConfigNMSAC` and `Config` definitions
      * @return
      *
      * @note the optimization is performed when the constructor is called
      */
    PythonNMSAC(boopy::list const & srcPts, boopy::list const & tgtPts,
            PyConfigNMSAC const & pyConfig) {
        //! public members for consumption on the python-side
        arma::mat source_pts(boopy::len(srcPts), boopy::len(srcPts[0]));
        arma::mat target_pts(boopy::len(tgtPts), boopy::len(tgtPts[0]));
        nmsac::ConfigNMSAC config;
        for (size_t i = 0; i < source_pts.n_rows; ++i) {
            for (size_t j = 0; j < source_pts.n_cols; ++j) {
                source_pts(i, j) = boopy::extract<double>(srcPts[i][j]);
            }
        }

        for (size_t i = 0; i < target_pts.n_rows; ++i) {
            for (size_t j = 0; j < target_pts.n_cols; ++j) {
                target_pts(i, j) = boopy::extract<double>(tgtPts[i][j]);
            }
        }

        config.random_seed = pyConfig.randomSeed;
        config.print_status = pyConfig.printStatus;
        config.ps = pyConfig.ps;
        config.max_iter = pyConfig.maxIter;
        config.min_iter = pyConfig.minIter;
        config.k = pyConfig.k;
        config.points_per_sample = pyConfig.pointsPerSample;
        config.epsilon = pyConfig.epsilon;
        config.n_pair_thresh = pyConfig.nPairThresh;
        config.pair_dist_thresh = pyConfig.pairDistThresh;
        config.max_iter_icp = pyConfig.maxIterIcp;
        config.tol_icp = pyConfig.tolIcp;
        config.outlier_rej_icp = pyConfig.outlierRejIcp;

        //! and make the main call (with timing)
        arma::mat33 R;
        arma::vec3 t;
        auto const start = std::chrono::high_resolution_clock::now();
        nmsac::main(source_pts, target_pts, config, R, t, num_inliers_, num_iters_);
        auto const end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        call_time_ = elapsed.count();

        for (size_t i = 0; i < 3; ++i) {
            boopy::list row;
            for (size_t j = 0; j < 3; ++j) {
                row.append(R(i, j));
            }
            t_.append(t(i));
            R_.append(row);
        }
    }

    /** PythonNMSAC::~PythonNMSAC()
     * @brief destructor for python wrapper of optimization wrapper class
     *
     * @param[in]
     * @return
     *
     * @note nothing to do; resources are automatically deallocated
     */
    ~PythonNMSAC() { }

    //! public members
    size_t num_inliers_;
    size_t num_iters_;
    boopy::list R_;
    boopy::list t_;
    double call_time_;
};

BOOST_PYTHON_MODULE(pynmsac) {
    PyEval_InitThreads();

    using namespace boost::python;
    /**
     * @brief python module for nonminimal sampling and consensus registration
     *
     * @note If you want more things exposed, submit a PR!
     */
    //! expose PyConfigNMSAC
    class_<PyConfigNMSAC>("PyConfigNMSAC")
        .def_readwrite("randomSeed", &PyConfigNMSAC::randomSeed)
        .def_readwrite("printStatus", &PyConfigNMSAC::printStatus)
        .def_readwrite("ps", &PyConfigNMSAC::ps)
        .def_readwrite("maxIter", &PyConfigNMSAC::maxIter)
        .def_readwrite("minIter", &PyConfigNMSAC::minIter)
        .def_readwrite("k", &PyConfigNMSAC::k)
        .def_readwrite("pointsPerSample", &PyConfigNMSAC::pointsPerSample)
        .def_readwrite("epsilon", &PyConfigNMSAC::epsilon)
        .def_readwrite("nPairThresh", &PyConfigNMSAC::nPairThresh)
        .def_readwrite("pairDistThresh", &PyConfigNMSAC::pairDistThresh)
        .def_readwrite("maxIterIcp", &PyConfigNMSAC::maxIterIcp)
        .def_readwrite("tolIcp", &PyConfigNMSAC::tolIcp)
        .def_readwrite("outlierRejIcp", &PyConfigNMSAC::outlierRejIcp);

    //! expose PyNMSAC - NOTE: no default constructor is defined/exposed
    class_<PythonNMSAC>("PythonNMSAC", init<boopy::list, boopy::list, PyConfigNMSAC>())
        .def_readonly("R", &PythonNMSAC::R_)
        .def_readonly("t", &PythonNMSAC::t_)
        .def_readonly("numInliers", &PythonNMSAC::num_inliers_)
        .def_readonly("numIters", &PythonNMSAC::num_iters_)
        .def_readonly("hiResCallTime", &PythonNMSAC::call_time_);
}
