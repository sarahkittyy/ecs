#include "registry.hpp"

#include <functional>
#include <stdexcept>

#include <entity.hpp>

namespace ecs {

registry::registry(size_t max_components)
	: MAX_COMPONENTS(max_components) {
}

registry::~registry() {
	for (auto& ent : m_entities) {
		ent->destroy();
	}
	m_entities.clear();
}

entity_t registry::create() {
	m_entities.push_back(std::shared_ptr<entity>(new entity(*this)));
	return m_entities.back();
}

void registry::remove(entity_t e) {
	e->destroy();
	std::erase(m_entities, e);
}

void registry::remove(std::type_index ti, size_t idx) {
	if (m_components.contains(ti)) {
		m_components.at(ti).remove(idx);
	}
}

size_t registry::count() const {
	return m_entities.size();
}

std::list<entity_t>& registry::entities() {
	return m_entities;
}

const std::list<entity_t>& registry::entities() const {
	return m_entities;
}

component_pool& registry::pool(std::type_index ti) {
	if (!m_components.contains(ti)) {
		throw std::out_of_range("Type not in component pool.");
	}
	return m_components.at(ti);
}

const component_pool& registry::pool(std::type_index ti) const {
	if (!m_components.contains(ti)) {
		throw std::out_of_range("Type not in component pool.");
	}
	return m_components.at(ti);
}

std::function<bool(entity_t)> registry::entity_has_components(std::vector<std::type_index> ts) {
	return [&ts](entity_t e) -> bool {
		return e->has(ts);
	};
}

}
