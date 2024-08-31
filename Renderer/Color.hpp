#ifndef COLOR_HPP
#define COLOR_HPP

#include <cmath>
#include <functional>
#include <algorithm>

struct Color
{
	constexpr Color() noexcept : blue(0), green(0), red(0) {}
	constexpr Color(uint8_t R, uint8_t G, uint8_t B) noexcept : red(R), green(G), blue(B){}
	Color(float r, float g, float b) noexcept
	{
		using namespace std;

		// create these, avoiding compiler optimizations
		r = abs(r); g = abs(g); b = abs(b);
		r = clamp(r, 0.0f, 1.0f);
		g = clamp(g, 0.0f, 1.0f);
		b = clamp(b, 0.0f, 1.0f);

		r *= 255.0f;
		g *= 255.0f;
		b *= 255.0f;

		blue  = static_cast<uint8_t>(b);  // same as floor()
		green = static_cast<uint8_t>(g); // same as floor()
		red   = static_cast<uint8_t>(r);   // same as floor()
	}

	uint8_t blue;
	uint8_t green;
	uint8_t red;

	Color& operator*=(float intensity) noexcept
	{
		using namespace std;

		intensity = abs(intensity);

		blue  = static_cast<uint8_t>(clamp((float)blue * intensity, 0.0f, 255.0f));
		green = static_cast<uint8_t>(clamp((float)green * intensity, 0.0f, 255.0f));
		red   = static_cast<uint8_t>(clamp((float)red * intensity, 0.0f, 255.0f));

		return *this;
	}
	Color operator*(float intensity) const noexcept
	{
		using namespace std;

		intensity = abs(intensity);

		const Color rgb = Color
		{
			static_cast<uint8_t>(clamp((float)red * intensity, 0.0f, 255.0f)),
			static_cast<uint8_t>(clamp((float)green * intensity, 0.0f, 255.0f)),
			static_cast<uint8_t>(clamp((float)blue * intensity, 0.0f, 255.0f))
		};

		return rgb;
	}
	Color& operator*=(const Color& other) noexcept
	{
		red   = static_cast<uint8_t>(((uint16_t)red * other.red) / 255);
		green = static_cast<uint8_t>(((uint16_t)green * other.green) / 255);
		blue  = static_cast<uint8_t>(((uint16_t)blue * other.blue) / 255);

		return *this;
	}
	Color operator*(const Color& other) const noexcept
	{
		return Color
		{
			static_cast<uint8_t>(((uint16_t)red * other.red) / 255),
			static_cast<uint8_t>(((uint16_t)green * other.green) / 255),
			static_cast<uint8_t>(((uint16_t)blue * other.blue) / 255)
		};
	}
	Color& operator+=(const Color& other) noexcept
	{
		using namespace std;

		blue  = static_cast<uint8_t>(clamp(blue + other.blue, 0, 255));
		green = static_cast<uint8_t>(clamp(green + other.green, 0, 255));
		red   = static_cast<uint8_t>(clamp(red + other.red, 0, 255));

		return *this;
	}
	Color operator+(const Color& other) const noexcept
	{
		using namespace std;

		return Color
		{
			static_cast<uint8_t>(clamp(red + other.red, 0, 255)),
			static_cast<uint8_t>(clamp(green + other.green, 0, 255)),
			static_cast<uint8_t>(clamp(blue + other.blue, 0, 255))
		};
	}
	Color& operator-=(const Color& other) noexcept
	{
		using namespace std;

		blue = static_cast<uint8_t>(clamp(blue - other.blue, 0, 255));
		green = static_cast<uint8_t>(clamp(green - other.green, 0, 255));
		red = static_cast<uint8_t>(clamp(red - other.red, 0, 255));

		return *this;
	}
	Color operator-(const Color& other) noexcept
	{
		using namespace std;

		return Color
		{
			static_cast<uint8_t>(clamp(red - other.red, 0, 255)),
			static_cast<uint8_t>(clamp(green - other.green, 0, 255)),
			static_cast<uint8_t>(clamp(blue - other.blue, 0, 255))
		};
	}
};

// *unused*
using ColorBlendingOP = std::function<Color(Color, Color)>;

#endif