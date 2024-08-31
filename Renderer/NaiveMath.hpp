#ifndef NAIVE_MATH_HPP
#define NAIVE_MATH_HPP

#pragma warning(push)
#pragma warning(disable: 4201)

#include <math.h>
#include <iostream>
#include <immintrin.h>
#include "SinCosTable.hpp"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

struct Vec2f
{
	constexpr Vec2f() noexcept : x(0), y(0) {}
	constexpr Vec2f(const Vec2f& other) noexcept : x(other.x), y(other.y) {}
	constexpr Vec2f(float X, float Y) noexcept : x(X), y(Y) {}

	float x;
	float y;

	inline Vec2f operator*(const Vec2f& v2) const noexcept
	{
		return { x * v2.x, y * v2.y };
	}
	inline Vec2f operator+(const Vec2f& v2) const noexcept
	{
		return { x + v2.x, y + v2.y };
	}
	inline Vec2f operator-(const Vec2f& v2) const noexcept
	{
		return { x - v2.x, y - v2.y };
	}

	inline Vec2f operator-() const noexcept
	{
		return { -x, -y };
	}
	inline Vec2f operator*(float t) const noexcept
	{
		return { x * t, y * t};
	}
	inline Vec2f operator/(float t) const noexcept
	{
		return { x / t, y / t};
	}

	inline Vec2f& operator+= (const Vec2f& v2) noexcept
	{
		x += v2.x;
		y += v2.y;
		return *this;
	}
	inline Vec2f& operator-= (const Vec2f& v2) noexcept
	{
		x -= v2.x;
		y -= v2.y;
		return *this;
	}
	inline Vec2f& operator*= (const Vec2f& v2) noexcept
	{
		x *= v2.x;
		y *= v2.y;
		return *this;
	}
	inline Vec2f& operator/= (const Vec2f& v2) noexcept
	{
		x /= v2.x;
		y /= v2.y;
		return *this;
	}
	inline Vec2f& operator*=(float t) noexcept
	{
		x *= t;
		y *= t;
		return *this;
	}
	inline Vec2f& operator/=(float t) noexcept
	{
		t = 1.0f / t;
		x *= t;
		y *= t;
		return *this;
	}

	friend Vec2f operator*(float t, const Vec2f& v) noexcept
	{
		return { v.x * t, v.y * t};
	}
	friend Vec2f operator/=(float t, const Vec2f& v) noexcept
	{
		return { v.x / t, v.y / t};
	}

	inline float length() const noexcept
	{
		return sqrtf(x * x + y * y);
	}
	inline float squared_length() const noexcept
	{
		return x * x + y * y;
	}
	inline void make_normalized() noexcept
	{
		const float k = 1.0f / sqrtf(x * x + y * y);
		x *= k;
		y *= k;
	}

	friend std::istream& operator>>(std::istream& is, Vec2f& t)
	{
		is >> t.x >> t.y;
		return is;
	}
	friend std::ostream& operator<<(std::ostream& os, const Vec2f& t)
	{
		os << "[X], [Y]: [" << t.x << "], [" << t.y << "]";
		return os;
	}
	friend std::wostream& operator<<(std::wostream& os, const Vec2f& t)
	{
		os << L"[X], [Y]: [" << t.x << L"], [" << t.y << L"]";
		return os;
	}
};

