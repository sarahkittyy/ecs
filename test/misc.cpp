#include <catch2/catch.hpp>

#include <deque>
#include <ecs.hpp>

using ecs::registry;

struct nya {
	nya(int x)
		: x(x) {
	}
	int x;
};

struct posBase {
	posBase() {
	}
	posBase& operator=(const posBase& other) = delete;
	posBase(const posBase& other)			 = delete;
	std::deque<nya> pos;
};

struct positionVector : public posBase {
	positionVector() {
		pos.push_back(nya(2));
	}
	void push_five() {
		pos.push_back(nya(5));
	}
};

TEST_CASE("Miscellaneous edge case testing") {
	registry reg;
	auto e = reg.create();
	e->add<positionVector>();
	exit(0);
	positionVector& posV = e->get<positionVector>();
	REQUIRE(posV.pos[0].x == 2);
	e->get<positionVector>().push_five();
	posV.pos.push_back(nya(3));
	REQUIRE(posV.pos[1].x == 5);
	REQUIRE(posV.pos[2].x == 3);
	std::deque<nya>& pos2 = e->get<positionVector>().pos;
	REQUIRE(pos2[1].x == 5);
	REQUIRE(pos2[2].x == 3);
	const registry& creg = reg;
	for (const auto& [posVC] : creg.view<positionVector>()) {
		REQUIRE(posVC.pos[0].x == 2);
		REQUIRE(posVC.pos[1].x == 5);
		REQUIRE(posVC.pos[2].x == 3);
	}
}
