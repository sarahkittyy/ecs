#include <catch2/catch.hpp>

#include <internal/component_pool.hpp>

TEST_CASE("Component pools work", "[component_pool]") {
	using namespace ecs;
	GIVEN("A component pool") {
		component_pool cp(2, sizeof(double));
		REQUIRE(cp.count() == 0);
		WHEN("Components are added") {
			size_t i = cp.add<double>(5.0);
			size_t j = cp.add<double>(3.0);
			REQUIRE(i == 0);
			REQUIRE(j == 1);
			THEN("The size updates accordingly") {
				REQUIRE(cp.count() == 2);
			}
			THEN("They can be retrieved") {
				REQUIRE(*cp.get<double>(i) == 5.0);
				REQUIRE(*cp.get<double>(j) == 3.0);
			}
			THEN("They can be removed") {
				cp.remove(i);
				REQUIRE_THROWS(cp.get<double>(i));
				REQUIRE(cp.count() == 1);
				cp.remove(j);
				REQUIRE_THROWS(cp.get<double>(j));
				REQUIRE(cp.count() == 0);
				REQUIRE_THROWS(cp.remove(i));
			}
		}
		WHEN("Too many components are added") {
			cp.add<double>(5.0);
			cp.add<double>(3.0);
			THEN("It throws") {
				REQUIRE_THROWS(cp.add<double>(8.0));
			}
		}
		WHEN("Nonexistent components are removed") {
			THEN("It throws") {
				REQUIRE_THROWS(cp.remove(0));
			}
		}
		WHEN("A hole is created") {
			size_t i = cp.add<double>(5.0);
			size_t j = cp.add<double>(3.0);
			REQUIRE(i == 0);
			REQUIRE(j == 1);
			cp.remove(i);
			THEN("A new added component fills that hole") {
				REQUIRE(cp.add<double>(2.8) == 0);
			}
		}
	}
}
