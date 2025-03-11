#include "helpers/test_helpers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include "../source/Parameters.h"

TEST_CASE("Parameters class functionality", "[Parameters]")
{
    Parameters params;

    SECTION("Adding and getting parameters")
    {
        /* Add a new parameter and check its values */
        params.addParameter("ArpEnable", 117, 0, 0, 1);
        auto param1 = params.getParameter("ArpEnable");
        REQUIRE(param1.cc == 117);
        REQUIRE(param1.value == 0);
        REQUIRE(param1.minValue == 0);
        REQUIRE(param1.maxValue == 1);
        REQUIRE(param1.updated == false);
        REQUIRE(param1.name == "ArpEnable");
    }

    SECTION("Setting existing parameters")
    {
        // Add a parameter first
        params.addParameter("ArpEnable", 117, 0, 0, 1);

        // Update the parameter's value
        params.setParameter("ArpEnable", 1);
        auto param1 = params.getParameter("ArpEnable");
        REQUIRE(param1.cc == 117);
        REQUIRE(param1.value == 1);
        REQUIRE(param1.minValue == 0);
        REQUIRE(param1.maxValue == 1);
        REQUIRE(param1.updated == true);
        REQUIRE(param1.name == "ArpEnable");
    }

    SECTION("Handling non-existent parameters")
    {
        // Attempt to set a non-existent parameter
        REQUIRE_THROWS_AS(params.setParameter("NonExistent", 1), std::runtime_error);

        // Attempt to get a non-existent parameter
        REQUIRE_THROWS_AS(params.getParameter("NonExistent"), std::runtime_error);

        // Attempt to get a non-existent parameter value
        REQUIRE_THROWS_AS(params.getParameterValue("NonExistent"), std::runtime_error);
    }

    SECTION("Adding duplicate parameters")
    {
        // Add a parameter
        params.addParameter("ArpEnable", 117, 0, 0, 1);

        // Attempt to add the same parameter again
        REQUIRE_THROWS_AS(params.addParameter("ArpEnable", 117, 0, 0, 1), std::runtime_error);
    }

    SECTION("Adding and getting multiple parameters")
    {
        // Add multiple parameters
        params.addParameter("Param1", 101, 10, 0, 100);
        params.addParameter("Param2", 102, 20, 0, 200);
        params.addParameter("Param3", 103, 30, 0, 300);

        // Check the values of the added parameters
        auto param1 = params.getParameter("Param1");
        REQUIRE(param1.cc == 101);
        REQUIRE(param1.value == 10);
        REQUIRE(param1.minValue == 0);
        REQUIRE(param1.maxValue == 100);
        REQUIRE(param1.updated == false);
        REQUIRE(param1.name == "Param1");

        auto param2 = params.getParameter("Param2");
        REQUIRE(param2.cc == 102);
        REQUIRE(param2.value == 20);
        REQUIRE(param2.minValue == 0);
        REQUIRE(param2.maxValue == 200);
        REQUIRE(param2.updated == false);
        REQUIRE(param2.name == "Param2");

        auto param3 = params.getParameter("Param3");
        REQUIRE(param3.cc == 103);
        REQUIRE(param3.value == 30);
        REQUIRE(param3.minValue == 0);
        REQUIRE(param3.maxValue == 300);
        REQUIRE(param3.updated == false);
        REQUIRE(param3.name == "Param3");
    }

    SECTION("Reading parameter value directly")
    {
        // Add a parameter first
        params.addParameter("ArpEnable", 117, 0, 0, 1);

        // Update the parameter's value
        params.setParameter("ArpEnable", 1);

        // Read the parameter's value directly
        int value = params.getParameterValue("ArpEnable");
        REQUIRE(value == 1);
    }
}