#pragma once

#include <algorithm>
#include <list>
#include <memory>
#include <range/v3/all.hpp>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include <internal/component_pool.hpp>

namespace ecs {

class entity;
template <typename... Components>
class view;
typedef std::shared_ptr<entity> entity_t;

/**
 * @brief registry of entities and their components
 * provides apis to access all entities / components
 */
class registry {
public:
	/**
	 * @brief constructor
	 *
	 * @param max_components the maximum amount of each type of component
	 */
	registry(size_t max_components = 1024);
	~registry();

	registry(const registry& other) = delete;
	registry(registry&& other)		= delete;
	registry& operator=(const registry& other) = delete;
	registry& operator=(registry&& other) = delete;

	/**
	 * @brief creates an entity and adds it to the registry
	 */
	entity_t create();

	/**
	 * @brief removes an entity from the registry
	 *
	 * @param e the entity to remove
	 */
	void remove(entity_t e);

	/**
	 * @return how many entities are registered
	 */
	size_t count() const;

	/**
	 * @brief retrieves a view of all entites with the component(s) listed
	 *
	 * @tparam Components the components required for each entity returned
	 */
	template <typename... Components>
	view<Components...> view() {
		return ecs::view<Components...>(*this);
	}

	/**
	 * @brief retrieve all entities
	 */
	std::list<entity_t>& entities();

	/**
	 * @brief retrieve all entities
	 */
	const std::list<entity_t>& entities() const;

	/**
	 * @brief retrieve the pool saving the components of the given type
	 */
	component_pool& pool(std::type_index ti);

	/**
	 * @brief retrieve the pool saving the components of the given type
	 */
	const component_pool& pool(std::type_index ti) const;

private:
	friend class entity;

	const size_t MAX_COMPONENTS;   /// max amount of each type of component

	/**
	 * @brief add a component to the pool
	 *
	 * @param args the arguments to construct the component with
	 *
	 * @returns the index of that component to save
	 */
	template <typename Component, typename... Args>
	size_t add(Args&&... args) {
		std::type_index ti(typeid(Component));
		// construct the component pool of this type if it does not exist
		if (!m_components.contains(ti)) {
			m_components.try_emplace(
				ti,
				MAX_COMPONENTS,
				(size_t)sizeof(Component));
		}
		if (!m_components.contains(ti)) throw std::logic_error(";-;");
		// retrieve the pool
		component_pool& pool = m_components.at(ti);
		// add the component
		return pool.add(Component{ args... });
	}

	/**
	 * @brief remove a component from the pool
	 *
	 * @tparam Component the type of component to remove
	 * @param idx the index of the component to remove
	 */
	template <typename Component>
	void remove(size_t idx) {
		std::type_index ti(typeid(Component));
		remove(ti, idx);
	}

	/**
	 * @brief remove a component from the pool
	 *
	 * @param ti the type of component to remove
	 * @param idx the index of the component to remove
	 */
	void remove(std::type_index ti, size_t idx);

	/**
	 * @brief retrieves the specified component at index idx
	 *
	 * @tparam Component the component to retrieve
	 * @param idx the index of the component
	 *
	 * @remarks throws if the index does not exist
	 */
	template <typename Component>
	Component& get(size_t idx) {
		std::type_index ti(typeid(Component));
		return *m_components.at(ti).get<Component>(idx);
	}

	/**
	 * @brief member to produce a function that checks an entity's components,
	 * as a workaround for circular dependencies
	 */
	static std::function<bool(entity_t)> entity_has_components(std::vector<std::type_index> ts);

	static std::vector<std::type_index> get_template_pack_types() {
		return std::vector<std::type_index>();
	}

	template <typename Component>
	static std::vector<std::type_index> get_template_pack_types() {
		return std::vector<std::type_index>{ std::type_index(typeid(Component)) };
	}

	template <typename Component1, typename Component2, typename... Components>
	static std::vector<std::type_index> get_template_pack_types() {
		std::vector<std::type_index> fst;
		fst.push_back(get_template_pack_types<Component1>());
		fst.push_back(get_template_pack_types<Component2>());
		std::vector<std::type_index> rest = get_template_pack_types<Components...>();
		fst.insert(fst.end(), rest.begin(), rest.end());
		return fst;
	}

	/// vector of entities in the registry
	std::list<entity_t> m_entities;

	/// map of component types to their storage pool
public:
	std::unordered_map<std::type_index, component_pool> m_components;
};

}
