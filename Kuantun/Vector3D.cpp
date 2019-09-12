#include "Vector3D.h"
#include <iostream>
#include <cmath>

/**
 * Default constructor of the Vector3D class
 */
Vector3D::Vector3D() : x(0), y(0), z(0) {}

/**
 * Default constructor of the Vector3D class
 * @param x x coordinate value
 * @param y y coordinate value
 * @param z z coordinate value
 */
Vector3D::Vector3D(const float& x, const float& y, const float& z) : x(x), y(y), z(z) {}

/**
 * Copy constructor of the Vector3D class
 * @param vector vector to copy
 */
Vector3D::Vector3D(const Vector3D* pVector) : x(pVector->x), y(pVector->y), z(pVector->z) {}

/**
 * Copy constructor of the Vector3D class
 * @param vector vector to copy
 */
Vector3D::Vector3D(const Vector3D& vector) : x(vector.x), y(vector.y), z(vector.z) {}

/**
 * Magnitude of the vector (3D)
 * @return magnitude of the vector
 */
const float Vector3D::GetMagnitude() const {
	return static_cast<float>(sqrt(x*x + y*y + z*z));
}

/**
 * Dot product of two vectors (3D)
 * @param vector second vector on which the dot product is applied
 * @return dot product value
 */
const float Vector3D::GetDot(const Vector3D& vector) const {
	return (x * vector.x + y * vector.y + z * vector.z);
}

/**
 * Angle between two vectors (3D)
 * @param vector second vector of the pair, where the angle is calculated
 * @return angle between the two vectors
 */
const float Vector3D::GetAngle(const Vector3D& vector) const {
	const float magnitudesProduct = GetMagnitude() * vector.GetMagnitude();

	if (magnitudesProduct != 0.0f)
		return static_cast<float>(acos(GetDot(vector) / magnitudesProduct));
	else
		return 0.0f;
}

/**
 * Normalizes an INSTANCE of the vector (3D)
 * @return reference to a new normalized Vector3D
 */
Vector3D Vector3D::GetNormalized() const {
	const float magnitude = GetMagnitude();

	if (magnitude != 0.0f)
		return Vector3D(x / magnitude, y / magnitude, z / magnitude);
	else
		return (*this);
}

/**
 * Inverts an INSTANCE of the vector (3D)
 * @return reference to a new inverted Vector3D
 */
Vector3D Vector3D::GetInverted() const {
	return Vector3D(-x, -y, -z);
}

/**
 * Cross product of the two Vector3D, outputs a Vector3D
 * @param vector second vector of the pair
 * @return new perpendicular Vector3D
 */
Vector3D Vector3D::GetCross(const Vector3D& vector) const {
	return Vector3D(y * vector.z - z * vector.y,
					z * vector.x - x * vector.z,
					x * vector.y - y * vector.x);
}

/**
 * Projects an INSTANCE of the vector onto another (3D)
 * @param vector vector on which the projection is done
 * @return reference to a new normalized Vector3D
 */
Vector3D Vector3D::GetProjected(const Vector3D& vector) const {
	const float selfDot = vector.GetDot(vector);

	if (selfDot != 0.0f)
		return (vector * (GetDot(vector) / (selfDot)));
	else
		return (*this);
}
/**
 * Simetrico
 * @param vector vector on which the projection is done
 * @return normal to a new sim Vector3D
 */
Vector3D Vector3D::GetSymmetrical(const Vector3D& vector) const {
    return vector*2*vector.GetDot(this)-this;
}

Vector3D Vector3D::GetCoordProduct(const Vector3D &vector) const {
	return Vector3D(this->x * vector.x, this->y * vector.y, this->z * vector.z);
}

/**
 * Calculates the spherical coordinates of the vector (3D)
 * @param angle1 first angle
 * @param angle2 second angle
 * @return magnitude of the vector
 */
const float Vector3D::GetSphericCoordinates(float& angle1, float& angle2) const {
	const float magnitude = GetMagnitude();
	const float squaredMagnitude = magnitude * magnitude;
	const float squaredY = y * y;
	const float ratio = static_cast<float>(squaredMagnitude - squaredY);

	if ((ratio != 0.0f) && (magnitude != 0.0f)) {
		angle1 = static_cast<float>(acos(x / ratio));
		angle2 = static_cast<float>(acos(y / magnitude));
	}

	return magnitude;
}

/**
 * Calculates the distance between two vectors (3D)
 * @param vector vector to calculate the distance from
 * @return distance between the two vectors
 */
const float Vector3D::GetDistance(Vector3D& vector) const {
	Vector3D tmp = *this;
	tmp -= vector;
	return tmp.GetMagnitude();
}

/**
 * Changes the magnitude of the vector (3D)
 * @param new magnitude value
 * @return new vector
 */
Vector3D& Vector3D::SetMagnitude(const float& magnitude) {
	const float currentMagnitude = GetMagnitude();

	if (currentMagnitude != 0.0f) {
		const float magnitudeRatio = magnitude / currentMagnitude;	//Faster, but not nice.
		x *= magnitudeRatio;
		y *= magnitudeRatio;
		z *= magnitudeRatio;
	}

	return (*this);
}