struct Vec3f
{
	constexpr Vec3f() noexcept : x(0), y(0), z(0) {}
	constexpr Vec3f(const Vec3f& other) noexcept : x(other.x), y(other.y), z(other.z){}
	constexpr Vec3f(float X, float Y, float Z) noexcept : x(X), y(Y), z(Z) {}

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};
		struct
		{
			float r;
			float g;
			float b;
		};
		struct
		{
			float u;
			float v;
			float w;
		};
		float e[3];
	};

	inline float operator[] (int i) const noexcept 
	{ 
		return e[i]; 
	}
	inline float& operator[] (int i) noexcept
	{
		return e[i];
	}
	
	inline Vec3f operator*(const Vec3f& v2) const noexcept
	{
		return { x * v2.x, y * v2.y, z * v2.z };
	}
	inline Vec3f operator+(const Vec3f& v2) const noexcept
	{
		return { x + v2.x, y + v2.y, z + v2.z };
	}
	inline Vec3f operator-(const Vec3f& v2) const noexcept
	{
		return { x - v2.x, y - v2.y, z - v2.z };
	}

	inline Vec3f operator-() const noexcept 
	{ 
		return { -x, -y, -z };
	}
	inline Vec3f operator*(float t) const noexcept
	{
		return { x * t, y * t, z * t };
	}
	inline Vec3f operator/(float t) const noexcept
	{
		return { x / t, y / t, z / t };
	}

	inline Vec3f& operator+= (const Vec3f& v2) noexcept
	{
		x += v2.x;
		y += v2.y;
		z += v2.z;
		return *this;
	}
	inline Vec3f& operator-= (const Vec3f& v2) noexcept
	{
		x -= v2.x;
		y -= v2.y;
		z -= v2.z;
		return *this;
	}
	inline Vec3f& operator*= (const Vec3f& v2) noexcept
	{
		x *= v2.x;
		y *= v2.y;
		z *= v2.z;
		return *this;
	}
	inline Vec3f& operator/= (const Vec3f& v2) noexcept
	{
		x /= v2.x;
		y /= v2.y;
		z /= v2.z;
		return *this;
	}
	inline Vec3f& operator*=(float t) noexcept
	{
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}
	inline Vec3f& operator/=(float t) noexcept
	{
		t = 1.0f / t;
		x *= t;
		y *= t;
		z *= t;
		return *this;
	}

	friend Vec3f operator*(float t, const Vec3f& v) noexcept
	{
		return { v.x * t, v.y * t, v.z * t };
	}
	friend Vec3f operator/=(float t, const Vec3f& v) noexcept
	{
		return { v.x / t, v.y / t, v.z/t };
	}

	inline float length() const noexcept
	{
		return sqrtf(x * x + y * y + z * z);
	}
	inline float squared_length() const noexcept
	{
		return x * x + y * y + z * z;
	}
	inline void make_normalized() noexcept
	{
		const float k = 1.0f / sqrtf(x * x + y * y + z * z);
		x *= k;
		y *= k;
		z *= k;
	}

	friend std::istream& operator>>(std::istream& is, Vec3f& t)
	{
		is >> t.x >> t.y >> t.z;
		return is;
	}
	friend std::ostream& operator<<(std::ostream& os, const Vec3f& t)
	{
		os << "[X], [Y], [Z]: [" << t.x << "], [" << t.y << "], [" << t.z << "]";
		return os;
	}
	friend std::wostream& operator<<(std::wostream& os, const Vec3f& t)
	{
		os << L"[X], [Y], [Z]: [" << t.x << L"], [" << t.y << L"], [" << t.z << L"]";
		return os;
	}
};

struct Vec4f
{
	float x;
	float y;
	float z;
	float w;
};

inline Vec2f lerp(const Vec2f& v1, const Vec2f& v2, float t) noexcept
{
	return v1 + t * (v2 - v1);
}
inline Vec3f lerp(const Vec3f& v1, const Vec3f& v2, float t) noexcept
{
	return v1 + t * (v2 - v1);
}
inline Vec3f normalize(const Vec3f& v) noexcept
{
	return v / v.length();
}
inline float dot(const Vec3f& v1, const Vec3f& v2) noexcept
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
inline Vec3f cross(const Vec3f& v1, const Vec3f& v2) noexcept
{
	return { (v1.y * v2.z - v1.z * v2.y), -(v1.x * v2.z - v1.z * v2.x), v1.x * v2.y - v1.y * v2.x };
}
inline Vec3f reflect(const Vec3f& v, const Vec3f& n) noexcept
{
	return v - 2.0f * dot(v, n) * n;
}
inline bool refract(const Vec3f& incidentRay, const Vec3f& normal, float refractiveIndexRatio, Vec3f& refractedRay) noexcept
{
	const Vec3f unitIncidentRay = normalize(incidentRay);
	const float dotIRN = dot(unitIncidentRay, normal);
	const float discriminant = 1.0f - refractiveIndexRatio * refractiveIndexRatio * (1.0f - dotIRN * dotIRN);

	if (discriminant > 0)
	{
		refractedRay = refractiveIndexRatio * (unitIncidentRay - normal * dotIRN) - normal * sqrtf(discriminant);
		return true;
	}
	else
	{
		return false;
	}
}
inline float schlick(float cosine, float refractionIndex) noexcept
{
	float r0 = (1.0f - refractionIndex) / (1.0f + refractionIndex);
	r0 *= r0;

	return r0 + (1.0f - r0) * powf(1.0f - cosine, 5);
}
constexpr float toRadians(float degrees) noexcept
{
	return (3.141592653f / 180.0f) * degrees;
}

