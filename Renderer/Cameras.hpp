#ifndef CAMERAS_HPP
#define CAMERAS_HPP

#include "GeometricData.hpp"
#include "Quaternion.hpp"

class Camera
{
	friend class Application;

public:
	void SetTarget(Vec3f* ptr = nullptr) const noexcept
	{
		target = ptr;
	}

	Matrix4x4f UpdateViewMatrix() noexcept
	{
		Vec3f _lookingAt = { 0, 0, 1 };

		if (target)
		{
			_lookingAt = normalize(*target - position);
		}

		rotation.x = std::clamp(rotation.x, -89.9f, 89.9f);
		rotation.y = std::fmod(rotation.y, 360.0f);
		rotation.z = std::fmod(rotation.z, 360.0f);

		const Quaternion q = Quaternion::fromEuler(rotation.x, rotation.y, rotation.z);
		rotationCached = q.toMatrix4x4f();

		_lookingAt = rotationCached * _lookingAt;
		_lookingAt += position;

		lastCameraMatrix = PointAt(position, _lookingAt, { 0, 1, 0 }).Invert(); // pointAt.Invert() -> viewAt

		return lastCameraMatrix;
	}

	Vec3f towards(const Vec3f& location) const noexcept
	{
		return rotationCached * normalize(location);
	}

	Vec3f forward() const noexcept
	{
		if (target)
		{
			return rotationCached * normalize(*target - position);
		}
		else
		{
			return rotationCached * Vec3f(0, 0, 1);
		}
	}

	Vec3f up() const noexcept
	{
		return rotationCached * Vec3f(0, 1, 0);
	}

	Vec3f left() const noexcept
	{
		if (target)
		{
			return cross(up(), normalize(*target - position));
		}
		else
		{
			return rotationCached * Vec3f(-1, 0, 0);
		}
	}

	Vec3f position;
	Vec3f rotation;
	Vec3f scale = {1.0f, 1.0f, 1.0f};

	struct
	{
		float fieldOfView = 90.0f;
		float nearPlane = 0.1f;
		float farPlane = 1000.0f;
	} projection;

private:
	mutable Vec3f const* target = nullptr;
	mutable Matrix4x4f lastCameraMatrix = Matrix4x4f::Identity();
	mutable Matrix4x4f rotationCached = Matrix4x4f::Identity();
};

#endif