/**
 * Normalizes the vector (3D)
 * @return reference to THIS normalized Vector3D
 */
Vector3D& Vector3D::SetNormalized() {
	const float magnitude = GetMagnitude();

	if (magnitude != 0.0f) {
		x /= magnitude;
		y /= magnitude;
		z /= magnitude;
	}

	return (*this);
}

/**
 * Inverts the vector (3D)
 * @return reference to THIS inverted Vector3D
 */
Vector3D& Vector3D::SetInverted() {
	x = -x;
	y = -y;
	z = -z;
	return (*this);
}

/**
 * Cross product of the current vector and param, onto current vector
 * @param vector vector with which the cross product is done
 * @return reference to THIS crossed Vector3D
 */
Vector3D& Vector3D::SetCross(const Vector3D& vector) {
	*this = GetCross(vector);
	return (*this);
}

/**
 * Cross product of 2 param vectors, stored in current vector.
 * @param vector1 first vector of the cross product
 * @param vector2 second vector of the cross product
 * @return reference to THIS crossed Vector3D
 */
Vector3D& Vector3D::SetCross(const Vector3D& vector1, const Vector3D& vector2) {
	*this = vector1.GetCross(vector2);
	return (*this);
}

/**
 * Projects the vector onto another (3D)
 * @param vector vector on which the projection is done
 * @return reference to THIS projected Vector3D
 */
Vector3D& Vector3D::SetProjected(const Vector3D& vector) {
	*this = GetProjected(vector);
	return (*this);
}

/**
 * Projects the two param vectors and stores in current vector
 * @param vector1 vector to project second vector on
 * @param vector2 vector projected onto first one
 * @return reference to THIS projected Vector3D
 */
Vector3D& Vector3D::SetProjected(const Vector3D& vector1, const Vector3D& vector2) {
	*this = vector1.GetProjected(vector2);
	return (*this);
}

/**
 * ADD operator replacement
 * @param vector vector to sum
 * @return summation result
 */
Vector3D Vector3D::GetAdd(const Vector3D& vector) const {
	return Vector3D(this->x + vector.x, this->y + vector.y, this->z + vector.z);
}

/**
 * ADD_EQUAL operator replacement
 * @param vector vector to add to the current vector
 * @return summation result
 */
Vector3D& Vector3D::SetAdd(const Vector3D& vector) {
	this->x += vector.x;
	this->y += vector.y;
	this->z += vector.z;

	return (*this);
}

/**
 * ADD_EQUAL operator replacement
 * @param vector1 first vector to add
 * @param vector2 second vector to add
 * @return summation result
 */
Vector3D& Vector3D::SetAdd(const Vector3D& vector1, const Vector3D& vector2) {
	this->x = vector1.x + vector2.x;
	this->y = vector1.y + vector2.y;
	this->z = vector1.z + vector2.z;

	return (*this);
}

/**
 * SUB operator replacement
 * @param vector vector to sub
 * @return substraction result
 */
Vector3D Vector3D::GetSub(const Vector3D& vector) const {
	return Vector3D(this->x - vector.x, this->y - vector.y, this->z - vector.z);
}

/**
 * SUB_EQUAL operator replacement
 * @param vector vector to sub to the current vector
 * @return substraction result
 */
Vector3D& Vector3D::SetSub(const Vector3D& vector) {
	this->x -= vector.x;
	this->y -= vector.y;
	this->z -= vector.z;

	return (*this);
}

/**
 * SUB_EQUAL operator replacement
 * @param vector1 first vector to sub
 * @param vector2 second vector to sub
 * @return substraction result
 */
Vector3D& Vector3D::SetSub(const Vector3D& vector1, const Vector3D& vector2) {
	this->x = vector1.x - vector2.x;
	this->y = vector1.y - vector2.y;
	this->z = vector1.z - vector2.z;

	return (*this);
}

/**
 * MULTIPLY operator replacement
 * @param scalar scalar value multiplied by the vector
 * @return scaled vector
 */
Vector3D Vector3D::GetMul(const float& scalar) const {
	return Vector3D(this->x * scalar, this->y * scalar, this->z * scalar);
}

/**
 * MULTIPLY_EQUAL operator replacement
 * @param scalar scalar multiplied by the current vector
 * @return scaled vector
 */
Vector3D& Vector3D::SetMul(const float& scalar) {
	this->x *= scalar;
	this->y *= scalar;
	this->z *= scalar;

	return (*this);
}

/**
 * MULTIPLY_EQUAL operator replacement
 * @param vector1 first vector to multiply
 * @param scalar scalar to multiply to the first vector
 * @return scaled vector
 */
Vector3D& Vector3D::SetMul(const Vector3D& vector, const float& scalar) {
	this->x = vector.x * scalar;
	this->y = vector.y * scalar;
	this->z = vector.z * scalar;

	return (*this);
}