struct Matrix4x4f
{
	static constexpr Matrix4x4f Identity() noexcept
	{
		return 
		{
			1.0f, 0, 0, 0,
			0, 1.0f, 0, 0,
			0, 0, 1.0f, 0,
			0, 0, 0, 1.0f
		};
	}

	union
	{
		float m[16];

		// row, column
		float rc[4][4];
	};

	Vec3f operator*(const Vec3f& in) const noexcept
	{
		Vec3f out;

		out.x = in.x * rc[0][0] + in.y * rc[1][0] + in.z * rc[2][0] + rc[3][0];
		out.y = in.x * rc[0][1] + in.y * rc[1][1] + in.z * rc[2][1] + rc[3][1];
		out.z = in.x * rc[0][2] + in.y * rc[1][2] + in.z * rc[2][2] + rc[3][2];
		float w = in.x * rc[0][3] + in.y * rc[1][3] + in.z * rc[2][3] + rc[3][3];

		if (w != 0.0f) [[likely]]
		{
			out.x /= w; out.y /= w; out.z /= w;
		}

		return out;
	}

	Vec4f operator*(const Vec4f& in) const noexcept
	{
		Vec4f out;

		out.x = in.x * rc[0][0] + in.y * rc[1][0] + in.z * rc[2][0] + rc[3][0];
		out.y = in.x * rc[0][1] + in.y * rc[1][1] + in.z * rc[2][1] + rc[3][1];
		out.z = in.x * rc[0][2] + in.y * rc[1][2] + in.z * rc[2][2] + rc[3][2];
		out.w = in.x * rc[0][3] + in.y * rc[1][3] + in.z * rc[2][3] + rc[3][3];

		if (out.w != 0.0f) [[likely]]
		{
			out.x /= out.w; out.y /= out.w; out.z /= out.w;
		}

		return out;
	}

	constexpr float at(int row, int column) const noexcept
	{
		return rc[row][column];
	}

	constexpr Matrix4x4f operator*(const Matrix4x4f& other) const noexcept
	{
		Matrix4x4f out;
		for (int c = 0; c < 4; c++)
		{
			for (int r = 0; r < 4; r++)
			{
				out.rc[r][c] = rc[r][0] * other.rc[0][c] + rc[r][1] * other.rc[1][c] + rc[r][2] * other.rc[2][c] + rc[r][3] * other.rc[3][c];
			}
		}
		return out;
	}

	inline const Matrix4x4f Transposed() const noexcept
	{
		__m128 row0 = _mm_set_ps(rc[0][3], rc[0][2], rc[0][1], rc[0][0]);
		__m128 row1 = _mm_set_ps(rc[1][3], rc[1][2], rc[1][1], rc[1][0]);
		__m128 row2 = _mm_set_ps(rc[2][3], rc[2][2], rc[2][1], rc[2][0]);
		__m128 row3 = _mm_set_ps(rc[3][3], rc[3][2], rc[3][1], rc[3][0]);

		// transpose
		_MM_TRANSPOSE4_PS(row0, row1, row2, row3);

		// load
		const __m256 temp1 = _mm256_insertf128_ps(_mm256_castps128_ps256(row0), row1, 1);
		const __m256 temp2 = _mm256_insertf128_ps(_mm256_castps128_ps256(row2), row3, 1);

		Matrix4x4f out;
		memcpy(&out, &temp1, sizeof(__m256));
		memcpy(&out.m[8], &temp2, sizeof(__m256));

		return out;
	}

