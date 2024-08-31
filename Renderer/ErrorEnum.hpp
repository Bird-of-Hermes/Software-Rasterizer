#ifndef ERROR_ENUM_HPP
#define ERROR_ENUM_HPP

#include <iostream>
#include <utility>
#include <cassert>

// treat errors as values

enum class RESULT_VALUE : uint8_t
{
	OK = 0,
	STB_ERROR,
	GENERIC_ERROR,
	ALLOCATOR_NOT_INITIALIZED,
	ALLOCATOR_ALREADY_INITIALIZED,
	INVALID_POINTER,
	DOUBLE_FREE,
	REQUESTED_AMOUNT_IS_ZERO,
	REQUESTED_AMOUNT_EXCEEDS_AVAILABLE_MEMORY,
	ALLOCATED_OBJECTS_EXCEEDED,
	MISSING_FILEPATH,
	ASSIMP_FAILURE,
};

static constexpr const char* RV_MSG[] = 
{
	"",
	"STB_ERROR. Failed to load a file through the STB lib, check the console output for more details",
	"GENERIC_ERROR. Couldn't determine the cause",
	"ALLOCATOR_NOT_INITIALIZED. Tried to allocate but the Allocator was not initialized, make sure to call Allocator::Init()",
	"ALLOCATOR_ALREADY_INITIALIZED. Tried to call Allocator::Init() but there's already a valid instance of the Allocator",
	"INVALID_POINTER. Should've been nullptr but wasn't",
	"DOUBLE_FREE. Called Free() on a already freed object, this won't crash but pay attention to possible bugs",
	"REQUESTED_AMOUNT_IS_ZERO. Check Allocator::Allocate() 2nd parameter",
	"REQUESTED_AMOUNT_EXCEEDS_AVAILABLE_MEMORY. Check Application::Start() 4th parameter or Allocator::Init() 1st parameter",
	"ALLOCATED_OBJECTS_EXCEEDED. Check Allocator::Init() 3rd parameter",
	"MISSING_FILEPATH. Check if the path to a file really exists",
	"ASSIMP_FAILURE. Failed to load data from ASSIMP, model wasn't loaded",
};

static constexpr const char* Error(RESULT_VALUE val) noexcept
{
	return RV_MSG[std::to_underlying(val)];
}

inline constexpr void logResult(RESULT_VALUE val) noexcept
{
	if (val != RESULT_VALUE::OK)
	{
		std::cerr << "Error: " <<  Error(val) << std::endl;
	}
}

#endif