/**
 * DIVIDE operator replacement
 * @param scalar scalar value that divides the vector
 * @return scaled vector
 */
Vector3D Vector3D::GetDiv(const float& scalar) const {
	return Vector3D(this->x / scalar, this->y / scalar, this->z / scalar);
}

/**
 * DIVIDE_EQUAL operator replacement
 * @param scalar scalar multiplied by the current vector
 * @return scaled vector
 */
Vector3D& Vector3D::SetDiv(const float& scalar) {
	this->x /= scalar;
	this->y /= scalar;
	this->z /= scalar;

	return (*this);
}

/**
 * MULTIPLY_EQUAL operator replacement
 * @param vector1 first vector to multiply
 * @param scalar scalar to multiply to the first vector
 * @return scaled vector
 */
Vector3D& Vector3D::SetDiv(const Vector3D& vector, const float& scalar) {
	this->x = vector.x / scalar;
	this->y = vector.y / scalar;
	this->z = vector.z / scalar;

	return (*this);
}

/**
 * EQUAL_EQUAL operator
 * @param vector vector to compare
 * @return equal value
 */
const bool Vector3D::equals(const Vector3D& vector) const {
	return ((this->x == vector.x) && (this->y == vector.y) && (this->z == vector.z));
}

/**
 * NOT_EQUAL operator
 * @param vector vector to compare
 * @return not equal value
 */
const bool Vector3D::notEquals(const Vector3D& vector) const {
	return ((this->x != vector.x) && (this->y != vector.y) && (this->z != vector.z));
}

/**
 * ADD operator
 * @param vector vector to sum
 * @return summation result
 */
Vector3D Vector3D::operator + (const Vector3D& vector) const {
	return Vector3D(x + vector.x, y + vector.y, z + vector.z);
}

/**
 * MINUS operator
 * @param vector vector to minus(sic)
 * @return minus result
 */
Vector3D Vector3D::operator - (const Vector3D& vector) const {
	return Vector3D(x - vector.x, y - vector.y, z - vector.z);
}

/**
 * MULTIPLY operator
 * @param scalar scalar value divided by the vector
 * @return scaled vector
 */
Vector3D Vector3D::operator * (const float& scalar) const {
	return Vector3D(x * scalar, y * scalar, z * scalar);
}

/**
 * DIVIDE operator
 * @param scalar scalar value divided by the vector
 * @return scaled vector
 */
Vector3D Vector3D::operator / (const float& scalar) const {
	if (scalar != 0.0f)
		return Vector3D(x / scalar, y / scalar, z / scalar);
	else
		return (*this);
}

/**
 * ADD_EQUAL operator
 * @param vector vector to add to the current vector
 * @return summation result
 */
Vector3D& Vector3D::operator += (const Vector3D& vector) {
	x += vector.x;
	y += vector.y;
	z += vector.z;
	return (*this);
}

/**
 * MINUS_EQUAL operator
 * @param vector vector to minus(sic) to the current vector
 * @return minus result
 */
Vector3D& Vector3D::operator -= (const Vector3D& vector) {
	x -= vector.x;
	y -= vector.y;
	z -= vector.z;
	return (*this);
}

/**
 * MULTIPLY_EQUAL operator
 * @param scalar scalar multiplied by the current vector
 * @return scaled result
 */
Vector3D& Vector3D::operator *= (const float& scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return (*this);
}

/**
 * DIVIDE_EQUAL operator
 * @param scalar scalar divided by the current vector
 * @return scaled result
 */
Vector3D& Vector3D::operator /= (const float& scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
	return (*this);
}

/**
 * SIGN operator
 * @return signed result
 */
Vector3D Vector3D::operator - () const {
	return Vector3D(-x, -y, -z);
}

/**
 * EQUAL_EQUAL operator
 * @param vector vector to compare
 * @return equal value
 */
const bool Vector3D::operator == (Vector3D &vector) const {
	return ((x == vector.x) && (y == vector.y) && (z == vector.z)) ? true : false;
}

/**
 * NOT_EQUAL operator
 * @param vector vector to compare
 * @return not equal value
 */
const bool Vector3D::operator != (Vector3D &vector) {
	return ((x != vector.x) && (y != vector.y) && (z != vector.z)) ? true : false;
}

/**
 * Global MULTIPLY operator
 * @param scalar scalar value to multiply with the second parameter
 * @param vector vector to multiply the scalar value with
 * @return scaled vector instance
 */
Vector3D operator * (float scalar, Vector3D& vector) {
	return Vector3D(vector.x * scalar, vector.y * scalar, vector.z * scalar);
}

/**
 * Global DIVIDE operator
 * @param scalar scalar value to divide with the second parameter
 * @param vector vector to divide the scalar value with
 * @return scaled vector instance
 */
Vector3D operator / (float scalar, Vector3D& vector) {
	return Vector3D(vector.x / scalar, vector.y / scalar, vector.z / scalar);
}

void Vector3D::print(){
    std::cout << "Vector: X(" << this->x << "). Y(" << this->y << ") Z(" << this->z << ")." << std::endl;
}
