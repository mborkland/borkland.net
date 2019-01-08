#include <random>
#include <set>
#include <unordered_set>
#include "../../catch/catch.hpp"
#include "../src/GraphBuilder.hpp"

using bork_lib::BasicGraphBuilder;
using bork_lib::LabeledGraphBuilder;
using bork_lib::GraphAL;

std::random_device rd;

TEST_CASE("GraphAL can be constructed using GraphBuilder", "[GraphAL]")
{
    SECTION("Default graph can be constructed")
    {
        auto graph = BasicGraphBuilder<>{}.build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Weighted graph can be constructed")
    {
        auto graph = BasicGraphBuilder<>{}.weighted().build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(graph.weighted());
    }

    SECTION("Graph with satellite data can be constructed")
    {
        auto graph = BasicGraphBuilder<>{}.use_satellite_data().build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Directed graph can be constructed")
    {
        auto graph = BasicGraphBuilder<>{}.directed().build_adj_list();
        REQUIRE(graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Labeled graph can be constructed")
    {
        auto graph = LabeledGraphBuilder<>{}.build_adj_list();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Graph with multiple build options can be constructed")
    {
        auto graph = BasicGraphBuilder<>{}.directed().weighted().build_adj_list();
        REQUIRE(graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(graph.weighted());
    }
}

std::pair<GraphAL<>, std::vector<std::unordered_map<std::size_t, int>>>
        create_random_graph(std::size_t num_vertices, bool directed = false, bool weighted = false)
{
    BasicGraphBuilder<> builder;
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

TEST_CASE("Vertices and edges can be added to GraphALs in various ways", "[GraphAL]")
{
    SECTION("Vertices and edges can be added to undirected GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            auto graph_pair = create_random_graph(num_vertices);
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

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            auto graph_pair = create_random_graph(num_vertices);
            add_vertices_with_init_list(graph_pair, num_vertices, vertices_to_add);
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
    }

    SECTION("Vertices and edges can be added to directed GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            auto graph_pair = create_random_graph(num_vertices, true);
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

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            auto graph_pair = create_random_graph(num_vertices, true);
            add_vertices_with_init_list(graph_pair, num_vertices, vertices_to_add, true);
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
    }

    SECTION("Vertices and edges can be added to weighted GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            auto graph_pair = create_random_graph(num_vertices, false, true);
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

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            auto graph_pair = create_random_graph(num_vertices, false, true);
            add_vertices_with_init_list(graph_pair, num_vertices, vertices_to_add, false, true);
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
    }

    SECTION("Vertices and edges can be added to weighted and directed GraphALs in various ways")
    {
        SECTION("Edges can be added manually to existing vertices")
        {
            constexpr std::size_t num_vertices = 1000;
            auto graph_pair = create_random_graph(num_vertices, true, true);
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

        SECTION("Edges and vertices and be added using a vector of pairs")
        {
            constexpr std::size_t num_vertices = 1000;
            constexpr std::size_t vertices_to_add = 1000;
            auto graph_pair = create_random_graph(num_vertices, true, true);
            add_vertices_with_init_list(graph_pair, num_vertices, vertices_to_add, true, true);
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
    }
}

GraphAL<> create_small_graph(bool directed = false, bool weighted = false, bool has_satellite_data = false)
{
    constexpr std::size_t num_vertices = 10;
    BasicGraphBuilder<> builder;
    if (directed) {
        builder.directed();
    }
    if (weighted) {
        builder.weighted();
    }
    if (has_satellite_data) {
        builder.use_satellite_data();
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
    graph.add_edge(5, 7, weight(mt));
    graph.add_edge(5, 8, weight(mt));
    graph.add_edge(7, 3, weight(mt));
    graph.add_edge(8, 9, weight(mt));
    graph.add_edge(9, 6, weight(mt));
    graph.add_edge(3, 6, weight(mt));
    return graph;
}

GraphAL<std::string> create_small_labeled_graph(bool directed = false, bool weighted = false, bool has_satellite_data = false)
{
    LabeledGraphBuilder<> builder;
    if (directed) {
        builder.directed();
    }
    if (weighted) {
        builder.weighted();
    }
    if (has_satellite_data) {
        builder.use_satellite_data();
    }
    auto graph = builder.build_adj_list();

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
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertex with given label does not exist in graph.");
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
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertex with given label does not exist in graph.");
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
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertex with given label does not exist in graph.");
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
            REQUIRE_THROWS_WITH(graph.neighbors("Tampa"), "Vertex with given label does not exist in graph.");
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
    SECTION("Edges can be removed from undirected GraphALs")
    {
        auto graph = create_small_graph();
        graph.remove_edge(1, 5);
        graph.remove_edge(2, 5);
        graph.remove_edge(4, 5);
        REQUIRE(graph.neighbors(1).empty());
        REQUIRE(graph.neighbors(2).empty());
        REQUIRE(graph.neighbors(4).size() == 1);
        REQUIRE(graph.neighbors(5).size() == 2);
        REQUIRE(graph.neighbors(5).find(1) == graph.neighbors(5).end());
        REQUIRE(graph.neighbors(5).find(2) == graph.neighbors(5).end());
        REQUIRE(graph.neighbors(5).find(4) == graph.neighbors(5).end());
        REQUIRE_FALSE(graph.neighbors(5).find(7) == graph.neighbors(5).end());
        REQUIRE_FALSE(graph.neighbors(5).find(8) == graph.neighbors(5).end());
    }

    SECTION("Edges can be removed from directed GraphALs")
    {
        auto graph = create_small_graph(true);
        graph.remove_edge(1, 5);
        graph.remove_edge(2, 5);
        graph.remove_edge(4, 5);
        REQUIRE(graph.neighbors(1).empty());
        REQUIRE(graph.neighbors(2).empty());
        REQUIRE(graph.neighbors(4).size() == 1);
        REQUIRE(graph.neighbors(5).size() == 2);
        REQUIRE(graph.neighbors(5).find(1) == graph.neighbors(5).end());
        REQUIRE(graph.neighbors(5).find(2) == graph.neighbors(5).end());
        REQUIRE(graph.neighbors(5).find(4) == graph.neighbors(5).end());
        REQUIRE_FALSE(graph.neighbors(5).find(7) == graph.neighbors(5).end());
        REQUIRE_FALSE(graph.neighbors(5).find(8) == graph.neighbors(5).end());
    }
}

TEST_CASE("GraphALs can be subscripted", "[GraphAL]")
{
    SECTION("GraphALs can be subscripted by key")
    {
        SECTION("Non-const GraphALs can be subscripted by key")
        {
            auto graph = create_small_graph(false, false, true);
            REQUIRE(graph[5].data() == 0);
            graph[5].data() = 42;
            REQUIRE(graph[5].data() == 42);
        }

        SECTION("Const GraphALs can be subscripted by key")
        {
            auto graph = create_small_graph(false, false, true);
            const auto& graph_ref = graph;
            REQUIRE(graph_ref[5].data() == 0);
            //graph_ref[5].data() = 42;    // uncommenting should keep test file from compiling
        }
    }

    SECTION("GraphALs can be subscripted by label")
    {
        SECTION("Non-const GraphALs can be subscripted by label")
        {
            auto graph = create_small_labeled_graph(false, false, true);
            REQUIRE(graph["St. Petersburg"].label() == "St. Petersburg");
            graph.change_label("St. Petersburg", "St. Pete");
            REQUIRE_THROWS_WITH(graph["St. Petersburg"], "Vertex with given label does not exist in graph.");
            REQUIRE(graph["St. Pete"].label() == "St. Pete");
            REQUIRE(graph["St. Pete"].data() == 0);
            graph["St. Pete"].data() = 42;
            REQUIRE(graph["St. Pete"].data() == 42);
        }

        SECTION("Const GraphALs can be subscripted by label")
        {
            auto graph = create_small_labeled_graph();
            const auto& graph_ref = graph;
            REQUIRE(graph_ref["St. Petersburg"].label() == "St. Petersburg");
            //graph_ref["St. Petersburg"].label() = "St. Pete";    // uncommenting should keep test file from compiling
        }
    }
}

TEST_CASE("Edge weight function returns the weight between two edges for a GraphAL", "[GraphAL]")
{
    SECTION("Edge weights can be found in undirected graphs")
    {
        auto graph = create_small_graph(false, true);
        REQUIRE(graph.edge_weight(1, 5) == graph.neighbors(1)[5]);
        REQUIRE(graph.edge_weight(1, 5) == graph.edge_weight(5, 1));

        REQUIRE(graph.edge_weight(4, 5) == graph.neighbors(4)[5]);
        REQUIRE(graph.edge_weight(4, 5) == graph.edge_weight(5, 4));

        REQUIRE(graph.edge_weight(2, 5) == graph.neighbors(2)[5]);
        REQUIRE(graph.edge_weight(2, 5) == graph.edge_weight(5, 2));

        REQUIRE(graph.edge_weight(4, 0) == graph.neighbors(4)[0]);
        REQUIRE(graph.edge_weight(4, 0) == graph.edge_weight(0, 4));

        REQUIRE(graph.edge_weight(5, 7) == graph.neighbors(5)[7]);
        REQUIRE(graph.edge_weight(5, 7) == graph.edge_weight(7, 5));

        REQUIRE(graph.edge_weight(5, 8) == graph.neighbors(5)[8]);
        REQUIRE(graph.edge_weight(5, 8) == graph.edge_weight(8, 5));

        REQUIRE(graph.edge_weight(7, 3) == graph.neighbors(7)[3]);
        REQUIRE(graph.edge_weight(7, 3) == graph.edge_weight(3, 7));

        REQUIRE(graph.edge_weight(8, 9) == graph.neighbors(8)[9]);
        REQUIRE(graph.edge_weight(8, 9) == graph.edge_weight(9, 8));

        REQUIRE(graph.edge_weight(9, 6) == graph.neighbors(9)[6]);
        REQUIRE(graph.edge_weight(9, 6) == graph.edge_weight(6, 9));

        REQUIRE(graph.edge_weight(3, 6) == graph.neighbors(3)[6]);
        REQUIRE(graph.edge_weight(3, 6) == graph.edge_weight(6, 3));

        REQUIRE_FALSE(graph.edge_weight(1, 4));
        REQUIRE_FALSE(graph.edge_weight(2, 4));
        REQUIRE_FALSE(graph.edge_weight(6, 7));
        REQUIRE_FALSE(graph.edge_weight(7, 9));
    }

    SECTION("Edge weights can be found in directed graphs")
    {
        auto graph = create_small_graph(true, true);
        REQUIRE(graph.edge_weight(1, 5) == graph.neighbors(1)[5]);
        REQUIRE_FALSE(graph.edge_weight(5, 1));

        REQUIRE(graph.edge_weight(4, 5) == graph.neighbors(4)[5]);
        REQUIRE_FALSE(graph.edge_weight(5, 4));

        REQUIRE(graph.edge_weight(2, 5) == graph.neighbors(2)[5]);
        REQUIRE_FALSE(graph.edge_weight(5, 2));

        REQUIRE(graph.edge_weight(4, 0) == graph.neighbors(4)[0]);
        REQUIRE_FALSE(graph.edge_weight(0, 4));

        REQUIRE(graph.edge_weight(5, 7) == graph.neighbors(5)[7]);
        REQUIRE_FALSE(graph.edge_weight(7, 5));

        REQUIRE(graph.edge_weight(5, 8) == graph.neighbors(5)[8]);
        REQUIRE_FALSE(graph.edge_weight(8, 5));

        REQUIRE(graph.edge_weight(7, 3) == graph.neighbors(7)[3]);
        REQUIRE_FALSE(graph.edge_weight(3, 7));

        REQUIRE(graph.edge_weight(8, 9) == graph.neighbors(8)[9]);
        REQUIRE_FALSE(graph.edge_weight(9, 8));

        REQUIRE(graph.edge_weight(9, 6) == graph.neighbors(9)[6]);
        REQUIRE_FALSE(graph.edge_weight(6, 9));

        REQUIRE(graph.edge_weight(3, 6) == graph.neighbors(3)[6]);
        REQUIRE_FALSE(graph.edge_weight(6, 3));

        REQUIRE_FALSE(graph.edge_weight(1, 4));
        REQUIRE_FALSE(graph.edge_weight(2, 4));
        REQUIRE_FALSE(graph.edge_weight(6, 7));
        REQUIRE_FALSE(graph.edge_weight(7, 9));
    }
}

TEST_CASE("GraphALs can be searched using breadth-first search", "[GraphAL]")
{
    SECTION("BFS accesses all vertices for an undirected, connected graph")
    {
        auto graph = create_small_graph();
        std::set<std::size_t> vertices_accessed;
        graph.bfs(1, [&](auto vertex){ vertices_accessed.insert(vertex); });
        std::size_t i = 0;
        for (std::size_t vertex_accessed : vertices_accessed) {
            REQUIRE(vertex_accessed == i);
            ++i;
        }
    }

    SECTION("BFS does not access all vertices in a directed, connected graph when some vertices are not reachable from the start vertex")
    {
        auto graph = create_small_graph(true);
        std::set<std::size_t> vertices_accessed;
        graph.bfs(1, [&](auto vertex){ vertices_accessed.insert(vertex); });
        REQUIRE(vertices_accessed.find(2) == vertices_accessed.end());
        REQUIRE(vertices_accessed.find(4) == vertices_accessed.end());
        REQUIRE(vertices_accessed.find(0) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(1) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(5) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(7) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(8) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(9) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(6) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(3) == vertices_accessed.end());
    }

    SECTION("BFS returns correct data for an undirected GraphAL")
    {
        auto graph = create_small_graph();
        auto bfs_data = graph.bfs(1);
        REQUIRE(bfs_data[1].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[1].distance == 0);
        REQUIRE(bfs_data[5].parent == 1);
        REQUIRE(bfs_data[5].distance == 1);
        REQUIRE(bfs_data[4].parent == 5);
        REQUIRE(bfs_data[4].distance == 2);
        REQUIRE(bfs_data[2].parent == 5);
        REQUIRE(bfs_data[2].distance == 2);
        REQUIRE(bfs_data[7].parent == 5);
        REQUIRE(bfs_data[7].distance == 2);
        REQUIRE(bfs_data[8].parent == 5);
        REQUIRE(bfs_data[8].distance == 2);
        REQUIRE(bfs_data[0].parent == 4);
        REQUIRE(bfs_data[0].distance == 3);
        REQUIRE(bfs_data[9].parent == 8);
        REQUIRE(bfs_data[9].distance == 3);
        REQUIRE(bfs_data[3].parent == 7);
        REQUIRE(bfs_data[3].distance == 3);
        REQUIRE((bfs_data[6].parent == 9 || bfs_data[6].parent == 3));
        REQUIRE(bfs_data[6].distance == 4);
    }

    SECTION("BFS returns correct data for a directed GraphAL")
    {
        auto graph = create_small_graph(true);
        auto bfs_data = graph.bfs(1);
        REQUIRE(bfs_data[1].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[1].distance == 0);
        REQUIRE(bfs_data[5].parent == 1);
        REQUIRE(bfs_data[5].distance == 1);
        REQUIRE(bfs_data[4].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[4].distance == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[2].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[2].distance == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[7].parent == 5);
        REQUIRE(bfs_data[7].distance == 2);
        REQUIRE(bfs_data[8].parent == 5);
        REQUIRE(bfs_data[8].distance == 2);
        REQUIRE(bfs_data[0].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[0].distance == std::numeric_limits<std::size_t>::max());
        REQUIRE(bfs_data[9].parent == 8);
        REQUIRE(bfs_data[9].distance == 3);
        REQUIRE(bfs_data[3].parent == 7);
        REQUIRE(bfs_data[3].distance == 3);
        REQUIRE((bfs_data[6].parent == 9 || bfs_data[6].parent == 3));
        REQUIRE(bfs_data[6].distance == 4);
    }
}

TEST_CASE("GraphALs can be searched using depth-first search", "[GraphAL]")
{
    SECTION("DFS accesses all vertices for an undirected, connected graph")
    {
        auto graph = create_small_graph();
        std::set<std::size_t> vertices_accessed;
        graph.dfs(1, [&](auto vertex){ vertices_accessed.insert(vertex); });
        std::size_t i = 0;
        for (std::size_t vertex_accessed : vertices_accessed) {
            REQUIRE(vertex_accessed == i);
            ++i;
        }
    }

    SECTION("DFS does not access all vertices in a directed, connected graph when some vertices are not reachable from the start vertex")
    {
        auto graph = create_small_graph(true);
        std::set<std::size_t> vertices_accessed;
        graph.dfs(1, [&](auto vertex){ vertices_accessed.insert(vertex); });
        REQUIRE(vertices_accessed.find(2) == vertices_accessed.end());
        REQUIRE(vertices_accessed.find(4) == vertices_accessed.end());
        REQUIRE(vertices_accessed.find(0) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(1) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(5) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(7) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(8) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(9) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(6) == vertices_accessed.end());
        REQUIRE_FALSE(vertices_accessed.find(3) == vertices_accessed.end());
    }

    SECTION("DFS returns correct parent data for an undirected graph")
    {
        auto graph = create_small_graph();
        auto dfs_data = graph.dfs(1);
        REQUIRE(dfs_data[1].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(dfs_data[5].parent == 1);
        REQUIRE(dfs_data[2].parent == 5);
        REQUIRE(dfs_data[4].parent == 5);
        REQUIRE(dfs_data[0].parent == 4);
        REQUIRE(dfs_data[8].parent == 5);
        REQUIRE(dfs_data[9].parent == 8);
        REQUIRE(dfs_data[6].parent == 9);
        REQUIRE(dfs_data[3].parent == 6);
        REQUIRE(dfs_data[7].parent == 3);
    }

    SECTION("DFS returns correct parent data for a directed graph")
    {
        auto graph = create_small_graph(true);
        auto dfs_data = graph.dfs(1);
        REQUIRE(dfs_data[1].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(dfs_data[5].parent == 1);
        REQUIRE(dfs_data[8].parent == 5);
        REQUIRE(dfs_data[9].parent == 8);
        REQUIRE(dfs_data[6].parent == 9);
        REQUIRE(dfs_data[7].parent == 5);
        REQUIRE(dfs_data[3].parent == 7);
        REQUIRE(dfs_data[4].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(dfs_data[2].parent == std::numeric_limits<std::size_t>::max());
        REQUIRE(dfs_data[0].parent == std::numeric_limits<std::size_t>::max());
    }
}

bool graphs_are_equal(const GraphAL<>& graph1, const GraphAL<>& graph2, std::size_t num_vertices)
{
    for (std::size_t i = 0; i < num_vertices; ++i) {
        if (graph1.neighbors(i) != graph2.neighbors(i)) {
            return false;
        }
    }

    return true;
}

TEST_CASE("GraphALs are copy-constructible", "[GraphAL]")
{
    constexpr std::size_t num_vertices = 1000;
    auto graph = create_random_graph(num_vertices).first;
    auto copied_graph = graph;
    REQUIRE(graphs_are_equal(graph, copied_graph, num_vertices));
}

TEST_CASE("GraphALs are move-constructible", "[GraphAL]")
{
    constexpr std::size_t num_vertices = 1000;
    auto orig_graph = create_random_graph(num_vertices).first;
    auto copied_graph = orig_graph;
    auto moved_graph = std::move(orig_graph);
    REQUIRE(graphs_are_equal(moved_graph, copied_graph, num_vertices));
}

TEST_CASE("GraphALs are copy-assignable", "[GraphAL]")
{
    auto orig_graph = BasicGraphBuilder<>{}.build_adj_list();
    orig_graph.add_vertex();
    orig_graph.add_vertex();
    orig_graph.add_edge(0, 1);

    constexpr std::size_t num_vertices = 1000;
    auto assigned_graph = create_random_graph(num_vertices).first;
    orig_graph = assigned_graph;
    REQUIRE(graphs_are_equal(orig_graph, assigned_graph, num_vertices));
}

TEST_CASE("GraphALs are move-assignable", "[GraphAL]")
{
    auto orig_graph = BasicGraphBuilder<>{}.build_adj_list();
    orig_graph.add_vertex();
    orig_graph.add_vertex();
    orig_graph.add_edge(0, 1);

    constexpr std::size_t num_vertices = 1000;
    auto assigned_graph = create_random_graph(num_vertices).first;
    auto copied_graph = assigned_graph;
    orig_graph = std::move(assigned_graph);
    REQUIRE(graphs_are_equal(orig_graph, copied_graph, num_vertices));
}

struct A
{
    int x;
    int y;

    friend bool operator==(const A& a, const std::pair<int, int>& p)
    {
        return a.x == p.first && a.y == p.second;
    }
};

TEST_CASE("GraphALs can hold satellite data")
{
    auto graph = BasicGraphBuilder<int, A>{}.use_satellite_data().build_adj_list();
    for (int i = 0; i < 10; ++i) {
        graph.add_vertex(A{i, i + 1});
    }

    graph.add_edge(1, 5);
    graph.add_edge(4, 5);
    graph.add_edge(2, 5);
    graph.add_edge(4, 0);
    graph.add_edge(4, 5);
    graph.add_edge(5, 7);
    graph.add_edge(5, 8);
    graph.add_edge(7, 3);
    graph.add_edge(8, 9);
    graph.add_edge(9, 6);
    graph.add_edge(3, 6);

    for (int i = 0; i < 10; ++i) {
        REQUIRE(graph[static_cast<std::size_t>(i)].data() == std::make_pair(i, i + 1));
    }

    graph.remove_vertex(0);
    for (int i = 0; i < 9; ++i) {
        REQUIRE(graph[static_cast<std::size_t>(i)].data() == std::make_pair(i + 1, i + 2));
    }
}

namespace bork_lib
{
    template<>
    struct default_edge_weight<A>
    {
        A operator()() const noexcept { return {0, 0}; }
    };
}

TEST_CASE("GraphALs can use non-arithmetic weight functions if default_weight is specialized")
{
    auto graph = BasicGraphBuilder<A>{}.directed().weighted().build_adj_list();
    for (int i = 0; i < 10; ++i) {
        graph.add_vertex();
    }

    graph.add_edge(1, 5, A{1, 5});
    graph.add_edge(4, 5, A{4, 5});
    graph.add_edge(2, 5, A{2, 5});
    graph.add_edge(4, 0, A{4, 0});
    graph.add_edge(4, 5, A{4, 5});
    graph.add_edge(5, 7, A{5, 7});
    graph.add_edge(5, 8, A{5, 8});
    graph.add_edge(7, 3, A{7, 3});
    graph.add_edge(8, 9, A{8, 9});
    graph.add_edge(9, 6, A{9, 6});
    graph.add_edge(3, 6, A{3, 6});

    for (std::size_t i = 0; i < 10; ++i) {
        for (const auto& neighbor : graph.neighbors(i)) {
            REQUIRE(neighbor.second == std::make_pair(i, neighbor.first));
        }
    }
}

TEST_CASE("GraphAL size, empty and clear functions work as expected", "[GraphAL]")
{
    SECTION("GraphAL size function returns the correct size")
    {
        constexpr int num_vertices = 1000;
        auto large_graph = create_random_graph(num_vertices).first;
        REQUIRE(large_graph.size() == num_vertices);

        auto small_graph = create_small_graph();
        REQUIRE(small_graph.size() == 10);
        small_graph.remove_vertex(5);
        REQUIRE(small_graph.size() == 9);
        small_graph.add_vertex();
        REQUIRE(small_graph.size() == 10);
    }

    SECTION("GraphAL empty function returns the correct boolean value")
    {
        auto graph = BasicGraphBuilder<>{}.build_adj_list();
        REQUIRE(graph.empty());
        graph.add_vertex();
        REQUIRE_FALSE(graph.empty());
        graph.remove_vertex(0);
        REQUIRE(graph.empty());
    }

    SECTION("GraphAL clear function successfully clears the graph")
    {
        auto graph = create_small_graph();
        graph.clear();
        REQUIRE(graph.empty());
        REQUIRE(graph.size() == 0);
        graph.add_vertex();
        REQUIRE_FALSE(graph.empty());
        REQUIRE(graph.size() == 1);
        graph.add_vertex();
        graph.add_edge(0, 1);
        REQUIRE_FALSE(graph.empty());
        REQUIRE(graph.size() == 2);
    }
}

TEST_CASE("GraphAL reserve function works as expected", "[GraphAL]")
{
    auto graph = BasicGraphBuilder<>{}.build_adj_list();
    constexpr auto min_capacity = graph.min_capacity();
    REQUIRE(graph.capacity() == min_capacity);

    for (int i = 0; i < min_capacity + 1; ++i) {
        graph.add_vertex();
    }
    REQUIRE(graph.capacity() == min_capacity * 2);

    graph.reserve(min_capacity * 4);
    REQUIRE(graph.capacity() == min_capacity * 4);
}