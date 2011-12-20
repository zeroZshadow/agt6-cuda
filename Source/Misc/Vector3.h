//Code taken from jacco template
#pragma once


	class Vector3
	{
	public:
		Vector3() : x( 0.0f ), y( 0.0f ), z( 0.0f ) {};
		Vector3( float a_X, float a_Y, float a_Z ) : x( a_X ), y( a_Y ), z( a_Z ) {};

		void Set( float a_X, float a_Y, float a_Z );
		void Normalize();
		float Length() const;
		float SqrLength() const;
		float Dot( Vector3 a_V ) const;
		Vector3 Cross( Vector3 v ) const;
		void operator += ( const Vector3& a_V );
		void operator += ( Vector3* a_V );
		void operator -= ( const Vector3& a_V );
		void operator -= ( Vector3* a_V );
		void operator *= ( const float f );
		void operator *= ( const Vector3& a_V );
		void operator *= ( Vector3* a_V );
		float& operator [] ( int a_N );


		friend Vector3 operator + ( const Vector3& v1, const Vector3& v2 );
		Vector3 operator- () const;
		friend Vector3 operator + ( const Vector3& v1, const Vector3& v2 ) { return Vector3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z ); }
		friend Vector3 operator + ( const Vector3& v1, Vector3* v2 ) { return Vector3( v1.x + v2->x, v1.y + v2->y, v1.z + v2->z ); }
		friend Vector3 operator - ( const Vector3& v1, const Vector3& v2 ) { return Vector3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z ); }
		friend Vector3 operator - ( const Vector3& v1, Vector3* v2 ) { return Vector3( v1.x - v2->x, v1.y - v2->y, v1.z - v2->z ); }
		friend Vector3 operator - ( const Vector3* v1, Vector3& v2 ) { return Vector3( v1->x - v2.x, v1->y - v2.y, v1->z - v2.z ); }
		//  Vector3 operator - ( const Vector3* v1, Vector3* v2 ) { return Vector3( v1->x - v2->x, v1->y - v2->y, v1->z - v2->z ); }
		friend Vector3 operator ^ ( const Vector3& A, const Vector3& B ) { return Vector3(A.y*B.z-A.z*B.y,A.z*B.x-A.x*B.z,A.x*B.y-A.y*B.x); }
		friend Vector3 operator ^ ( const Vector3& A, Vector3* B ) { return Vector3(A.y*B->z-A.z*B->y,A.z*B->x-A.x*B->z,A.x*B->y-A.y*B->x); }
		friend Vector3 operator * ( const Vector3& v, const float f ) { return Vector3( v.x * f, v.y * f, v.z * f ); }
		friend Vector3 operator * ( const Vector3& v1, const Vector3& v2 ) { return Vector3( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z ); }
		friend Vector3 operator * ( const float f, const Vector3& v ) { return Vector3( v.x * f, v.y * f, v.z * f ); }
		friend Vector3 operator / ( const Vector3& v, const float f ) { return Vector3( v.x / f, v.y / f, v.z / f ); }
		friend Vector3 operator / ( const Vector3& v1, const Vector3& v2 ) { return Vector3( v1.x / v2.x, v1.y / v2.y, v1.z / v2.z ); }
		friend Vector3 operator / ( const float f, const Vector3& v ) { return Vector3( v.x / f, v.y / f, v.z / f ); }

		union
		{
			struct { float x, y, z; };
			struct { float cell[3]; };
		};
	};

