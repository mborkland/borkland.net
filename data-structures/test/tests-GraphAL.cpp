#include <random>
#include <unordered_set>
#include "../../catch/catch.hpp"
#include "../src/GraphBuilder.hpp"

using bork_lib::GraphBuilder;
using bork_lib::GraphAL;

std::random_device rd;

TEST_CASE("GraphAL can be constructed using GraphBuilder", "[GraphAL]")
{
    SECTION("Default graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Weighted graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.weighted().build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(graph.weighted());
    }

    SECTION("Graph with satellite data can be constructed")
    {
        auto graph = GraphBuilder<>{}.use_satellite_data().build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Directed graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.directed().build_adj_list();
        REQUIRE(graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Labeled graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.labeled().build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Graph with multiple build options can be constructed")
    {
        auto graph = GraphBuilder<>{}.directed().weighted().build_adj_list();
        REQUIRE(graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(graph.weighted());
    }
}

std::pair<GraphAL<>, std::vector<std::unordered_map<std::size_t, int>>>
        create_random_graph(std::size_t num_vertices, bool directed = false, bool weighted = false)
{
    GraphBuilder<> builder;
    if (directed) {
        builder.directed();
    }
    if (weighted) {
        builder.weighted();
    }
    auto graph = builder.build_adj_list();
    for (int i = 0; i < num_vertices; ++i) {
        graph.add_vertex();
    }

    std::vector<std::unordered_map<std::size_t, int>> adj_list(num_vertices);
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> weight{0, static_cast<int>(num_vertices)};
    for (std::size_t i = 0; i < num_vertices - 1; ++i) {
        auto max_num = static_cast<int>(num_vertices - i - 1);
        std::uniform_int_distribution<> num_edges{1, max_num / 2};
        std::uniform_int_distribution<> edge_to_add{static_cast<int>(i + 1), static_cast<int>(num_vertices - 1)};

        std::unordered_set<std::size_t> edges;
        for (int j = 0; j < num_edges(mt); ++j) {
            edges.insert(static_cast<std::size_t>(edge_to_add(mt)));
        }

        for (auto edge : edges) {
            auto rand_weight = weight(mt);
            auto actual_weight = weighted ? rand_weight : 1;
            graph.add_edge(i, edge, actual_weight);
            adj_list[i].insert({edge, actual_weight});
            if (!directed) {
                adj_list[edge].insert({i, actual_weight});
            }
        }
    }

    return {graph, adj_list};
}

void add_vertices_with_init_list(std::pair<GraphAL<>, std::vector<std::unordered_map<std::size_t, int>>>& graph_pair,
        std::size_t num_vertices, std::size_t vertices_to_add, bool directed = false, bool weighted = false)
{
    auto total_vertices = num_vertices + vertices_to_add;
    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> weight{0, static_cast<int>(total_vertices)};
    for (std::size_t i = num_vertices; i < total_vertices; ++i) {
        auto max_num = static_cast<int>(total_vertices - i - 1);
        std::uniform_int_distribution<> num_edges{1, max_num / 4};
        std::uniform_int_distribution<> edge_to_add{0, static_cast<int>(i - 1)};

        std::unordered_set<std::size_t> outgoing_unique;
        for (int j = 0; j < num_edges(mt); ++j) {
            outgoing_unique.insert(static_cast<std::size_t>(edge_to_add(mt)));
        }
        std::unordered_set<std::size_t> incoming_unique;
        if (directed) {
            for (int j = 0; j < num_edges(mt); ++j) {
                incoming_unique.insert(static_cast<std::size_t>(edge_to_add(mt)));
            }
        }

        std::vector<std::pair<std::size_t, int>> outgoing_edges;
        graph_pair.second.resize(total_vertices);
        for (auto edge : outgoing_unique) {
            auto rand_weight = weight(mt);
            auto actual_weight = weighted ? rand_weight : 1;
            outgoing_edges.emplace_back(edge, actual_weight);
            graph_pair.second[i].insert({edge, actual_weight});
            if (!directed) {
                graph_pair.second[edge].insert({i, actual_weight});
            }
        }
        std::vector<std::pair<std::size_t, int>> incoming_edges;
        for (auto edge : incoming_unique) {
            auto rand_weight = weight(mt);
            auto actual_weight = weighted ? rand_weight : 1;
            incoming_edges.emplace_back(edge, actual_weight);
            graph_pair.second[edge].insert({i, actual_weight});
        }

        graph_pair.first.add_vertex(outgoing_edges, incoming_edges);
    }
}

void valid_graph_test(const std::pair<GraphAL<>, std::vector<std::unordered_map<std::size_t, int>>>& graph_pair)
{
    auto [graph, adj_list] = graph_pair;
    for (std::size_t i = 0; i < adj_list.size(); ++i) {
        auto neighbors = graph.neighbors(i);
        for (auto neighbor : adj_list[i]) {
            auto it = neighbors.find(neighbor.first);
            REQUIRE_FALSE(it == neighbors.end());
            REQUIRE(it->second == neighbor.second);
        }
    }
}

void init_list_test(std::size_t num_vertices, std::size_t vertices_to_add, bool directed = false, bool weighted = false)
{
    auto graph_pair = create_random_graph(num_vertices, directed, weighted);
    add_vertices_with_init_list(graph_pair, num_vertices, vertices_to_add, directed, weighted);
    valid_graph_test(graph_pair);
}

TEST_CASE("Vertices and edges can be added to GraphALs in various ways", "[GraphAL]")
{
    SECTION("Vertices and edges can be added to undirected GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            valid_graph_test(create_random_graph(num_vertices));
        }

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            init_list_test(num_vertices, vertices_to_add);
        }
    }

    SECTION("Vertices and edges can be added to directed GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            valid_graph_test(create_random_graph(num_vertices, true));
        }

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            init_list_test(num_vertices, vertices_to_add, true);
        }
    }

    SECTION("Vertices and edges can be added to weighted GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            valid_graph_test(create_random_graph(num_vertices, false, true));
        }

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            init_list_test(num_vertices, vertices_to_add, false, true);
        }
    }

    SECTION("Vertices and edges can be added to weighted and directed GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            valid_graph_test(create_random_graph(num_vertices, true, true));
        }

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            init_list_test(num_vertices, vertices_to_add, true, true);
        }
    }
}

