#include "Vector3.h"
#include "MathMisc.h"

void Vector3::Set( float a_X, float a_Y, float a_Z ) { x = a_X; y = a_Y; z = a_Z; }
void Vector3::Normalize() { float l = 1.0f / Length(); x *= l; y *= l; z *= l; }
float Vector3::Length() const { return (float)sqrt( x * x + y * y + z * z ); }
float Vector3::SqrLength() const { return x * x + y * y + z * z; }
float Vector3::Dot( Vector3 a_V ) const { return x * a_V.x + y * a_V.y + z * a_V.z; }
Vector3 Vector3::Cross( Vector3 v ) const { return Vector3( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x ); }
void Vector3::operator += ( const Vector3& a_V ) { x += a_V.x; y += a_V.y; z += a_V.z; }
void Vector3::operator += ( Vector3* a_V ) { x += a_V->x; y += a_V->y; z += a_V->z; }
void Vector3::operator -= ( const Vector3& a_V ) { x -= a_V.x; y -= a_V.y; z -= a_V.z; }
void Vector3::operator -= ( Vector3* a_V ) { x -= a_V->x; y -= a_V->y; z -= a_V->z; }
void Vector3::operator *= ( const float f ) { x *= f; y *= f; z *= f; }
void Vector3::operator *= ( const Vector3& a_V ) { x *= a_V.x; y *= a_V.y; z *= a_V.z; }
void Vector3::operator *= ( Vector3* a_V ) { x *= a_V->x; y *= a_V->y; z *= a_V->z; }
float& Vector3::operator [] ( int a_N ) { return cell[a_N]; }
Vector3 Vector3::operator- () const { return Vector3( -x, -y, -z ); }




