#include "Vector.h"

/**
 * Vectors
 */
// Default operators
Vector Vector::operator+(Vector const& other) const
{
    return Vector{x + other.x, y + other.y, z + other.z, w + other.w};
}
Vector Vector::operator-(Vector const& other) const
{
    return Vector{x - other.x, y - other.y, z - other.z, w - other.w};
}
Vector Vector::operator*(Vector const& other) const
{
    return Vector{x * other.x, y * other.y, z * other.z, w * other.w};
}
Vector Vector::operator/(Vector const& other) const
{
    return Vector{x / other.x, y / other.y, z / other.z, w / other.w};
}