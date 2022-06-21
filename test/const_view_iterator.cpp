#include <catch2/catch.hpp>

#include "ecs.hpp"

using namespace ecs;

struct cat {
	int nya;
	int meow;
};

TEST_CASE("Testing const view()s") {
	ecs::registry r;
	auto e = r.create();
	cat& c = e->add<cat>(4, 8);
	REQUIRE(c.nya == 4);
	REQUIRE(c.meow == 8);
	const ecs::registry& cr = r;
	SECTION("Range-based for loop") {
		for (const auto& [cat_component] : cr.view<cat>()) {
			REQUIRE(cat_component.nya == 4);
			REQUIRE(cat_component.meow == 8);
		}
	}
	SECTION("each w/o entity") {
		cr.view<cat>().each([](const cat& c) {
			REQUIRE(c.nya == 4);
			REQUIRE(c.meow == 8);
		});
	}
	SECTION("each w/ entity") {
		cr.view<cat>().each([](entity_t e, const cat& c) {
			REQUIRE(c.nya == 4);
			REQUIRE(c.meow == 8);
			REQUIRE(e->get<cat>().meow == 8);
		});
	}
}
