#include "entity.hpp"

#include <algorithm>

namespace ecs {

entity::entity(registry& r)
	: m_destroyed(false),
	  m_reg(r),
	  m_lookup() {
}

entity::~entity() {
	destroy();
}

void entity::destroy() {
	if (m_destroyed) return;
	// remove all corresponding components
	for (auto& [key, value] : m_lookup) {
		m_reg.remove(key, value);
	}
	m_destroyed = true;
}

void entity::assert_alive() const {
	if (m_destroyed) {
		throw std::runtime_error("Entity has been destroyed!");
	}
}

bool entity::alive() const {
	return !m_destroyed;
}

entity_t entity::clone() const {
	assert_alive();
	auto ent = m_reg.create();
	// for each component
	for (auto& [ti, idx] : m_lookup) {
		// get the relevant pool
		component_pool& p = m_reg.pool(ti);
		// clone the component
		size_t slot = p.clone(idx);
		// add it to the new entity
		ent->m_lookup[ti] = slot;
	}
	return ent;
}

size_t entity::lookup_id(std::type_index ti) const {
	assert_alive();
	return m_lookup.at(ti);
}

bool entity::has(std::type_index ti) const {
	assert_alive();
	return m_lookup.contains(ti);
}

bool entity::has(std::vector<std::type_index> tis) const {
	assert_alive();
	return std::all_of(tis.begin(), tis.end(), [this](const std::type_index& ti) {
		return m_lookup.contains(ti);
	});
}

}
