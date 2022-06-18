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

struct unused {
	double x;
};

TEST_CASE("Viewing components works.") {
	using namespace ecs;
	GIVEN("Some entities") {
		registry reg;
		auto e1 = reg.create();
		auto e2 = reg.create();
		e1->add<position>(2, 4);
		e2->add<position>(8, 9);
		e2->add<color>(.2f, .2f, .8f);
		REQUIRE(e2->get<color>().g == .2f);
		WHEN("We use a ranged-based for loop over a view of one component") {
			for (auto& [col] : reg.view<color>()) {
				THEN("We can retrieve the entity") {
					REQUIRE(col.r == 0.2f);
				}
			}
		}
		WHEN("We try fetching a component that doesn't exist") {
			THEN("A ranged based for loop never runs.") {
				for (auto& [_] : reg.view<unused>()) {
					REQUIRE(false);
				}
			}
			THEN("A callback is never called.") {
				reg.view<unused>().each([](unused& u) {
					REQUIRE(false);
				});
			}
		}
		WHEN("We retrieve a view of one component") {
			view v = reg.view<position>();
			THEN("We can iterate over it.") {
				int calls = 0;
				v.each([&calls](position& p) {
					p.x++;
					p.y++;
					calls++;
				});
				REQUIRE(calls == 2);
				REQUIRE(e1->get<position>().x == 3);
				REQUIRE(e1->get<position>().y == 5);
				REQUIRE(e2->get<position>().x == 9);
				REQUIRE(e2->get<position>().y == 10);
				calls = 0;
				v.each([&calls](entity_t t, position& p) {
					if (t->has<color>()) {
						p.x++;
						p.y++;
						calls++;
					}
				});
				REQUIRE(calls == 1);
				REQUIRE(e1->get<position>().x == 3);
				REQUIRE(e1->get<position>().y == 5);
				REQUIRE(e2->get<position>().x == 10);
				REQUIRE(e2->get<position>().y == 11);
				reg.remove(e2);
				auto e3 = reg.create();
				e3->add<color>(.2f, .2f, .8f);
				calls = 0;
				v.each([&calls](position& p) {
					p.x++;
					p.y++;
					calls++;
				});
				REQUIRE(calls == 1);
				REQUIRE(e1->get<position>().x == 4);
				REQUIRE(e1->get<position>().y == 6);
				REQUIRE(e3->get<color>().r == .2f);
				REQUIRE(e3->get<color>().g == .2f);
				REQUIRE(e3->get<color>().b == .8f);
				REQUIRE_THROWS(e2->get<position>().x == 11);
				REQUIRE_THROWS(e2->get<position>().y == 12);
			}
		}
		WHEN("We retrieve a view of two components") {
			view v = reg.view<position, color>();
			THEN("We can iterate over it.") {
				int calls = 0;
				v.each([&calls](position& p, color& c) {
					p.x++;
					p.y++;
					calls++;
				});
				REQUIRE(calls == 1);
				REQUIRE(e1->get<position>().x == 2);
				REQUIRE(e1->get<position>().y == 4);
				REQUIRE(e2->get<position>().x == 9);
				REQUIRE(e2->get<position>().y == 10);
				calls = 0;
				v.each([&calls](entity_t t, position& p, color& c) {
					if (!t->has<color>()) {

						p.x++;
						p.y++;
						calls++;
					}
				});
				REQUIRE(calls == 0);
				REQUIRE(e1->get<position>().x == 2);
				REQUIRE(e1->get<position>().y == 4);
				REQUIRE(e2->get<position>().x == 9);
				REQUIRE(e2->get<position>().y == 10);
			}
			THEN("We can iterate over it using const") {
				int calls = 0;
				v.each([&calls](const position& p, const color& c) {
					calls++;
				});
				REQUIRE(calls == 1);
				REQUIRE(e1->get<position>().x == 2);
				REQUIRE(e1->get<position>().y == 4);
				REQUIRE(e2->get<position>().x == 8);
				REQUIRE(e2->get<position>().y == 9);
			}
		}
	}
}
