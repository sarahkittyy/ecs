#include "internal/component_pool.hpp"

#include <cstring>

namespace ecs {

component_pool::component_pool(size_t max_sz, size_t comp_sz)
	: MAX_SZ(max_sz),
	  COMP_SZ(comp_sz),
	  m_data(new char[max_sz * comp_sz]()),
	  m_next_slot(0),
	  m_storage_bitmask(new char[(max_sz / 8) + 1]()),
	  m_count(0) {
}

component_pool::~component_pool() {
	delete[] m_data;
	delete[] m_storage_bitmask;
}

size_t component_pool::clone(size_t idx) {
	if (m_next_slot >= MAX_SZ) {
		throw std::out_of_range(
			"Not enough room in the component pool! \
				(make the max size larger)");
	}
	if (!is_set(idx)) {
		throw std::out_of_range("No component to clone!");
	}
	size_t slot = m_next_slot;
	// clone every bit
	std::memcpy(m_data + (slot * COMP_SZ), m_data + (idx * COMP_SZ), COMP_SZ);
	set_bit(slot);
	find_free_slot();
	m_count++;
	return slot;
}

void component_pool::remove(size_t idx) {
	if (idx >= MAX_SZ) {
		throw std::out_of_range("Attempt to remove a component out of range.");
	}
	if (!is_set(idx)) {
		throw std::runtime_error("Attempt to remove component that does not exist");
	}
	// unset that bit
	unset_bit(idx);
	m_count--;
	// if this slot is earlier than the current next available slot,
	if (m_next_slot > idx) {
		//..then update this to be the next free slot
		m_next_slot = idx;
	}
}

size_t component_pool::count() const {
	return m_count;
}

void component_pool::set_bit(size_t idx) {
	// split index into byte and bit lookup
	size_t byte_idx = idx / 8;
	size_t bit_idx	= idx % 8;
	// get byte
	char& byte = m_storage_bitmask[byte_idx];
	// set that bit to 1
	byte |= (1 << bit_idx);
}

void component_pool::unset_bit(size_t idx) {
	// split index into byte and bit lookup
	size_t byte_idx = idx / 8;
	size_t bit_idx	= idx % 8;
	// get byte
	char& byte = m_storage_bitmask[byte_idx];
	// set that bit to 0
	byte &= ~(1 << bit_idx);
}

bool component_pool::is_set(size_t idx) const {
	// split index into byte and bit lookup
	size_t byte_idx = idx / 8;
	size_t bit_idx	= idx % 8;
	// get byte
	char& byte = m_storage_bitmask[byte_idx];
	return byte & (1 << bit_idx);
}

void component_pool::find_free_slot() {
	// get the current bitmask byte
	size_t byte_idx = m_next_slot / 8;
	// iterate from this byte to the end
	for (size_t i = byte_idx; i < MAX_SZ; ++i) {
		// ignore if this byte is entirely full
		char byte = m_storage_bitmask[i];
		if (byte == (char)0xFF) continue;
		for (size_t j = 0; j < 8; ++j) {
			if (byte & (1 << j)) continue;
			m_next_slot = i * 8 + j;
			return;
		}
	}
	throw std::runtime_error("No free slots available.");
}

}
