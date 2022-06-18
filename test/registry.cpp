#include <catch2/catch.hpp>

#include <ecs.hpp>

struct position {
	int x;
	int y;
};

struct color {
	float r;
	float g;
	float b;
};

TEST_CASE("Entity registry works.") {
	using namespace ecs;
	GIVEN("An entity registry.") {
		registry reg;
		GIVEN("An entity") {
			auto e1 = reg.create();
			WHEN("We add a component to it") {
				e1->add<position>(5, 3);
				REQUIRE(e1->has<position>());
				REQUIRE(!e1->has<color>());
				THEN("The entity exists") {
					REQUIRE(reg.count() == 1);
				}
				THEN("We can remove it") {
					e1->remove<position>();
					REQUIRE(!e1->has<position>());
				}
				THEN("We cannot remove a component that is not attached.") {
					REQUIRE_THROWS(e1->remove<color>());
				}
			}
			WHEN("We add multiple components to it") {
				e1->add<position>(5, 3);
				e1->add<color>(.2f, .2f, .8f);
				THEN("The entity has those components.") {
					REQUIRE(e1->has<position, color>());
				}
			}
			WHEN("We clone it") {
				e1->add<position>(4, 8);
				auto e2 = e1->clone();
				THEN("The components are the same.") {
					REQUIRE(e1->get<position>().x == e2->get<position>().x);
					REQUIRE(e1->get<position>().y == e2->get<position>().y);
				}
				THEN("It's in the registry.") {
					REQUIRE(reg.count() == 2);
				}
			}
		}
	}
}
