//! c/c++ headers
#include <cstdlib>
#include <limits>
#include <vector>
//! dependency headers
//! project headers
#include "correspondences/common/utilities.hpp"
#include "correspondences/graph/graph.hpp"

//! namespaces
namespace cg = correspondences::graph;

/** UndirectedGraph::UndirectedGraph()
 * @brief default undirected graph constructor
 *
 * @param[in]
 * @return
 *
 * @note creates an empty graph
 */
// LCOV_EXCL_START
cg::UndirectedGraph::UndirectedGraph() { }
// LCOV_EXCL_STOP


/**
 * UndirectedGraph::UndirectedGraph(vertices_t const&, edges_t const&)
 *
 * @brief constructor that builds undirected graph from input
 * vertices and edges
 *
 * @param[in] vertices_t const&, graph vertices
 * @param[in] edges_t const&, graph edges
 *
 * @note the application will exit with error if graph is invalid
 * @see UndirectedGraph::validate_graph() method
 */
cg::UndirectedGraph::UndirectedGraph(vertices_t const & vertices,
    edges_t const & edges) : vertices_(vertices), edges_(edges) {
  if (!validate_graph()) {
    std::cerr << "Graph is invalid!!" << std::endl;
    exit(INVALID_GRAPH);
  }
  std::for_each(edges_.cbegin(), edges_.cend(), [&](auto &e){ add_adjacency(e); });
}

/**
 * UndirectedGraph::UndirectedGraph(arma::mat const&, arma::mat const&,
 *      double const&, double const &)
 *
 * @brief constructor that builds graph from source and target point clouds,
 * as well as consistency thresholds
 *
 * @param[in] arma::mat const&, source point cloud
 * @param[in] arma::mat const&, target point cloud
 * @param[in] double const&, distance between correspondences threshold
 * @param[in] double const&, pairwise distance threshold - reject pairwise
 * consideration when points in a set are too close
 */
cg::UndirectedGraph::UndirectedGraph(arma::mat const & source_pts,
    arma::mat const & target_pts, double const & eps,
    double const & pw_thresh) {
  size_t const & m = source_pts.n_cols;
  size_t const & n = target_pts.n_cols;
  for (size_t i = 0; i < m; ++i) {
    for (size_t j = 0; j < n; ++j) {
      for (size_t k = 0; k < m; ++k) {
        for (size_t l = 0; l < n; ++l) {
          if (i != k && j != l) {
            arma::vec3 const si = arma::vec3(source_pts.col(i));
            arma::vec3 const tj = arma::vec3(target_pts.col(j));
            arma::vec3 const sk = arma::vec3(source_pts.col(k));
            arma::vec3 const tl = arma::vec3(target_pts.col(l));
            double const c = consistency(si, tj, sk, tl);
            if (c <= eps
                && arma::norm(si - sk, 2) >= pw_thresh
                && arma::norm(tj - tl, 2) >= pw_thresh) {
              vertex_t const v1 = i*n + j;
              vertex_t const v2 = k*n + l;
              add_edge({v1, v2});
            }
          }
        }
      }
    }
  }
}



// LCOV_EXCL_START
/** UndirectedGraph::~UndirectedGraph()
 * @brief destructor for constrained objective function
 *
 * @param[in]
 * @return
 *
 * @note nothing to do; resources are automatically deleted
 */
cg::UndirectedGraph::~UndirectedGraph() { }
// LCOV_EXCL_STOP

/**
 * UndirectedGraph::add_edge(edge_t)
 *
 * @brief add edge to graph
 *
 * @param[in] vertex_t, vertex to add
 *
 * @note adds vertex to graph if it doesn't already exist
 */
void cg::UndirectedGraph::add_edge(edge_t e) noexcept {
  auto const & v1 = e.first;
  auto const & v2 = e.second;

  if (v1 == v2) return;

  //! v1, v2 will only be added if they are missing (see std::set docs)
  add_vertex(v1);
  add_vertex(v2);

  edges_.insert(e);

  add_adjacency(e);
}

/**
 * UndirectedGraph::validate_graph()
 *
 * @brief validate graph based on vertices and edges declared
 *
 * @note a graph is valid if, for every edge, the following are true
 *  (1) the first vertex is in the set of vertices
 *  (2) the second vertex is in the set of vertices
 *  (3) the first and second vertex indices are unique
 * @return true, if all 3 conditions are met, false otherwise
 */
bool cg::UndirectedGraph::validate_graph() const noexcept {
  auto is_vertex = [&](vertex_t v) { return (vertices_.find(v) != vertices_.end()); };
  for (auto &e : edges_) {
    if (!is_vertex(e.first) || !is_vertex(e.second) || (e.first == e.second)) return false;
  }
  return true;
}

