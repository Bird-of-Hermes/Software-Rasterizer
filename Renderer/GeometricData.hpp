#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "Allocator.hpp"
#include "NaiveMath.hpp"
#include <filesystem>

struct Vertex
{
	Vec3f position;
	Vec3f normals;
	Vec2f uv;

	Vertex& operator*=(const Matrix4x4f& mat) noexcept
	{
		position = mat * position;
		return *this;
	}

	Vertex operator*(const Matrix4x4f& mat) const noexcept
	{
		return { mat * position, normals, uv };
	}
};

inline Vertex lerp(const Vertex& v1, const Vertex& v2, float t) noexcept
{
	return
	{
		lerp(v1.position, v2.position, t),
		normalize(lerp(v1.normals, v2.normals, t)),
		lerp(v1.uv, v2.uv, t)
	};
}

template <typename vertexType>
concept minVertex = requires(const Vertex& vert, const vertexType& v1, const vertexType & v2, float t)
{
	v1.position; // needs a .position member
	v1.uv;       // and .uv, for proper texturing
	{ lerp(v1, v2, t) } -> std::same_as<Vertex>; // needs an overload for lerp
	vertexType(vert); // needs constructor/copy for type Vertex
};

template <minVertex vertexType = Vertex>
struct Triangle
{
	vertexType a;
	vertexType b;
	vertexType c;

	Triangle<vertexType>& operator*=(const Matrix4x4f& mat) noexcept
	{
		a *= mat;
		b *= mat;
		c *= mat;

		return *this;
	}

	Triangle<vertexType> operator*(const Matrix4x4f& mat) const noexcept
	{
		Triangle<vertexType> out;
		out.a = a * mat;
		out.b = b * mat;
		out.c = c * mat;

		return out;
	}
};

template <minVertex vertexType = Vertex>
struct Mesh
{
	A::array<vertexType> vertices;
	A::array<uint32_t> indices;
};

#endif