GraphAL<> create_small_graph(bool directed = false, bool weighted = false)
{
    constexpr std::size_t num_vertices = 10;
    GraphBuilder<> builder;
    if (directed) {
        builder.directed();
    }
    if (weighted) {
        builder.weighted();
    }
    auto graph = builder.build_adj_list();
    for (int i = 0; i < num_vertices; ++i) {
        graph.add_vertex();
    }

    std::mt19937 mt{rd()};
    std::uniform_int_distribution<> weight{0, static_cast<int>(num_vertices)};
    graph.add_edge(1, 5, weight(mt));
    graph.add_edge(4, 5, weight(mt));
    graph.add_edge(2, 5, weight(mt));
    graph.add_edge(4, 0, weight(mt));
    graph.add_edge(4, 5, weight(mt));
    graph.add_edge(5, 7, weight(mt));
    graph.add_edge(5, 8, weight(mt));
    graph.add_edge(7, 3, weight(mt));
    graph.add_edge(8, 9, weight(mt));
    graph.add_edge(9, 6, weight(mt));
    graph.add_edge(3, 6, weight(mt));
    return graph;
}

GraphAL<> create_small_labeled_graph(bool directed = false, bool weighted = false)
{
    GraphBuilder<> builder;
    if (directed) {
        builder.directed();
    }
    if (weighted) {
        builder.weighted();
    }
    auto graph = builder.labeled().build_adj_list();

    graph.add_vertex("Tampa");
    graph.add_vertex("St. Petersburg");
    graph.add_vertex("Fort Myers");
    graph.add_vertex("Naples");
    graph.add_vertex("Miami");
    graph.add_vertex("Orlando");
    graph.add_vertex("Daytona Beach");
    graph.add_vertex("Jacksonville");
    graph.add_vertex("Tallahassee");
    graph.add_vertex("Pensacola");
    graph.add_vertex("Mobile");

    graph.add_edge("Mobile", "Pensacola", 58);
    graph.add_edge("Pensacola", "Tallahassee", 196);
    graph.add_edge("Tallahassee", "Jacksonville", 166);
    graph.add_edge("Jacksonville", "Daytona Beach", 89);
    graph.add_edge("Daytona Beach", "Orlando", 56);
    graph.add_edge("Daytona Beach", "Miami", 261);
    graph.add_edge("Tampa", "St. Petersburg", 23);
    graph.add_edge("Tampa", "Fort Myers", 125);
    graph.add_edge("St. Petersburg", "Fort Myers", 111);
    graph.add_edge("Fort Myers", "Naples", 42);
    graph.add_edge("Naples", "Miami", 124);

    return graph;
}

