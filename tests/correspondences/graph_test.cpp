//! c/c++ headers
#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <fstream>
#include <streambuf>
//! googletest
#include "gtest/gtest.h"
//! dependency headers
//! unit-under-test header
#include "correspondences/graph/graph.hpp"

using namespace correspondences::graph;

//! The fixture for testing class Graph.
class GraphTest : public ::testing::Test {
 protected:
   /**
    * constants for test
    */
   // You can remove any or all of the following functions if their bodies would
   // be empty.

   GraphTest() {
     // You can do set-up work for each test here.
   }

   ~GraphTest() override {
     // You can do clean-up work that doesn't throw exceptions here.
   }

   // If the constructor and destructor are not enough for setting up
   // and cleaning up each test, you can define the following methods:

   void SetUp() override {
     // Code here will be called immediately after the constructor (right
     // before each test).
   }

   void TearDown() override {
     // Code here will be called immediately after each test (right
     // before the destructor).
   }

   // Class members declared here can be used by all tests in the test suite
   // for Foo.
};

TEST_F(GraphTest, SimpleGraphTest) {
  // Figure 2 from https://arxiv.org/pdf/1902.01534.pdf
  vertices_t true_vertices = {1, 2, 3, 4, 5, 6};
  edges_t true_edges = { {1, 2}, {1, 5}, {1, 6},
    {2, 3}, {2, 4}, {2, 5},
    {3, 4}, {3, 5},
    {4, 5},
    {5, 6} };
  std::map<vertex_t, size_t> true_degree = { {1, 3}, {2, 4}, {3, 3}, {4, 3}, {5, 5}, {6, 2} };

  UndirectedGraph g(true_vertices, true_edges);

  //! check vertices
  auto vertices = g.get_vertices();
  EXPECT_TRUE(vertices == true_vertices);

  //! check vertex degree
  std::for_each(true_degree.cbegin(), true_degree.cend(), [&](auto &p) {
      EXPECT_EQ(g.get_vertex_degree(p.first), p.second); });
}

TEST_F(GraphTest, InvalidGraphTest) {
  // Figure 2 from https://arxiv.org/pdf/1902.01534.pdf
  // omit vertex 3 and check that graph is marked as invalid
  // - edges with vertex 3 are still present
  vertices_t vertices = {1, 2, 4, 5, 6};
  edges_t edges = { {1, 2}, {1, 5}, {1, 6},
    {2, 3}, {2, 4}, {2, 5},
    {3, 4}, {3, 5},
    {4, 5},
    {5, 6} };

  EXPECT_EXIT(UndirectedGraph(vertices, edges), ::testing::ExitedWithCode(INVALID_GRAPH), "");
}

TEST_F(GraphTest, NextAvailableColorTest) {
  // test various input conditions for expected output
  // - expected output is the smallest non-negative integer
  // - not in the input set
  {
    std::list<size_t> in = {};
    ASSERT_EQ(next_available_color(in), 0);
  }
  {
    std::list<size_t> in = {0, 0};
    ASSERT_EQ(next_available_color(in), 1);
  }
  {
    std::list<size_t> in = {0, 0, 1, 1};
    ASSERT_EQ(next_available_color(in), 2);
  }
  {
    std::list<size_t> in = {0, 2, 1, 1, 1, 2, 1, 1, 1};
    ASSERT_EQ(next_available_color(in), 3);
  }
  {
    std::list<size_t> in = {0, 0, 2, 2};
    ASSERT_EQ(next_available_color(in), 1);
  }
  {
    std::list<size_t> in = {0, 1, 3, 1, 1, 0, 1, 3, 1, 1};
    ASSERT_EQ(next_available_color(in), 2);
  }
  {
    std::list<size_t> in = {1, 1, 2, 1, 1, 2};
    ASSERT_EQ(next_available_color(in), 0);
  }
}

TEST_F(GraphTest, GreedyVerticesColoringTest) {
  // Figure 2 from https://arxiv.org/pdf/1902.01534.pdf
  vertices_t true_vertices = {1, 2, 3, 4, 5, 6};
  edges_t true_edges = { {1, 2}, {1, 5}, {1, 6},
    {2, 3}, {2, 4}, {2, 5},
    {3, 4}, {3, 5},
    {4, 5},
    {5, 6} };
  coloring_t true_coloring = { {1, 0}, {2, 1}, {3, 0}, {4, 2}, {5, 3}, {6, 1} };

  UndirectedGraph g(true_vertices, true_edges);
  ASSERT_TRUE( greedy_vertices_coloring(true_vertices, g) == true_coloring );
}

TEST_F(GraphTest, SimpleGraphMCQTestBasic) {
  // Figure 2 from https://arxiv.org/pdf/1902.01534.pdf
  vertices_t true_vertices = {1, 2, 3, 4, 5, 6};
  edges_t true_edges = { {1, 2}, {1, 5}, {1, 6},
    {2, 3}, {2, 4}, {2, 5},
    {3, 4}, {3, 5},
    {4, 5},
    {5, 6} };
  std::map<vertex_t, size_t> true_degree = { {1, 3}, {2, 4}, {3, 3}, {4, 3}, {5, 5}, {6, 2} };

  UndirectedGraph g(true_vertices, true_edges);

  vertices_t R_best = {};
  find_max_clique(g, max_clique_algo_e::bnb_basic, R_best);

  vertices_t true_max_clique = {2, 3, 4, 5};
  EXPECT_TRUE(R_best == true_max_clique);
}

TEST_F(GraphTest, SimpleGraphMCQTestColor) {
  // Figure 2 from https://arxiv.org/pdf/1902.01534.pdf
  vertices_t true_vertices = {1, 2, 3, 4, 5, 6};
  edges_t true_edges = { {1, 2}, {1, 5}, {1, 6},
    {2, 3}, {2, 4}, {2, 5},
    {3, 4}, {3, 5},
    {4, 5},
    {5, 6} };
  std::map<vertex_t, size_t> true_degree = { {1, 3}, {2, 4}, {3, 3}, {4, 3}, {5, 5}, {6, 2} };

  UndirectedGraph g(true_vertices, true_edges);

  vertices_t R_best = {};
  find_max_clique(g, max_clique_algo_e::bnb_color, R_best);

  vertices_t true_max_clique = {2, 3, 4, 5};
  EXPECT_TRUE(R_best == true_max_clique);
}
