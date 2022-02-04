# ecs ![GitHub Workflow Status](https://img.shields.io/github/workflow/status/sarahkittyy/ecs/CMake)

I saw https://github.com/skypjack/entt and challenged myself to make my own (without cheating, of course ;)

## Example Usage

```cpp
#include <ecs.hpp>
#include <cassert>

struct position {
	int x;
	int y;
};

struct velocity {
	int x;
	int y;
};

void movement_system(registry& reg) {
	// view all objects with a position and velocity
	view v = reg.view<position, velocity>();
	// iteration via a range-based for loop
	for(auto& [position, velocity] : v) {
		position.x += velocity.x;
		position.y += velocity.y;
	}
	/************
	// iteration via a callback
	v.each([](position& p, velocity& v) {
		p.x += v.x;
		p.y += v.y;
	});
	// iteration via a callback, with access to the entity
	v.each([](entity_t e, position& p, velocity& v) {
		p.x += v.x;
		p.y += v.y;
	});
	************/
}

void game_tick(registry& reg) {
	movement_system(reg);
}

int main() {
	using namespace ecs;
	registry reg;
	auto entity = reg.create();
	entity->add<position>(0, 0);
	entity->add<velocity>(2, 3);
	// initial entity state
	assert(entity->get<position>().x == 0);
	assert(entity->get<position>().y == 0);
	assert(entity->get<velocity>().x == 2);
	assert(entity->get<velocity>().y == 3);
	// update the game 3 times
	game_tick(reg);
	game_tick(reg);
	game_tick(reg);
	// entity state after ticks
	assert(entity->get<position>().x == 6);
	assert(entity->get<position>().y == 9);
	assert(entity->get<velocity>().x == 2);
	assert(entity->get<velocity>().y == 3);

	return 0;
}
```

## Requirements

* C++20
* [Catch2](https://github.com/catchorg/Catch2) for testing.

## Building

```bash
git clone --recurse-submodules https://github.com/sarahkittyy/ecs.git
cd ecs
mkdir build
cd build
cmake ..
make
```

Output is then located in `build/`

## Testing

```bash
cd build
make test
```

## Docs

Find the docs [here!](https://sarahkittyy.github.io/ecs/)

## Including in your project with CMake

```cmake
add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/ecs)

add_dependency(target ecs)
target_link_library(target ecs)
target_include_directories(ecs
		PRIVATE
		${CMAKE_SOURCE_DIR}/submodules/ecs/include/
)
```
