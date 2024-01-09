#include <stdint.h>
#include <iostream>
#include <random>
const double tol = 1e-6;
#include <cmath>

#include <Util/util.h>

std::default_random_engine e;
std::uniform_int_distribution<uint64_t> U(0,-1);

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

SCENARIO("Fixed length number", "[utility]")
{
    GIVEN("The number 0.1234")
    {
        float x = 0.1234;
        THEN("fixedLengthString(x,0) is \"\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,0) == "");
        }
        THEN("fixedLengthString(x,1) is \"0\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,1) == "0");
        }
        THEN("fixedLengthString(x,2) is \"0.\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,2) == "0.");
        }
        THEN("fixedLengthString(x,3) is \"0.1\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,3) == "0.1");
        }
        THEN("fixedLengthString(x,4) is \"0.12\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,4) == "0.12");
        }
        THEN("fixedLengthString(x,5) is \"0.123\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,5) == "0.123");
        }
        THEN("fixedLengthString(x,6) is \"0.1234\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,6) == "0.1234");
        }
        THEN("fixedLengthString(x,7) is \"0.12340\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,7) == "0.12340");
        }
        THEN("fixedLengthString(x,64) is \"0.12340000000000000000000000000000000000000000000000000000000000\"")
        {
            REQUIRE(jGL::Util::fixedLengthNumber(x,64) == "0.12340000000000000000000000000000000000000000000000000000000000");
        }
    }
}

