#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>

namespace ecs {

/**
 * @brief stores a specific type of entity
 */
class component_pool {
public:
	component_pool(size_t max_sz, size_t comp_sz);
	~component_pool();

	component_pool(const component_pool& other) = delete;
	component_pool(component_pool&& other)		= delete;

	/**
	 * @brief add a component to the pool
	 *
	 * @param t the component data
	 * @return the index of that component
	 *
	 * @remarks throws if the pool is full
	 */
	template <typename Component, typename... Args>
	size_t add(Args&&... args) {
		if (sizeof(Component) != COMP_SZ) {
			throw std::runtime_error("Attempt to add a component of incompatible size.");
		}
		if (m_next_slot >= MAX_SZ) {
			throw std::out_of_range(
				"Not enough room in the component pool! \
				(make the max size larger)");
		}
		size_t slot		= m_next_slot;
		Component* data = reinterpret_cast<Component*>(m_data);
		new (data + slot)(Component){ args... };

		set_bit(slot);
		find_free_slot();
		m_count++;
		return slot;
	}

	/**
	 * @brief clone an index into the next available slot
	 *
	 * @param idx the index of the component to clone
	 * @return the index of the cloned component
	 *
	 * @remarks This is a shallow clone!
	 */
	size_t clone(size_t idx);

	/**
	 * @brief retrieve a component given the index
	 *
	 * @param idx the index of the component
	 * @return the component at that index
	 *
	 * @remarks throws if that component index does not exist
	 */
	template <typename T>
	T* get(size_t idx) {
		if (idx >= MAX_SZ) {
			throw std::out_of_range("Attempt to get a component out of bounds.");
		}
		if (!is_set(idx)) {
			throw std::out_of_range("Component at index does not exist.");
		}
		return reinterpret_cast<T*>(m_data) + idx;
	}

	/**
	 * @brief remove a component from the pool
	 *
	 * @param idx the element index to remove
	 */
	void remove(size_t idx);

	/**
	 * @return how many components are currently stored
	 */
	size_t count() const;

private:
	/// the maximum amount of components this pool can store
	const size_t MAX_SZ;
	/// the size (in bytes) of each component
	const size_t COMP_SZ;

	/// the stored components
	char* m_data;
	/// the next open slot in the component
	size_t m_next_slot;
	/// a collection of bits, each set to 1 if the corresponding data block is used
	char* m_storage_bitmask;
	/// how many components are currently stored
	size_t m_count;

	/// finds the next free slot >m_next_slot
	void find_free_slot();

	/// sets the bit in the storage bitmask
	void set_bit(size_t idx);
	/// unsets the bit in the storage bitmask
	void unset_bit(size_t idx);
	/// check if bit is set
	bool is_set(size_t idx) const;
};

}
