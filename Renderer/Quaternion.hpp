#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "NaiveMath.hpp"

struct Quaternion 
{
    float x;
    float y;
    float z;
    float w;

    Quaternion RotateX(float angleDegrees) const noexcept
    {
        float angleRadians = toRadians(angleDegrees);
        float halfAngle = angleRadians * 0.5f;
        float s = sin(halfAngle);
        float c = cos(halfAngle);

        Quaternion q;
        q.x = c * x + s * w;
        q.y = c * y + s * z;
        q.z = c * z - s * y;
        q.w = c * w - s * x;
        return q;
    }
    Quaternion RotateY(float angleDegrees) const noexcept
    {
        float angleRadians = toRadians(angleDegrees);
        float halfAngle = angleRadians * 0.5f;
        float s = sin(halfAngle);
        float c = cos(halfAngle);

        Quaternion q;
        q.x = c * x - s * z;
        q.y = c * y + s * w;
        q.z = c * z + s * x;
        q.w = c * w - s * y;
        return q;
    }
    Quaternion RotateZ(float angleDegrees) const noexcept
    {
        float angleRadians = toRadians(angleDegrees);
        float halfAngle = angleRadians * 0.5f;
        float s = sin(halfAngle);
        float c = cos(halfAngle);

        Quaternion q;
        q.x = c * x + s * y;
        q.y = c * y - s * x;
        q.z = c * z + s * w;
        q.w = c * w - s * z;
        return q;
    }

    static Quaternion fromEuler(float roll, float pitch, float yaw) noexcept
    {
        const float cy = lookupCosine(yaw * 0.5f);
        const float cp = lookupCosine(pitch * 0.5f);
        const float cr = lookupCosine(roll * 0.5f);
        const float sy = lookupSine(yaw * 0.5f);
        const float sp = lookupSine(pitch * 0.5f);
        const float sr = lookupSine(roll * 0.5f);

        return 
        { 
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            cr * cp * cy + sr * sp * sy
        };
    }

    Matrix4x4f toMatrix4x4f() const 
    {
        Matrix4x4f matrix;

        float xx = x * x;
        float yy = y * y;
        float zz = z * z;
        float xy = x * y;
        float xz = x * z;
        float yz = y * z;
        float wx = w * x;
        float wy = w * y;
        float wz = w * z;

        matrix.m[0] = 1.0f - 2.0f * (yy + zz);
        matrix.m[1] = 2.0f * (xy + wz);
        matrix.m[2] = 2.0f * (xz - wy);
        matrix.m[3] = 0.0f;

        matrix.m[4] = 2.0f * (xy - wz);
        matrix.m[5] = 1.0f - 2.0f * (xx + zz);
        matrix.m[6] = 2.0f * (yz + wx);
        matrix.m[7] = 0.0f;

        matrix.m[8] = 2.0f * (xz + wy);
        matrix.m[9] = 2.0f * (yz - wx);
        matrix.m[10] = 1.0f - 2.0f * (xx + yy);
        matrix.m[11] = 0.0f;

        matrix.m[12] = 0.0f;
        matrix.m[13] = 0.0f;
        matrix.m[14] = 0.0f;
        matrix.m[15] = 1.0f;

        return matrix;
    }

};

#endif