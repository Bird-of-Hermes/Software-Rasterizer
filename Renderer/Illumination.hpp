#ifndef	LIGHTS_HPP
#define LIGHTS_HPP

#include "NaiveMath.hpp"
#include "Color.hpp"

// *unused*

enum class LightType  : uint8_t
{
    Directional = 0,
    Point,
    Spot
};

struct Light 
{
    LightType type = LightType::Directional;
    Color color;
    Vec3f position;
    Vec3f direction;
    float intensity = 1.0f;
    float range = 100.0f;
    float spotAngle = 30.0f;

    float constantAttenuation = 0;
    float linearAttenuation = 0;
    float quadraticAttenuation = 0;

    static Light MakeDirectional(const Vec3f& direction, Color color, float intensity) noexcept
    {
        Light light;
        light.type = LightType::Directional;
        light.direction = direction;
        light.color = color;
        light.intensity = intensity;

        return light;
    }
    static Light MakeSpot(const Vec3f& position, const Vec3f& direction, Color color, float intensity, float angle) noexcept
    {
        Light light;
        light.type = LightType::Spot;
        light.color = color;
        light.position = position;
        light.direction = direction;
        light.intensity = intensity;
        light.spotAngle = angle;

        return light;
    }
    static Light MakePoint(const Vec3f& position, Color color, float intensity, float range) noexcept
    {
        Light light;
        light.type = LightType::Point;
        light.position = position;
        light.color = color;
        light.intensity = intensity;
        light.range = range;

        return light;
    }
};

inline Light DirectionalLight(const Vec3f& direction, Color color, float intensity) noexcept
{
    return Light::MakeDirectional(direction, color, intensity);
}
inline Light PointLight(const Vec3f& position, Color color, float intensity, float range) noexcept
{
    return Light::MakePoint(position, color, intensity, range);
}
inline Light SpotLight(const Vec3f& position, const Vec3f& direction, Color color, float intensity, float angle) noexcept
{
    return Light::MakeSpot(position, direction, color, intensity, angle);
}

#endif