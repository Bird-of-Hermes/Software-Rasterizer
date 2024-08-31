#ifndef IMAGES_HPP
#define IMAGES_HPP

#include "Allocator.hpp"
#include "NaiveMath.hpp"
#include "Color.hpp"
#include <filesystem>

struct Image
{
public:
	friend class Application;
	Image() {};
	Image(std::filesystem::path path);
	~Image();

	Color pixel(size_t x, size_t y) const noexcept;
	Color sample(float u, float v) const noexcept;
	[[nodiscard]] RESULT_VALUE LoadFromFile(std::filesystem::path path);

	Color* pixelGrid = nullptr;
	int32_t width = 0;
	int32_t height = 0;
	int32_t channels = 0;
};

#endif