/**
 * UndirectedGraph::add_vertex(vertex_t)
 *
 * @brief add vertex to graph
 *
 * @param[in] vertex_t, vertex to add
 */
void cg::UndirectedGraph::add_vertex(vertex_t v) noexcept {
  vertices_.insert(v);
}

/**
 * UndirectedGraph::add_adjacency(edge_t)
 *
 * @brief add to adjacency sets for vertices in an edge
 *
 * @param[in] edge_t, edge to incorporate in adjacency sets
 */
void cg::UndirectedGraph::add_adjacency(edge_t e) noexcept {
  auto const & v1 = e.first;
  auto const & v2 = e.second;

  if (v1 == v2) return;

  /**
   * add_to_adj(vertex_t, vertex_t)
   *
   * @brief simple helper function that inserts the first vertex into the
   * the second vertex's adjacency set, or creates the adjacency set if
   * this is the first vertex to be added (adjacency was empty)
   *
   * @note this fn uses calling scope (see [&] for lambda capture)
   */
  auto add_to_adj = [&](vertex_t v1, vertex_t v2) {
    //! adjacency set for v1 is non-empty, so insert v2 into it
    if (adjacency_.find(v1) != adjacency_.end()) {
      adjacency_[v1].insert(v2);
    } else {
      //! adjacency set is empty, so create the adjacency set with v2 as
      //! the first entry
      adjacency_t s = {v2};
      adjacency_.insert({v1, s});
    }
  };

  //! adjacency is symmetric: if v2 is in v1's adjacency set, then v1 is in v2's
  add_to_adj(v1, v2);
  add_to_adj(v2, v1);
}

/**
 * next_available_color(std::list<size_t> const&)
 *
 * @brief return next available color based on adjacency coloring
 * @see Section 3.2 of https://arxiv.org/pdf/1902.01534.pdf
 *
 * @param std::list<size_t>, list of accounted for colors
 * @return size_t, smallest unsigned integer not in input list
 */
size_t cg::next_available_color(std::list<size_t> const & colors) noexcept {
  std::vector<size_t> count(colors.size() + 1, 0);
  std::for_each(colors.cbegin(), colors.cend(), [&](auto &c){
      if (c < count.size()) { ++count[c]; } } );
  for (size_t c = 0; c < count.size(); ++c) {
    if (count[c] == 0) return c;
  }
  // LCOV_EXCL_START
  //! this should never be called, but tell the user (loudly) if the color is not valid
  return std::numeric_limits<size_t>::signaling_NaN();
  // LCOV_EXCL_STOP
}

/**
 * greedy_vertices_coloring(vertices_t const&, UndirectedGraph const&)
 *
 * @brief create greedy coloring of vertices, based on degree of vertex
 * @see Section 3.2 of https://arxiv.org/pdf/1902.01534.pdf
 *
 * @param vertices_t, set of vertices to color
 * @param UndirectedGraph, graph containing vertices to color
 * @return size_t, smallest unsigned integer not in input list
 */
cg::coloring_t cg::greedy_vertices_coloring(vertices_t const & vertices,
    UndirectedGraph const & graph) noexcept {
  //! create empty coloring - this will be filled later
  coloring_t coloring = {};
  //! create coloring helper function
  auto f = [&](vertex_t v) {
    auto adj = graph.get_adjacency(v);
    std::list<size_t> colors = {};
    for (auto &a : adj) {
      if (coloring.find(a) != coloring.end()) {
        colors.emplace_back( coloring[a] );
      }
    }
    return colors;
  };

  //! create graph coloring based on vertex degree
  std::for_each(vertices.cbegin(), vertices.cend(), [&](auto &v) {
      coloring[v] = next_available_color( f(v) ); });
  return coloring;
}

/**
 * max_cliq_bnb_basic(UndirectedGraph const&, vertices_t const&,
 *     vertices_t&, vertices_t&)
 *
 * @brief find maximum clique using a recursive basic branch-and-bound (bnb)
 * algorithm
 * @see Section 3.1 of https://arxiv.org/pdf/1902.01534.pdf
 *
 * @param[in] UndirectedGraph, graph to find maximum clique within
 * @param[in] vertices_t, set of candidate vertices to check
 * @param[in][out] vertices_t&, current clique to check for optimality
 * @param[in][out] vertices_t&, biggest clique found so far
 */
