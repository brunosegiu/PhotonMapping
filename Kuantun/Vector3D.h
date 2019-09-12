#ifndef __MATHLIB3D_VECTOR3D__
#define __MATHLIB3D_VECTOR3D__

class Vector3D {
public:

    /* Constructors */
    Vector3D();
    Vector3D(const float& x, const float& y, const float& z);
    Vector3D(const Vector3D* pVector);
    Vector3D(const Vector3D& vector);

    /* Get Methods - Original Vector Not Modified */
    const float GetMagnitude() const;
    const float GetDot(const Vector3D& vector) const;
    const float GetAngle(const Vector3D& vector) const;
    Vector3D GetNormalized() const;
    Vector3D GetInverted() const;
    Vector3D GetCross(const Vector3D& vector) const;
    Vector3D GetProjected(const Vector3D& vector) const;
    Vector3D GetSymmetrical(const Vector3D& vector) const;
    const float GetSphericCoordinates(float& angle1, float& angle2) const;
    const float GetDistance(Vector3D& vector) const;
	Vector3D Vector3D::GetCoordProduct(const Vector3D &vector) const;

    /* Set Methods - Original Vector Modified */
    Vector3D& SetMagnitude(const float& magnitude);
    Vector3D& SetNormalized();
    Vector3D& SetInverted();
    Vector3D& SetCross(const Vector3D& vector);
    Vector3D& SetCross(const Vector3D& vector1, const Vector3D& vector2);
    Vector3D& SetProjected(const Vector3D& vector);
    Vector3D& SetProjected(const Vector3D& vector1, const Vector3D& vector2);

    /* Operator replacements */
    Vector3D GetAdd(const Vector3D& vector) const;
    Vector3D& SetAdd(const Vector3D& vector);
    Vector3D& SetAdd(const Vector3D& vector1, const Vector3D& vector2);

    Vector3D GetSub(const Vector3D& vector) const;
    Vector3D& SetSub(const Vector3D& vector);
    Vector3D& SetSub(const Vector3D& vector1, const Vector3D& vector2);

    Vector3D GetMul(const float& scalar) const;
    Vector3D& SetMul(const float& scalar);
    Vector3D& SetMul(const Vector3D& vector, const float& scalar);

    Vector3D GetDiv(const float& scalar) const;
    Vector3D& SetDiv(const float& scalar);
    Vector3D& SetDiv(const Vector3D& vector, const float& scalar);

    const bool equals(const Vector3D& vector) const;
    const bool notEquals(const Vector3D& vector) const;

    /* Operators */
    Vector3D operator + (const Vector3D& vector) const;
    Vector3D operator - (const Vector3D& vector) const;
    Vector3D operator * (const float& scalar) const;
    Vector3D operator / (const float& scalar) const;
    Vector3D& operator += (const Vector3D& vector);
    Vector3D& operator -= (const Vector3D& vector);
    Vector3D& operator *= (const float& scalar);
    Vector3D& operator /= (const float& scalar);
    Vector3D operator - () const;
    const bool operator == (Vector3D &vector) const;
    const bool operator != (Vector3D &vector);

    /* Print */
    void print();

public:
    float x;
    float y;
    float z;
};
#endif