TEST_CASE("Vertices can be removed from GraphALs", "[GraphAL]")
{
    SECTION("Vertices can be removed from undirected GraphALs")
    {
        SECTION("Vertices can be removed by key")
        {
            auto graph = create_small_graph();
            graph.remove_vertex(5);
            REQUIRE(graph.neighbors(1).empty());
            REQUIRE(graph.neighbors(2).empty());
            REQUIRE_FALSE(graph.neighbors(3).find(5) == graph.neighbors(3).end());
            REQUIRE_FALSE(graph.neighbors(3).find(6) == graph.neighbors(3).end());
            REQUIRE_FALSE(graph.neighbors(8).find(7) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(8).find(5) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(4).find(0) == graph.neighbors(4).end());
        }

        SECTION("Vertices can be removed by label")
        {
            auto graph = create_small_labeled_graph();
            graph.remove_vertex("Tampa");
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertice with given label does not exist in graph.");
            REQUIRE(graph.neighbors("St. Petersburg").size() == 1);
            REQUIRE(graph.neighbors("St. Petersburg").find("Tampa") == graph.neighbors("St. Petersburg").end());
            REQUIRE_FALSE(graph.neighbors("St. Petersburg").find("Fort Myers") == graph.neighbors("St. Petersburg").end());
            REQUIRE(graph.neighbors("Orlando").size() == 1);
            REQUIRE(graph.neighbors("Orlando").find("Tampa") == graph.neighbors("Orlando").end());
            REQUIRE_FALSE(graph.neighbors("Orlando").find("Daytona Beach") == graph.neighbors("Orlando").end());
        }
    }

    SECTION("Vertices can be removed from directed GraphALs")
    {
        SECTION("Vertices can be removed by key")
        {
            auto graph = create_small_graph(true);
            graph.remove_vertex(5);
            REQUIRE(graph.neighbors(1).empty());
            REQUIRE(graph.neighbors(2).empty());
            REQUIRE_FALSE(graph.neighbors(3).find(5) == graph.neighbors(3).end());
            REQUIRE(graph.neighbors(3).find(6) == graph.neighbors(3).end());
            REQUIRE(graph.neighbors(8).find(7) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(8).find(5) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(4).find(0) == graph.neighbors(4).end());
        }

        SECTION("Vertices can be removed by label")
        {
            auto graph = create_small_labeled_graph(true);
            graph.remove_vertex("Tampa");
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertice with given label does not exist in graph.");
            REQUIRE(graph.neighbors("St. Petersburg").size() == 1);
            REQUIRE(graph.neighbors("St. Petersburg").find("Tampa") == graph.neighbors("St. Petersburg").end());
            REQUIRE_FALSE(graph.neighbors("St. Petersburg").find("Fort Myers") == graph.neighbors("St. Petersburg").end());
            REQUIRE(graph.neighbors("Orlando").empty());
            REQUIRE(graph.neighbors("Orlando").find("Tampa") == graph.neighbors("Orlando").end());
            REQUIRE(graph.neighbors("Orlando").find("Daytona Beach") == graph.neighbors("St. Petersburg").end());
        }
    }

    SECTION("Vertices can be removed from weighted GraphALs")
    {
        SECTION("Vertices can be removed by key")
        {
            auto graph = create_small_graph(false, true);
            graph.remove_vertex(5);
            REQUIRE(graph.neighbors(1).empty());
            REQUIRE(graph.neighbors(2).empty());
            REQUIRE_FALSE(graph.neighbors(3).find(5) == graph.neighbors(3).end());
            REQUIRE_FALSE(graph.neighbors(3).find(6) == graph.neighbors(3).end());
            REQUIRE_FALSE(graph.neighbors(8).find(7) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(8).find(5) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(4).find(0) == graph.neighbors(4).end());
        }

        SECTION("Vertices can be removed by label")
        {
            auto graph = create_small_labeled_graph(false, true);
            graph.remove_vertex("Tampa");
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertice with given label does not exist in graph.");
            REQUIRE(graph.neighbors("St. Petersburg").size() == 1);
            REQUIRE(graph.neighbors("St. Petersburg").find("Tampa") == graph.neighbors("St. Petersburg").end());
            REQUIRE_FALSE(graph.neighbors("St. Petersburg").find("Fort Myers") == graph.neighbors("St. Petersburg").end());
            REQUIRE(graph.neighbors("Orlando").size() == 1);
            REQUIRE(graph.neighbors("Orlando").find("Tampa") == graph.neighbors("Orlando").end());
            REQUIRE_FALSE(graph.neighbors("Orlando").find("Daytona Beach") == graph.neighbors("Orlando").end());
        }
    }

    SECTION("Vertices can be removed from directed and weighted GraphALs")
    {
        SECTION("Vertices can be removed by key")
        {
            auto graph = create_small_graph(true, true);
            graph.remove_vertex(5);
            REQUIRE(graph.neighbors(1).empty());
            REQUIRE(graph.neighbors(2).empty());
            REQUIRE_FALSE(graph.neighbors(3).find(5) == graph.neighbors(3).end());
            REQUIRE(graph.neighbors(3).find(6) == graph.neighbors(3).end());
            REQUIRE(graph.neighbors(8).find(7) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(8).find(5) == graph.neighbors(8).end());
            REQUIRE_FALSE(graph.neighbors(4).find(0) == graph.neighbors(4).end());
        }

        SECTION("Vertices can be removed by label")
        {
            auto graph = create_small_labeled_graph(true, true);
            graph.remove_vertex("Tampa");
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertice with given label does not exist in graph.");
            REQUIRE(graph.neighbors("St. Petersburg").size() == 1);
            REQUIRE(graph.neighbors("St. Petersburg").find("Tampa") == graph.neighbors("St. Petersburg").end());
            REQUIRE_FALSE(graph.neighbors("St. Petersburg").find("Fort Myers") == graph.neighbors("St. Petersburg").end());
            REQUIRE(graph.neighbors("Orlando").empty());
            REQUIRE(graph.neighbors("Orlando").find("Tampa") == graph.neighbors("Orlando").end());
            REQUIRE(graph.neighbors("Orlando").find("Daytona Beach") == graph.neighbors("St. Petersburg").end());
        }
    }
}

TEST_CASE("Edges can be removed from GraphALs", "[GraphAL]")
{

}

TEST_CASE("GraphALs can be searched using breadth-first search", "[GraphAL]")
{

}

TEST_CASE("GraphALs can be searched using depth-first search", "[GraphAL]")
{

}