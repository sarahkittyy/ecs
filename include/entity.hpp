#pragma once

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

#include <registry.hpp>

namespace ecs {

/**
 * @brief entity that stores references to all components it holds
 * provides apis to add and remove components at will
 */
class entity {
public:
	~entity();

	/**
	 * @brief add a component to this entity
	 *
	 * @tparam Component the type of component to add
	 * @param args the arguments to construct the component with
	 */
	template <typename Component, typename... Args>
	void add(Args&&... args) {
		assert_alive();
		std::type_index ti(typeid(Component));
		if (m_lookup.contains(ti)) {
			throw std::runtime_error("Component already exists.");
		}
		size_t idx	 = m_reg.add<Component>(args...);
		m_lookup[ti] = idx;
	}

	/**
	 * @brief remove a component from this entity
	 *
	 * @tparam Component the component to remove
	 */
	template <typename Component>
	void remove() {
		assert_alive();
		std::type_index ti(typeid(Component));
		if (!m_lookup.contains(ti)) {
			throw std::runtime_error("Component type does not exist on entity");
		}
		m_reg.remove<Component>(m_lookup[ti]);
		m_lookup.erase(ti);
	}

	/**
	 * @brief returns true if the entity has the given components
	 *
	 * @tparam Components the type of components to test
	 */
	template <typename... Components>
	bool has() const {
		assert_alive();
		return (m_lookup.contains(std::type_index(typeid(Components))) && ...);
	}

	/**
	 * @brief returns true if the entity has the given component
	 *
	 * @param ti the type index of the component
	 */
	bool has(std::type_index ti) const;

	/**
	 * @brief returns true if the entity has the given components
	 *
	 * @param ti the type indexes of the components
	 */
	bool has(std::vector<std::type_index> ti) const;

	/**
	 * @brief retrieve the given component
	 *
	 * @tparam Component the component to retrieve
	 */
	template <typename Component>
	Component& get() {
		assert_alive();
		std::type_index ti(typeid(Component));
		if (!m_lookup.contains(ti)) {
			throw std::runtime_error("get() Component " + std::string(ti.name()) + " does not exist.");
		}
		return m_reg.get<Component>(m_lookup.at(ti));
	}

	/**
	 * @brief retrieve the given component
	 *
	 * @tparam Component the component to retrieve
	 */
	template <typename Component>
	const Component& get() const {
		assert_alive();
		std::type_index ti(typeid(Component));
		if (!m_lookup.contains(ti)) {
			throw std::runtime_error("get() Component " + std::string(ti.name()) + " does not exist.");
		}
		return m_reg.get<Component>(m_lookup.at(ti));
	}

	/**
	 * @brief clone this component
	 *
	 * @return the new component
	 */
	entity_t clone() const;

	/**
	 * @brief lookup the id of the component
	 *
	 * @param ti the type index of the component
	 * @return -1 if nonexistent, otherwise the index of that component for the respective pool
	 */
	size_t lookup_id(std::type_index ti) const;

	/**
	 * @brief true if the entity is still alive and usable
	 */
	bool alive() const;

private:
	friend class registry;
	entity(registry& r);

	/**
	 * @brief frees all stored components, sets a flag that this entity is useless,
	 * preventing method calls (called on destructor, and when removed by the registry)
	 */
	void destroy();
	bool m_destroyed;

	/**
	 * @brief check m_destroyed to make sure this entity is still active
	 */
	void assert_alive() const;

	/// registry this entity is assigned to
	registry& m_reg;

	/// map of component types to their indices in the corresponding pool
	std::unordered_map<std::type_index, size_t> m_lookup;
};

typedef std::shared_ptr<entity> entity_t;
}
