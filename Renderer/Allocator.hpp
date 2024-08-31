#ifndef ALLOCATOR_HPP
#define ALLOCATOR_HPP

#include "ErrorEnum.hpp"
#include <atomic>

static constexpr size_t KB(size_t val) { return val * 1024; }
static constexpr size_t MB(size_t val) { return KB(val) * 1024; }
static constexpr size_t GB(size_t val) { return MB(val) * 1024; }

static constexpr size_t alignValue(size_t val, size_t alignment) noexcept
{
	return (val + alignment - 1) &~ (alignment - 1);
}

class Allocator
{
public:
	~Allocator();

	static RESULT_VALUE Init(size_t bufferSize, size_t alignment = 64, size_t maxElements = 4096);
	static void Resize(size_t newSize, size_t alignment = 64);
	static RESULT_VALUE Allocate(void*& ptr, size_t bytes = 0);
	static void Free(void*& ptr);

private:
	struct AddressRegistry
	{
		struct Register
		{
			bool inUse = true;
			size_t capacity = 0;
			size_t offsetIntoBuffer = 0;
		};
		inline static std::atomic<size_t> capacity = 0;
		inline static std::atomic<size_t> size = 0;
		inline static Register* reg = nullptr;
	};

	inline static void* m_buffer = nullptr;
	inline static size_t m_alignment = 0;
	inline static std::atomic<size_t> m_allocated = 0;
	inline static std::atomic<size_t> m_consumed = 0;

	inline static AddressRegistry m_registry;
};

namespace A
{
	// to construct an object, call: object.make_array(object, arraySize) or A::array<Type>::make_array(object, arraySize)
	template <typename T>
	struct array
	{
		array() {};
		~array()
		{
			destroy();
		}

		static bool make_array(array& storage, size_t N)
		{
			const RESULT_VALUE value = Allocator::Allocate(reinterpret_cast<void*&>(storage), sizeof(T) * N);
			if (value != RESULT_VALUE::OK)
			{
				logResult(value);
				return false;
			}
			else
			{
				storage._capacity = N;
				return true;
			}
		}

		void clear() noexcept
		{
			_size = 0;
		}

		void destroy() noexcept
		{
			if (_data)
			{
				Allocator::Free(reinterpret_cast<void*&>(_data));
				_size = 0;
				_capacity = 0;
				_data = nullptr;
			}
		}

		bool emplace_back(const T& data) noexcept
		{
			if (_size < _capacity)
			{
				_data[_size++] = data;
				return true;
			}
			else
			{
				return false;
			}
		}

		constexpr T& operator[](size_t index) noexcept
		{
			return _data[index];
		}

		constexpr T& operator[](size_t index) const noexcept
		{
			return _data[index];
		}

		T& at(size_t index) 
		{
			if (index >= _size) 
			{
				throw std::out_of_range("Index out of range");
			}
			return _data[index];
		}

		const T& at(size_t index) const 
		{
			if (index >= _size) 
			{
				throw std::out_of_range("Index out of range");
			}
			return _data[index];
		}

		size_t size() const noexcept
		{
			return _size;
		}

		size_t capacity() const noexcept
		{
			return _capacity;
		}

	private:
		T* _data = nullptr;
		size_t _capacity = 0;
		size_t _size = 0;
	};
};

#endif