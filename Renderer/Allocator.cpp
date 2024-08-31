#include "Allocator.hpp"
#include <cstring>

Allocator::~Allocator()
{
	if (m_buffer)
	{
		delete[] static_cast<unsigned char*>(m_buffer);
		m_buffer = nullptr;
	}
}

RESULT_VALUE Allocator::Init(size_t bufferSize, size_t alignment, size_t maxElements)
{
	if (m_buffer)
	{
		return RESULT_VALUE::ALLOCATOR_ALREADY_INITIALIZED;
	}

	const size_t registryAlloc = maxElements * sizeof(AddressRegistry);

	m_alignment = alignment;
	m_allocated = registryAlloc + alignValue(bufferSize, alignment);
	m_buffer = new unsigned char[m_allocated];
	memset(m_buffer, 0, m_allocated);

	{	// registry
		unsigned char* valuePtr = static_cast<unsigned char*>(m_buffer);
		m_registry.reg = reinterpret_cast<AddressRegistry::Register*>(valuePtr);
		m_registry.capacity = maxElements;
	}
	m_consumed += registryAlloc;

	return RESULT_VALUE::OK;
}

void Allocator::Resize(size_t newSize, size_t alignment)
{
	if (m_buffer)
	{
		delete[] static_cast<unsigned char*>(m_buffer);
		m_buffer = nullptr; // necessary for proper Init()
	}
	m_consumed = 0;
	Init(newSize, alignment);
}

RESULT_VALUE Allocator::Allocate(void*& ptr, size_t amount)
{
	if (m_allocated == 0 || m_allocated == sizeof(AddressRegistry) * m_registry.capacity)
	{
		return RESULT_VALUE::ALLOCATOR_NOT_INITIALIZED;
	}
	if (!amount)
	{
		return RESULT_VALUE::REQUESTED_AMOUNT_IS_ZERO;
	}
	if (amount > (m_allocated - m_consumed))
	{
		return RESULT_VALUE::REQUESTED_AMOUNT_EXCEEDS_AVAILABLE_MEMORY;
	}
	if (ptr != nullptr)
	{
		return RESULT_VALUE::INVALID_POINTER;
	}

	// check if there's somewhere already free'd to reallocate first
	// todo: gotta implement some "merge" operation though
	for (size_t i = 0; i < m_registry.size; i++)
	{
		// check if there's a spot, if it fits and if the size requested is at least 1/2 of the spot
		if ((m_registry.reg[i].inUse == false) && (m_registry.reg[i].capacity >= amount) && (amount >= m_registry.reg[i].capacity / 2))
		{
			unsigned char* address = static_cast<unsigned char*>(m_buffer) + m_registry.reg[i].offsetIntoBuffer;
			ptr = static_cast<void*>(address);
			m_registry.reg[i].inUse = true;
			return RESULT_VALUE::OK;
		}
	}

	if (m_registry.size > m_registry.capacity)
	{
		return RESULT_VALUE::ALLOCATED_OBJECTS_EXCEEDED;
	}

	unsigned char* valuePtr = static_cast<unsigned char*>(m_buffer) + m_consumed;
	ptr = reinterpret_cast<void*>(valuePtr);

	// Register allocation
	m_registry.reg[m_registry.size++] = { .inUse = true, .capacity = amount, .offsetIntoBuffer = m_consumed};

	// Update consumed amount
	m_consumed += amount;

	return RESULT_VALUE::OK;
}

void Allocator::Free(void*& ptr)
{
	const size_t offsetIntoBuffer = reinterpret_cast<size_t>(ptr) - reinterpret_cast<size_t>(m_buffer);

	for (size_t i = 0; i < m_registry.size; i++)
	{
		if (m_registry.reg[i].offsetIntoBuffer == offsetIntoBuffer) // found it
		{
			m_registry.reg[i].inUse = false; // flag it as free to re-use
			break;
		}
	}
}
