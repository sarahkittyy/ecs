#include <catch2/catch.hpp>

#include <ecs.hpp>

using ecs::registry;
using ecs::view;

struct position {
	int x;
	int y;
};

struct velocity {
	int x;
	int y;
};

static void movement_system(registry& reg) {
	// view all objects with a position and velocity
	view v = reg.view<position, velocity>();
	// iteration via a range-based for loop
	for (auto& [position, velocity] : v) {
		position.x += velocity.x;
		position.y += velocity.y;
	}
	exit(0);
}

static void game_tick(registry& reg) {
	movement_system(reg);
}

TEST_CASE("Example in readme works.") {
	registry reg;
	auto entity = reg.create();
	entity->add<position>(0, 0);
	entity->add<velocity>(2, 3);
	// initial entity state
	REQUIRE(entity->get<position>().x == 0);
	REQUIRE(entity->get<position>().y == 0);
	REQUIRE(entity->get<velocity>().x == 2);
	REQUIRE(entity->get<velocity>().y == 3);
	// update the game 3 times
	game_tick(reg);
	game_tick(reg);
	game_tick(reg);
	// entity state after ticks
	REQUIRE(entity->get<position>().x == 6);
	REQUIRE(entity->get<position>().y == 9);
	REQUIRE(entity->get<velocity>().x == 2);
	REQUIRE(entity->get<velocity>().y == 3);
}
