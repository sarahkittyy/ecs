#pragma once

#include <functional>
#include <memory>
#include <range/v3/all.hpp>
#include <type_traits>
#include <typeindex>

#include <internal/component_pool.hpp>
#include <internal/view_iterator.hpp>

#include <entity.hpp>
#include <registry.hpp>
#include "range/v3/view/filter.hpp"

namespace ecs {

/**
 * @brief provides an interface for accessing requested entities / components
 */
template <typename... Components>
class view {
public:
	/**
	 * @brief runs a callback on each set of components
	 *
	 * @param callback
	 */
	void each(std::function<void(Components&...)> callback) {
		// clang-format off
		std::vector<entity_t> ents = m_reg.entities() 
			| ranges::views::filter([](entity_t e) {
				return e->has<Components...>();
			})
			| ranges::to<std::vector<entity_t>>();
		// clang-format on
		for (auto& ent : ents) {
			callback(ent->get<Components>()...);
		}
	}

	/**
	 * @brief runs a callback on each entity and their set of components
	 *
	 * @param callback
	 */
	void each(std::function<void(entity_t, Components&...)> callback) {
		// clang-format off
		std::vector<entity_t> ents = m_reg.entities() 
			| ranges::views::filter([](entity_t e) {
				return e->has<Components...>();
			})
			| ranges::to<std::vector<entity_t>>();
		// clang-format on
		for (auto& ent : ents) {
			callback(ent, ent->get<Components>()...);
		}
	}

	/**
	 * @brief iterator that points to the end of the data
	 *
	 * @remarks becomes invalid if the registry is modified during iteration
	 */
	view_iterator<Components...> begin() {
		return view_iterator<Components...>(m_reg, 0);
	}

	/**
	 * @brief iterator that points to the end of the data
	 *
	 * @remarks becomes invalid if the registry is modified during iteration
	 */
	view_iterator<Components...> end() {
		return view_iterator<Components...>(m_reg, m_reg.count());
	}

private:
	friend class registry;
	friend class view_iterator<Components...>;

	/**
	 * @brief can only be instantiated by the registry api
	 */
	view(registry& r)
		: m_reg(r) {
	}

	/// the registry this view originates from
	registry& m_reg;
};
}
