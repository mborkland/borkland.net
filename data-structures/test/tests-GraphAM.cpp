#include "../../catch/catch.hpp"
#include "../src/GraphBuilder.hpp"

using bork_lib::GraphBuilder;
using bork_lib::GraphAM;

TEST_CASE("GraphAM can be constructed using GraphBuilder", "[GraphAM]")
{
    SECTION("Default graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.build_adj_matrix();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Weighted graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.weighted().build_adj_matrix();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(graph.weighted());
    }

    SECTION("Graph with satellite data can be constructed")
    {
        auto graph = GraphBuilder<>{}.use_satellite_data().build_adj_matrix();
        REQUIRE(!graph.directed());
        REQUIRE(graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Directed graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.directed().build_adj_matrix();
        REQUIRE(graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Labeled graph can be constructed")
    {
        auto graph = GraphBuilder<>{}.labeled().build_adj_matrix();
        REQUIRE(!graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(graph.labeled());
        REQUIRE(!graph.weighted());
    }

    SECTION("Graph with multiple build options can be constructed")
    {
        auto graph = GraphBuilder<>{}.directed().weighted().build_adj_matrix();
        REQUIRE(graph.directed());
        REQUIRE(!graph.has_satellite_data());
        REQUIRE(!graph.labeled());
        REQUIRE(graph.weighted());
    }
}