	inline const Matrix4x4f Invert() const noexcept
	{
		// Transpose elements
		const Matrix4x4f TMat = Transposed();

		const __m128 row0 = _mm_set_ps(TMat.rc[0][3], TMat.rc[0][2], TMat.rc[0][1], TMat.rc[0][0]);
		const __m128 row1 = _mm_set_ps(TMat.rc[1][3], TMat.rc[1][2], TMat.rc[1][1], TMat.rc[1][0]);
		const __m128 row2 = _mm_set_ps(TMat.rc[2][3], TMat.rc[2][2], TMat.rc[2][1], TMat.rc[2][0]);
		const __m128 row3 = _mm_set_ps(TMat.rc[3][3], TMat.rc[3][2], TMat.rc[3][1], TMat.rc[3][0]);

		__m128 V00 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 1, 0, 0));
		__m128 V10 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(3, 2, 3, 2));
		__m128 V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 1, 0, 0));
		__m128 V11 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 2, 3, 2));
		__m128 V02 = _mm_shuffle_ps(row2, row0, _MM_SHUFFLE(2, 0, 2, 0));
		__m128 V12 = _mm_shuffle_ps(row3, row1, _MM_SHUFFLE(3, 1, 3, 1));

		__m128 D0 = _mm_mul_ps(V00, V10);
		__m128 D1 = _mm_mul_ps(V01, V11);
		__m128 D2 = _mm_mul_ps(V02, V12);

		V00 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 2, 3, 2));
		V10 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(1, 1, 0, 0));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 2, 3, 2));
		V11 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 1, 0, 0));
		V02 = _mm_shuffle_ps(row2, row0, _MM_SHUFFLE(3, 1, 3, 1));
		V12 = _mm_shuffle_ps(row3, row1, _MM_SHUFFLE(2, 0, 2, 0));

		D0 = _mm_fnmadd_ps(V00, V10, D0);
		D1 = _mm_fnmadd_ps(V01, V11, D1);
		D2 = _mm_fnmadd_ps(V02, V12, D2);

		// V11 = D0Y,D0W,D2Y,D2Y
		V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 1, 3, 1));
		V00 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 0, 2, 1));
		V10 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(0, 3, 0, 2));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(0, 1, 0, 2));
		V11 = _mm_shuffle_ps(V11, D0, _MM_SHUFFLE(2, 1, 2, 1));

		// V13 = D1Y,D1W,D2W,D2W
		__m128 V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 3, 3, 1));
		V02 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(1, 0, 2, 1));
		__m128 V03 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(0, 1, 0, 2));
		V12 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(0, 3, 0, 2));
		V13 = _mm_shuffle_ps(V13, D1, _MM_SHUFFLE(2, 1, 2, 1));

		__m128 C0 = _mm_mul_ps(V00, V10);
		__m128 C2 = _mm_mul_ps(V01, V11);
		__m128 C4 = _mm_mul_ps(V02, V12);
		__m128 C6 = _mm_mul_ps(V03, V13);

		// V11 = D0X,D0Y,D2X,D2X
		V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(0, 0, 1, 0));
		V00 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(2, 1, 3, 2));
		V10 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(2, 1, 0, 3));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 3, 2, 3));
		V11 = _mm_shuffle_ps(D0, V11, _MM_SHUFFLE(0, 2, 1, 2));

		// V13 = D1X,D1Y,D2Z,D2Z
		V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(2, 2, 1, 0));
		V02 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(2, 1, 3, 2));
		V12 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(2, 1, 0, 3));
		V03 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(1, 3, 2, 3));
		V13 = _mm_shuffle_ps(D1, V13, _MM_SHUFFLE(0, 2, 1, 2));

		C0 = _mm_fnmadd_ps(V00, V10, C0);
		C2 = _mm_fnmadd_ps(V01, V11, C2);
		C4 = _mm_fnmadd_ps(V02, V12, C4);
		C6 = _mm_fnmadd_ps(V03, V13, C6);

		V00 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(0, 3, 0, 3));
		// V10 = D0Z,D0Z,D2X,D2Y
		V10 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 2, 2));
		V10 = _mm_shuffle_ps(V10, V10, _MM_SHUFFLE(0, 2, 3, 0));
		V01 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(2, 0, 3, 1));
		// V11 = D0X,D0W,D2X,D2Y
		V11 = _mm_shuffle_ps(D0, D2, _MM_SHUFFLE(1, 0, 3, 0));
		V11 = _mm_shuffle_ps(V11, V11, _MM_SHUFFLE(2, 1, 0, 3));
		V02 = _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(0, 3, 0, 3));
		// V12 = D1Z,D1Z,D2Z,D2W
		V12 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 2, 2));
		V12 = _mm_shuffle_ps(V12, V12, _MM_SHUFFLE(0, 2, 3, 0));
		V03 = _mm_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 0, 3, 1));
		// V13 = D1X,D1W,D2Z,D2W
		V13 = _mm_shuffle_ps(D1, D2, _MM_SHUFFLE(3, 2, 3, 0));
		V13 = _mm_shuffle_ps(V13, V13, _MM_SHUFFLE(2, 1, 0, 3));

		V00 = _mm_mul_ps(V00, V10);
		V01 = _mm_mul_ps(V01, V11);
		V02 = _mm_mul_ps(V02, V12);
		V03 = _mm_mul_ps(V03, V13);
		__m128 C1 = _mm_sub_ps(C0, V00);
		C0 = _mm_add_ps(C0, V00);
		__m128 C3 = _mm_add_ps(C2, V01);
		C2 = _mm_sub_ps(C2, V01);
		__m128 C5 = _mm_sub_ps(C4, V02);
		C4 = _mm_add_ps(C4, V02);
		__m128 C7 = _mm_add_ps(C6, V03);
		C6 = _mm_sub_ps(C6, V03);

		C0 = _mm_shuffle_ps(C0, C1, _MM_SHUFFLE(3, 1, 2, 0));
		C2 = _mm_shuffle_ps(C2, C3, _MM_SHUFFLE(3, 1, 2, 0));
		C4 = _mm_shuffle_ps(C4, C5, _MM_SHUFFLE(3, 1, 2, 0));
		C6 = _mm_shuffle_ps(C6, C7, _MM_SHUFFLE(3, 1, 2, 0));
		C0 = _mm_shuffle_ps(C0, C0, _MM_SHUFFLE(3, 1, 2, 0));
		C2 = _mm_shuffle_ps(C2, C2, _MM_SHUFFLE(3, 1, 2, 0));
		C4 = _mm_shuffle_ps(C4, C4, _MM_SHUFFLE(3, 1, 2, 0));
		C6 = _mm_shuffle_ps(C6, C6, _MM_SHUFFLE(3, 1, 2, 0));

		// Get the determinant
		static const __m128 one = _mm_set_ps1(1.0f);
		__m128 vTemp = _mm_dp_ps(C0, row0, 0xff); // cofactors
		vTemp = _mm_div_ps(one, vTemp);

		__m128 r0 = _mm_mul_ps(C0, vTemp);
		__m128 r1 = _mm_mul_ps(C2, vTemp);
		__m128 r2 = _mm_mul_ps(C4, vTemp);
		__m128 r3 = _mm_mul_ps(C6, vTemp);

		Matrix4x4f out;
		memcpy(&out.rc[0][0], &r0, sizeof(__m128));
		memcpy(&out.rc[1][0], &r1, sizeof(__m128));
		memcpy(&out.rc[2][0], &r2, sizeof(__m128));
		memcpy(&out.rc[3][0], &r3, sizeof(__m128));

		return out;
	}

	friend std::ostream& operator<<(std::ostream& os, const Matrix4x4f& t)
	{
		os << t.rc[0][0] << ", " << t.rc[0][1] << ", " << t.rc[0][2] << ", " << t.rc[0][3] << '\n';
		os << t.rc[1][0] << ", " << t.rc[1][1] << ", " << t.rc[1][2] << ", " << t.rc[1][3] << '\n';
		os << t.rc[2][0] << ", " << t.rc[2][1] << ", " << t.rc[2][2] << ", " << t.rc[2][3] << '\n';
		os << t.rc[3][0] << ", " << t.rc[3][1] << ", " << t.rc[3][2] << ", " << t.rc[3][3] << '\n';

		return os;
	}
	friend std::wostream& operator<<(std::wostream& os, const Matrix4x4f& t)
	{
		os << t.rc[0][0] << L", " << t.rc[0][1] << L", " << t.rc[0][2] << L", " << t.rc[0][3] << '\n';
		os << t.rc[1][0] << L", " << t.rc[1][1] << L", " << t.rc[1][2] << L", " << t.rc[1][3] << '\n';
		os << t.rc[2][0] << L", " << t.rc[2][1] << L", " << t.rc[2][2] << L", " << t.rc[2][3] << '\n';
		os << t.rc[3][0] << L", " << t.rc[3][1] << L", " << t.rc[3][2] << L", " << t.rc[3][3] << '\n';

		return os;
	}

};

