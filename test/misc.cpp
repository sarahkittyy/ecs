#include <catch2/catch.hpp>

#include <deque>
#include <ecs.hpp>
#include <functional>

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
	std::deque<nya> pos;
};

struct positionVector : public posBase {
	positionVector()
		: failable(true) {
		INFO("pv constructor called");
		pos.push_back(nya(2));
	}
	~positionVector() {
		INFO("Destructor called!");
		if (failable)
			FAIL("Destructor called!");
	}
	positionVector& operator=(const positionVector& other) {
		INFO("pv operator= called");
		std::deque<nya> test;
		failable = other.failable;
		pos		 = other.pos;
		return *this;
	}
	positionVector(positionVector&& other) {
		INFO("pv move constructor called");
		failable = std::move(other.failable);
		pos		 = std::move(other.pos);
	}
	positionVector(const positionVector& other)
		: failable(other.failable) {
		pos = other.pos;
		INFO("pv copy constructor called");
	}
	void push_five() {
		pos.push_back(nya(5));
	}
	bool failable;
};

struct fn {
	std::function<int()> internal;
};

TEST_CASE("Miscellaneous edge case testing") {
	registry reg;
	auto e	= reg.create();
	auto e2 = reg.create(), e3 = reg.create();
	e2->add<fn>([e]() -> int {
		return 3;
	});
	e->add<positionVector>();
	exit(0);
	positionVector& posV = e->get<positionVector>();
	REQUIRE(posV.pos[0].x == 2);
	e->get<positionVector>().push_five();
	REQUIRE(e->has<positionVector>() == true);
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
	e->add<fn>([e]() -> int {
		return e->get<positionVector>().pos.back().x;
	});
	for (auto& [fn] : reg.view<fn>()) {
		REQUIRE(fn.internal() == 3);
	}
	e->get<positionVector>().failable = false;
}
