#pragma once

class Vector
{
public:
    mutable float x;
    mutable float y;
    mutable float z;
    mutable float w;
    
    Vector(float x, float y, float z, float w) 
        : x(x), y(y), z(z), w(w) {}
    Vector& operator=(Vector const& other)=default;
    ~Vector()=default;
    
    // Default operators
    Vector operator+(Vector const& other) const;
    Vector operator-(Vector const& other) const;
    Vector operator*(Vector const& other) const;
    Vector operator/(Vector const& other) const;
    
    // Default methods
    float Magnitude() const;
    
};
class Vector2 : public Vector
{
public:
    Vector2(float x, float y) : Vector(x, y, 0.0f, 0.0f) {}
};
class Vector3 : public Vector
{
public:
    Vector3(float x, float y, float z) : Vector(x, y, z, 0.0f) {}
};
class Vector4 : public Vector
{
public:
    Vector4(float x, float y, float z, float w) : Vector(x, y ,z, w) {}
};