inline Matrix4x4f ProjectionMatrix(uint16_t screenWidth = 800, uint16_t screenHeight = 600, float FOV = 90.0f, float Near = 0.1f, float Far = 1000.0f) noexcept
{
	const float aspectRatio = (float)screenHeight / (float)screenWidth;
	const float fovRad = 1.0f / tanf(FOV * 0.5f / 180.0f * 3.14159f);

	Matrix4x4f out{};
	out.rc[0][0] = aspectRatio * fovRad;
	out.rc[1][1] = fovRad;
	out.rc[2][2] = Far / (Far - Near);
	out.rc[3][2] = (-Far * Near) / (Far - Near);
	out.rc[2][3] = 1.0f;
	out.rc[3][3] = 0.0f;

	return out;
}
inline Matrix4x4f PointAt(const Vec3f& pos, const Vec3f& target, const Vec3f& up) noexcept
{
	const Vec3f newForward = normalize(target - pos);
	const Vec3f newUp = normalize(up - (newForward * dot(up, newForward)));
	const Vec3f newRight = cross(newUp, newForward);

	// Dimensioning && Translation Matrix
	Matrix4x4f out;
	out.rc[0][0] = newRight.x;		out.rc[0][1] = newRight.y;		out.rc[0][2] = newRight.z;		out.rc[0][3] = 0.0f;
	out.rc[1][0] = newUp.x;			out.rc[1][1] = newUp.y;			out.rc[1][2] = newUp.z;			out.rc[1][3] = 0.0f;
	out.rc[2][0] = newForward.x;	out.rc[2][1] = newForward.y;	out.rc[2][2] = newForward.z;	out.rc[2][3] = 0.0f;
	out.rc[3][0] = pos.x;			out.rc[3][1] = pos.y;			out.rc[3][2] = pos.z;			out.rc[3][3] = 1.0f;

	return out;
}

