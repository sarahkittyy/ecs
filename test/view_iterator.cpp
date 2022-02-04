#include <catch2/catch.hpp>
#include <iostream>

#include <ecs.hpp>

struct health {
	int x;
};

struct damage {
	int x;
};

TEST_CASE("View iterators work.") {
	using namespace ecs;
	GIVEN("Some entities") {
		registry reg;
		auto e1 = reg.create();
		e1->add<health>(5);
		e1->add<damage>(3);
		auto e2 = e1->clone();
		auto e3 = e1->clone();
		auto e4 = e1->clone();
		WHEN("We create a view of one component") {
			view v = reg.view<health>();
			THEN("We can iterate over it using a for range") {
				int count = 0;
				for (auto [health] : v) {
					count++;
					REQUIRE(health.x == 5);
				}
				REQUIRE(count == 4);
			}
			THEN("Iterating over lets us change the values") {
				int count = 0;
				for (auto& [health] : v) {
					count++;
					health.x++;
				}
				REQUIRE(e1->get<health>().x == 6);
				REQUIRE(e2->get<health>().x == 6);
				REQUIRE(e3->get<health>().x == 6);
				REQUIRE(e4->get<health>().x == 6);
				REQUIRE(count == 4);
			}
		}
		WHEN("We create a view of two components") {
			view v = reg.view<health, damage>();
			THEN("We can iterate over it using a for range.") {
				int count = 0;
				for (auto& [health, damage] : v) {
					count++;
					REQUIRE(health.x == 5);
					REQUIRE(damage.x == 3);
				}
				REQUIRE(count == 4);
			}
		}
	}
}
