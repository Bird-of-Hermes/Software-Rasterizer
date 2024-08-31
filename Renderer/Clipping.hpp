#ifndef CLIPPING_HPP
#define CLIPPING_HPP

#include "GeometricData.hpp"

// many thanks to OLC for making this resource freely available
// https://github.com/OneLoneCoder/Javidx9/tree/master/ConsoleGameEngine/BiggerProjects/Engine3D

template <minVertex vertexType = Vertex>
struct ClippedTriangle
{
	uint32_t num = 0;
	Triangle<vertexType> triangles[2];
};

inline float Vector_IntersectPlane(const Vec3f& plane_p, const Vec3f& plane_n, const Vec3f& lineStart, const Vec3f& lineEnd) noexcept
{
	// normalize plane_n before calling this
	const float plane_d = -dot(plane_n, plane_p);
	const float ad = dot(lineStart, plane_n);
	const float bd = dot(lineEnd, plane_n);

	return (-plane_d - ad) / (bd - ad);
}

template <minVertex vertexType = Vertex>
inline ClippedTriangle<vertexType> ClipAgainstPlane(const Vec3f& plane_p, const Vec3f& plane_n, const Triangle<vertexType>& in) noexcept
{
	// Make sure plane normal is indeed normal when calling this

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](const Vec3f& p) -> float
		{
			return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - dot(plane_n, plane_p));
		};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vertexType const* inside_points[3];
	vertexType const* outside_points[3];

	int nInsidePointCount = 0;
	int nOutsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	const float dA = dist(in.a.position);
	const float dB = dist(in.b.position);
	const float dC = dist(in.c.position);

	if (dA >= 0) 
	{ 
		inside_points[nInsidePointCount++] = &in.a;
	}
	else 
	{ 
		outside_points[nOutsidePointCount++] = &in.a;
	}
	if (dB >= 0) 
	{ 
		inside_points[nInsidePointCount++] = &in.b;
	}
	else
	{
		outside_points[nOutsidePointCount++] = &in.b;
	}
	if (dC >= 0) 
	{ 
		inside_points[nInsidePointCount++] = &in.c;
	}
	else 
	{ 
		outside_points[nOutsidePointCount++] = &in.c;
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required.

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return ClippedTriangle<vertexType>(0); // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through

		return ClippedTriangle<vertexType>(1, {in}); // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		Triangle<vertexType> out;

		out.a = *inside_points[0];

		float t = Vector_IntersectPlane(plane_p, plane_n, inside_points[0]->position, outside_points[0]->position);
		out.b = lerp(*inside_points[0], *outside_points[0], t);

		t = Vector_IntersectPlane(plane_p, plane_n, inside_points[0]->position, outside_points[1]->position);
		out.c = lerp(*inside_points[0], *outside_points[1], t);

		return ClippedTriangle<vertexType>(1, { out }); // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		Triangle out1, out2;

		out1.a = *inside_points[0];
		out1.b = *inside_points[1];

		float t = Vector_IntersectPlane(plane_p, plane_n, inside_points[0]->position, outside_points[0]->position);
		
		out1.c = lerp(*inside_points[0], *outside_points[0], t);

		out2.a = *inside_points[1];
		out2.b = out1.c;
		t = Vector_IntersectPlane(plane_p, plane_n, inside_points[1]->position, outside_points[0]->position);
		out2.c = lerp(*inside_points[1], *outside_points[0], t);

		return ClippedTriangle<vertexType>(2, { out1, out2 }); // Return two newly formed triangles which form a quad
	}

	return ClippedTriangle<vertexType>(0); // unreachable
}


#endif