void cg::max_cliq_bnb_basic(UndirectedGraph const & graph, vertices_t S,
    vertices_t & R, vertices_t & R_best) noexcept {
  //! only execute call if set of vertices to expand is non-empty
  while (!S.empty()) {
    //! if the current max clique is bigger than what is possible
    //! given the current expansion, the algorithm is done
    if (R.size() + S.size() <= R_best.size()) return;

    //! expand first vertex in vertices
    auto v = *S.begin();
    R.insert(v);

    //! add all vertices in v's adjacency set to list of candidate vertices
    vertices_t Sp;
    auto adj = graph.get_adjacency(v);
    std::for_each(S.cbegin(), S.cend(), [&](auto &a){
        if (adj.find(a) != adj.end()) { Sp.insert(a); } });

    //! if the list of vertices to expand is non-empty, make recursive call
    if (!Sp.empty()) {
      max_cliq_bnb_basic(graph, Sp, R, R_best);
    } else if (R.size() > R_best.size()) {
      //! if the current clique is bigger than the current best estimate,
      //! update the estimate
      R_best = R;
    }

    //! remove vertex from candidate expansion sets
    R.extract(v);
    S.extract(v);
  }
}

/**
 * max_cliq_bnb_color(UndirectedGraph const&, vertices_t const&,
 *     coloring_t const&, vertices_t&, vertices_t&)
 *
 * @brief find maximum clique using a recursive basic branch-and-bound (bnb)
 * algorithm
 * @see Section 3.1 of https://arxiv.org/pdf/1902.01534.pdf
 *
 * @param[in] UndirectedGraph, graph to find maximum clique within
 * @param[in] vertices_t, set of candidate vertices to check
 * @param[in] coloring_t, vertex coloring (@see greedy_vertices_coloring)
 * @param[in][out] vertices_t&, current clique to check for optimality
 * @param[in][out] vertices_t&, biggest clique found so far
 */
void cg::max_cliq_bnb_color(UndirectedGraph const & graph, vertices_t const & S,
    coloring_t const & f, vertices_t & R, vertices_t & R_best) noexcept {
  //! this algorithm requires a data structure sorted by vertex degree, so
  //! convert the input vertices set to a list
  std::list<vertex_t> S_sort(S.begin(), S.end());

  //! sort the candidate list based on vertex degree - smallest to largest
  S_sort.sort([&](vertex_t const & a, vertex_t const & b) {
        return graph.get_adjacency(a).size() < graph.get_adjacency(b).size(); });

  //! only execute call if set of vertices to expand is non-empty
  while (!S_sort.empty()) {
    //! consider vertex with largest degree - this is the last element of S_sort
    auto v = *S_sort.rbegin();
    //! if the current max clique is bigger than what is possible
    //! given the current expansion (and coloring), the algorithm is done
    if (R.size() + f.at(v) < R_best.size()) return;

    //! add v to current clique
    R.insert(v);

    //! add all vertices in v's adjacency set to list of candidate vertices
    vertices_t Sp;
    auto adj = graph.get_adjacency(v);
    std::for_each(S.cbegin(), S.cend(), [&](auto &a){
        if (adj.find(a) != adj.end()) { Sp.insert(a); } });

    //! if the list of vertices to expand is non-empty, make recursive call
    if (!Sp.empty()) {
      auto fp = greedy_vertices_coloring(Sp, graph);
      max_cliq_bnb_color(graph, Sp, fp, R, R_best);
    } else if (R.size() > R_best.size()) {
      //! if the current clique is bigger than the current best estimate,
      //! update the estimate
      R_best = R;
    }

    //! remove vertex from candidate expansion sets
    R.extract(v);
    S_sort.remove(v);
  }
}

/**
 * find_max_clique(UndirectedGraph const&, max_clique_algo_e const&,
 *     vertices_t&)
 *
 * @brief find maximum clique using a recursive basic branch-and-bound (bnb)
 * algorithm
 * @see Section 3.1 of https://arxiv.org/pdf/1902.01534.pdf
 *
 * @param[in] UndirectedGraph, graph to find maximum clique within
 * @param[in] max_clique_algo_e, algorithm to use
 * @param[in][out] vertices_t&, maximum clique of graph
 */
void cg::find_max_clique(UndirectedGraph const & graph, max_clique_algo_e const & algo,
    vertices_t & R_best) noexcept {
  //! make sure that R_best is currently empty
  R_best.clear();

  //! initialize clique container
  cg::vertices_t R = {};

  if (algo == max_clique_algo_e::bnb_basic) {
    //! call basic bnb implementation
    max_cliq_bnb_basic(graph, graph.get_vertices(), R, R_best);
  } else if (algo == max_clique_algo_e::bnb_color) {
    //! call colored bnb implementation
    //! - first, create coloring of vertices in the entire graph to improve
    //! - candidate vertices selection
    auto f = greedy_vertices_coloring(graph.get_vertices(), graph);
    max_cliq_bnb_color(graph, graph.get_vertices(), f, R, R_best);
  }
}
