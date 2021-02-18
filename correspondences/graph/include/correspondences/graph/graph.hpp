#pragma once
//! c/c++ headers
#include <list>
#include <set>
#include <unordered_map>
#include <utility>
//! dependency headers
#include <armadillo>
//! project headers

namespace correspondences {
namespace graph {
//! useful type definitions
using vertex_t = size_t;  // vertices will be indexed by an unsigned integer
using vertices_t = std::set< vertex_t >;
using edge_t = std::pair< vertex_t, vertex_t >;
using edges_t = std::set< edge_t >;
using adjacency_t = std::set< size_t >;
using coloring_t = std::unordered_map< vertex_t, size_t >;

//! exit code when constructed graph is invalid
constexpr inline int INVALID_GRAPH = 1;

/**
 * @class UndirectedGraph
 *
 * @see https://en.wikipedia.org/wiki/Graph_(discrete_mathematics)#Undirected_graph
 */
class UndirectedGraph {
 public:
   /** UndirectedGraph::UndirectedGraph()
    * @brief default undirected graph constructor
    *
    * @param[in]
    * @return
    *
    * @note creates an empty graph
    */
   UndirectedGraph();

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
   UndirectedGraph(vertices_t const & vertices, edges_t const & edges);

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
    *
    * @note the application will exit with error if graph is invalid
    * @see UndirectedGraph::validate_graph() method
    */
   UndirectedGraph(arma::mat const & source_pts, arma::mat const & target_pts,
       double const & eps, double const & pw_thresh);

   /** UndirectedGraph::~UndirectedGraph()
    * @brief destructor for constrained objective function
    *
    * @param[in]
    * @return
    *
    * @note nothing to do; resources are automatically deleted
    */
   ~UndirectedGraph();

   /**
    * UndirectedGraph::add_edge(edge_t)
    *
    * @brief add edge to graph
    *
    * @param[in] vertex_t, vertex to add
    *
    * @note adds vertex to graph if it doesn't already exist
    */
   void add_edge(edge_t e) noexcept;

   /**
    * UndirectedGraph::get_adjacency(vertex_t const&)
    *
    * @brief get adjacency set for a vertex in the graph
    * @note the size of the adjacency set is the "degree" of the vertex
    *
    * @param[in] vertex_t, desired vertex to get adjacency of
    * @return adjacency set of input vertex (or {}, if vertex is not in graph)
    */
   adjacency_t get_adjacency(vertex_t const & v) const noexcept {
     if (adjacency_.find(v) != adjacency_.end()) {
       return adjacency_.at(v);
     }
     adjacency_t out = {};
     return out;
   }

   /**
    * UndirectedGraph::get_vertex_degree(vertex_t const&)
    *
    * @brief get degree of vertex in the graph
    *
    * @param[in] vertex_t, desired vertex to get degree of
    * @return degree of input vertex (or 0, if vertex is not in the graph)
    */
   size_t get_vertex_degree(vertex_t const & v) const noexcept {
     return (vertices_.find(v) != vertices_.end()) ? get_adjacency(v).size() : 0;
   }

   /**
    * UndirectedGraph::get_vertices()
    *
    * @return vertices of the graph
    */
   vertices_t get_vertices() const noexcept { return vertices_; }

   /**
    * UndirectedGraph::get_edges()
    *
    * @return edges of the graph
    */
   edges_t get_edges() const noexcept { return edges_; }

 private:
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
   bool validate_graph() const noexcept;

   /**
    * UndirectedGraph::add_vertex(vertex_t)
    *
    * @brief add vertex to graph
    *
    * @param[in] vertex_t, vertex to add
    */
   void add_vertex(vertex_t v) noexcept;

   /**
    * UndirectedGraph::add_adjacency(edge_t)
    *
    * @brief add to adjacency sets for vertices in an edge
    *
    * @param[in] edge_t, edge to incorporate in adjacency sets
    */
   void add_adjacency(edge_t e) noexcept;

   std::unordered_map<vertex_t, adjacency_t> adjacency_;  // NOLINT [linelength] key-value store for vertex and its adjacency set
   vertices_t vertices_;  // set of graph vertices
   edges_t edges_;  // set of graph edges
};

/**
 * next_available_color(std::list<size_t> const&)
 *
 * @brief return next available color based on adjacency coloring
 * @see Section 3.2 of https://arxiv.org/pdf/1902.01534.pdf
 *
 * @param[in] std::list<size_t>, list of accounted for colors
 * @return size_t, smallest unsigned integer not in input list
 */
size_t next_available_color(std::list<size_t> const & colors) noexcept;

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
coloring_t greedy_vertices_coloring(vertices_t const & vertices,
    UndirectedGraph const & graph) noexcept;

/**
 * @enum class max_clique_algo_e
 *
 * @brief available algorithms for finding maximum clique
 */
enum class max_clique_algo_e {
  bnb_basic = 0,
  bnb_color = 1
};

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
void max_cliq_bnb_basic(UndirectedGraph const & graph, vertices_t S,
    vertices_t & R, vertices_t & R_best) noexcept;

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
void max_cliq_bnb_color(UndirectedGraph const & graph, vertices_t const & S,
    coloring_t const & f, vertices_t & R, vertices_t & R_best) noexcept;

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
void find_max_clique(UndirectedGraph const & graph, max_clique_algo_e const & algo,
    vertices_t & R_best) noexcept;
}  // namespace graph
}  // namespace correspondences
