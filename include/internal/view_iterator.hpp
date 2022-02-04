#pragma once

#include <entity.hpp>
#include <registry.hpp>

#include <stdexcept>
#include <tuple>

namespace ecs {

template <typename... Components>
class view;

/**
 * @brief Iterates over a view's components
 *
 * @tparam Components the components that the view iterator runs through
 */
template <typename Component, typename... Components>
class view_iterator {
public:
	/**
	 * @brief construct the iterator
	 *
	 * @param reg the registry this iterates over
	 */
	view_iterator(registry& reg, size_t idx)
		: m_reg(reg),
		  m_entity_count(reg.count()) {
		if (idx >= reg.count()) {
			m_iter = m_reg.entities().end();
		} else {
			m_iter = m_reg.entities().begin();
			while (idx-- > 0) {
				m_iter++;
			}
		}
		compute_tuple();
	}

	/// dereferencing
	std::tuple<Component&, Components&...>& operator*() {
		return *m_tuple;
	}
	std::tuple<Component&, Components&...>* operator->() {
		return m_tuple.get();
	}

	/// increment
	view_iterator<Component, Components...>& operator++() {
		m_iter++;
		compute_tuple();
		return *this;
	}
	view_iterator<Component, Components...> operator++(int) {
		auto tmp = *this;
		++(*this);
		compute_tuple();
		return tmp;
	}

	/// equality
	friend bool operator==(const view_iterator<Component, Components...>& a,
						   const view_iterator<Component, Components...>& b) {
		return a.m_iter == b.m_iter;
	}
	friend bool operator!=(const view_iterator<Component, Components...>& a,
						   const view_iterator<Component, Components...>& b) {
		return !(a == b);
	}

private:
	/// internal registry
	registry& m_reg;
	/// how many entities were there when this iterator was initialized
	const int m_entity_count;
	/// registry entity list iterator
	std::list<entity_t>::iterator m_iter;
	/// the current tuple of components we are at
	std::unique_ptr<std::tuple<Component&, Components&...>> m_tuple;
	/// compute the current tuple given m_idx
	void compute_tuple() {
		if (m_iter == m_reg.entities().end()) return;
		entity_t ent = *m_iter;
		m_tuple.reset(
			new std::tuple<Component&, Components&...>(
				ent->get<Component>(),
				ent->get<Components>()...));
	}
};

}
