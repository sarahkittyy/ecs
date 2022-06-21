
#include <catch2/catch.hpp>

#include <deque>
#include <ecs.hpp>

using ecs::registry;

struct positionVector {
	positionVector()
		: pos() {
		pos.push_back(nya(2));
	}
	struct nya {
		nya(int x)
			: x(x) {
		}
		int x;
	};
	void push_five() {
		pos.push_back(nya(5));
	}
	std::deque<nya> pos;
};

TEST_CASE("Miscellaneous edge case testing") {
	registry reg;
	auto e = reg.create();
	e->add<positionVector>();
	positionVector& posV = e->get<positionVector>();
	REQUIRE(posV.pos[0].x == 2);
	e->get<positionVector>().push_five();
	posV.pos.push_back(positionVector::nya(3));
	REQUIRE(posV.pos[1].x == 5);
	REQUIRE(posV.pos[2].x == 3);
	std::deque<positionVector::nya>& pos2 = e->get<positionVector>().pos;
	REQUIRE(pos2[1].x == 5);
	REQUIRE(pos2[2].x == 3);
}