inline Matrix4x4f RotateX(float angle) noexcept
{
	const float cosa = lookupCosine(angle);
	const float sina = lookupSine(angle);

	Matrix4x4f out = Matrix4x4f::Identity();
	out.rc[0][0] = 1.0f;
	out.rc[1][1] = cosa;
	out.rc[1][2] = sina;
	out.rc[2][1] = -sina;
	out.rc[2][2] = cosa;
	out.rc[3][3] = 1.0f;

	return out;
}
inline Matrix4x4f RotateY(float angle) noexcept
{
	const float cosa = lookupCosine(angle);
	const float sina = lookupSine(angle);

	Matrix4x4f out = Matrix4x4f::Identity();
	out.rc[0][0] = cosa;
	out.rc[0][2] = sina;
	out.rc[2][0] = -sina;
	out.rc[1][1] = 1.0f;
	out.rc[2][2] = cosa;
	out.rc[3][3] = 1.0f;

	return out;
}
inline Matrix4x4f RotateZ(float angle) noexcept
{
	const float cosa = lookupCosine(angle);
	const float sina = lookupSine(angle);

	Matrix4x4f out = Matrix4x4f::Identity();
	out.rc[0][0] = cosa;
	out.rc[0][1] = sina;
	out.rc[1][0] = -sina;
	out.rc[1][1] = cosa;
	out.rc[2][2] = 1.0f;
	out.rc[3][3] = 1.0f;
	return out;
}
inline Matrix4x4f Rotate(float Z_yaw, float Y_pitch, float X_roll)
{
	// multiply z * y * x

	const float sinAlpha = lookupSine(Z_yaw);
	const float sinBeta  = lookupSine(Y_pitch);
	const float sinGamma = lookupSine(X_roll);
	const float cosAlpha = lookupCosine(Z_yaw);
	const float cosBeta  = lookupCosine(Y_pitch);
	const float cosGamma = lookupCosine(X_roll);

	return 
	{
		cosBeta*cosGamma, -(sinAlpha*sinBeta*cosGamma - cosAlpha*sinGamma), cosAlpha*sinBeta*cosGamma + sinAlpha*sinGamma, 0.0f,
		-(cosBeta*sinGamma), sinAlpha*sinBeta*sinGamma + cosAlpha*cosGamma, -(cosAlpha*sinBeta*sinGamma - sinAlpha*cosGamma), 0.0f,
		-sinBeta, -(sinAlpha*cosBeta), cosAlpha*cosBeta, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
}
inline Matrix4x4f Translate(float x, float y, float z)
{
	return
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1
	};
}
inline Matrix4x4f Scale(float x, float y, float z) noexcept
{
	return 
	{
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
}
inline Matrix4x4f SRT(const Matrix4x4f& scale, const Matrix4x4f& rotation, const Matrix4x4f& translate) noexcept
{
	return scale * rotation * translate;
}
inline Matrix4x4f ViewPortMatrix(size_t ViewPortWidth, size_t ViewPortHeight) noexcept
{
	const float wD2 = static_cast<float>(ViewPortWidth / 2);
	const float hD2 = static_cast<float>(ViewPortHeight / 2);

	return
	{
		wD2, 0, 0, 0,
		0, -hD2, 0, 0,
		0, 0, 1.0f, 0,
		wD2, hD2, 0, 1.0f
	};
}

using vec3 = Vec3f;
#pragma warning(pop)
#endif