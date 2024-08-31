#include "Images.hpp"
#include <utility>

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(push)
#pragma warning(disable: 4996)
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include <stb_image.h>
#include <stb_image_write.h>
#pragma warning(pop)
#pragma warning(pop)

Image::Image(std::filesystem::path path)
{
	logResult(LoadFromFile(path));
}

Image::~Image()
{
	Allocator::Free(reinterpret_cast<void*&>(pixelGrid));
}

RESULT_VALUE Image::LoadFromFile(std::filesystem::path path)
{
	unsigned char* _stbi_image = stbi_load(path.string().c_str(), &width, &height, &channels, sizeof(Color));
	if (_stbi_image)
	{
		const size_t imgSize = (size_t)width * height * sizeof(Color);
		const RESULT_VALUE val = Allocator::Allocate(reinterpret_cast<void*&>(pixelGrid), imgSize);

		if (val == RESULT_VALUE::OK)
		{
			memcpy(pixelGrid, _stbi_image, imgSize);
			stbi_image_free(_stbi_image);

			const size_t numPixels = imgSize / sizeof(Color);

			// inverse img RGB -> BGR, that's how GDI expects the pixels to be ordered
			for (size_t i = 0; i < numPixels; i++)
			{
				std::swap(pixelGrid[i].red, pixelGrid[i].blue);
			}
		}
		else
		{
			return val;
		}
	}
	else
	{
		std::cerr << stbi_failure_reason() << std::endl;
		return RESULT_VALUE::STB_ERROR;
	}

	return RESULT_VALUE::OK;
}

Color Image::pixel(size_t x, size_t y) const noexcept
{
	if (x >= width || y >= height)
	{
		return Color{ (unsigned char)200, 100, 100 }; // pink, indicates the call was malformed
	}
	return pixelGrid[y * width + x];
}

Color Image::sample(float u, float v) const noexcept
{
	if (width == 0 || u < 0.0f || u > 1.0f || v < 0.0f || v > 1.0f)
	{
		return Color{ (unsigned char)0, 0, 0 }; // there's nothing to sample
	}

	const size_t x = static_cast<size_t>(u * (width - 1));
	const size_t y = static_cast<size_t>(v * (height - 1));

	return pixelGrid[y * width + x];
}
