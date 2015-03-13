#pragma once
#include <windows.h>
#include <math.h>

#include <emmintrin.h>
#include <xmmintrin.h>

#include <cstring>


#define FLT_PI 		3.1415925f
#define FLT_PI2		1.5707963f
#define FLT_EPSILON 1.192092896e-07f

#define max(a,b) (((a)>(b))?(a):(b))
#define min(a,b) (((a)<(b))?(a):(b))

static void SinCos(const float rad, float &sin, float &cos) // #include <emmintrin.h>, #include <xmmintrin.h>
{
	const __m128 _ps_fopi = _mm_set_ss(1.27323954473516f);

	const __m128 _ps_0p5 = _mm_set_ss(0.5f);
	const __m128 _ps_1   = _mm_set_ss(1.0f);

	const __m128 _ps_dp1 = _mm_set_ss(-0.7851562f);
	const __m128 _ps_dp2 = _mm_set_ss(-2.4187564849853515625e-4f);
	const __m128 _ps_dp3 = _mm_set_ss(-3.77489497744594108e-8f);

	const __m128 _ps_sincof_p0 = _mm_set_ss(2.443315711809948e-5f);
	const __m128 _ps_sincof_p1 = _mm_set_ss(8.3321608736e-3f);
	const __m128 _ps_sincof_p2 = _mm_set_ss(-1.6666654611e-1f);
	const __m128 _ps_coscof_p0 = _mm_set_ss(2.443315711809948e-5f);
	const __m128 _ps_coscof_p1 = _mm_set_ss(-1.388731625493765e-3f);
	const __m128 _ps_coscof_p2 = _mm_set_ss(4.166664568298827e-2f);

	const __m128i _pi32_1  = _mm_set1_epi32(1);
	const __m128i _pi32_i1 = _mm_set1_epi32(~1);
	const __m128i _pi32_2  = _mm_set1_epi32(2);
	const __m128i _pi32_4  = _mm_set1_epi32(4);

	const __m128 _mask_sign_raw = (__m128)_mm_set1_epi32(0x80000000);
	const __m128 _mask_sign_inv = (__m128)_mm_set1_epi32(~0x80000000);


	__m128  xmm1, xmm2, xmm3 = _mm_setzero_ps();
	__m128i emm0, emm2, emm4;

	__m128 sign_bit_cos, sign_bit_sin;

	__m128 x, y, z;
	__m128 y1,  y2;

	__m128 a = _mm_set_ss(rad);

	x = _mm_and_ps(a, _mask_sign_inv);
	y = _mm_mul_ps(x, _ps_fopi);

	emm2 = _mm_cvtps_epi32(y);
	emm2 = _mm_add_epi32(emm2, _pi32_1);
	emm2 = _mm_and_si128(emm2, _pi32_i1);
	y    = _mm_cvtepi32_ps(emm2);

	emm4 = emm2;

	emm0 = _mm_and_si128(emm2, _pi32_4);
	emm0 = _mm_slli_epi32(emm0, 29);
	__m128 swap_sign_bit_sin = _mm_castsi128_ps(emm0);

	emm2 = _mm_and_si128(emm2, _pi32_2);
	emm2 = _mm_cmpeq_epi32(emm2, _mm_setzero_si128());
	__m128 poly_mask = _mm_castsi128_ps(emm2);

	x = _mm_add_ps(x, _mm_mul_ps(y, _ps_dp1));
	x = _mm_add_ps(x, _mm_mul_ps(y, _ps_dp2));
	x = _mm_add_ps(x, _mm_mul_ps(y, _ps_dp3));

	emm4 = _mm_sub_epi32(emm4, _pi32_2);
	emm4 = _mm_andnot_si128(emm4, _pi32_4);
	emm4 = _mm_slli_epi32(emm4, 29);

	sign_bit_cos = _mm_castsi128_ps(emm4);
	sign_bit_sin = _mm_xor_ps(_mm_and_ps(a, _mask_sign_raw), swap_sign_bit_sin);

	z = _mm_mul_ps(x, x);

	y1 = _mm_mul_ps(_ps_coscof_p0, z);
	y1 = _mm_add_ps(y1, _ps_coscof_p1);
	y1 = _mm_mul_ps(y1, z);
	y1 = _mm_add_ps(y1, _ps_coscof_p2);
	y1 = _mm_mul_ps(y1, z);
	y1 = _mm_mul_ps(y1, z);
	y1 = _mm_sub_ps(y1, _mm_mul_ps(z, _ps_0p5));
	y1 = _mm_add_ps(y1, _ps_1);

	y2 = _mm_mul_ps(_ps_sincof_p0, z);
	y2 = _mm_add_ps(y2, _ps_sincof_p1);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_add_ps(y2, _ps_sincof_p2);
	y2 = _mm_mul_ps(y2, z);
	y2 = _mm_mul_ps(y2, x);
	y2 = _mm_add_ps(y2, x);

	xmm3 = poly_mask;

	__m128 ysin2 = _mm_and_ps(xmm3, y2);
	__m128 ysin1 = _mm_andnot_ps(xmm3, y1);

	sin = _mm_cvtss_f32(_mm_xor_ps(_mm_add_ps(ysin1, ysin2), sign_bit_sin));
	cos = _mm_cvtss_f32(_mm_xor_ps(_mm_add_ps(_mm_sub_ps(y1, ysin1), _mm_sub_ps(y2, ysin2)), sign_bit_cos));
}

inline float Atan2(float y, float x)
{
	if (x == 0.0f)
	{
		if (y > 0.0f)
		{
			return FLT_PI2;
		}

		if (y == 0.0f)
		{
			return 0.0f;
		}

		return -FLT_PI2;
	}

	float atan;
	float z = y / x;

	if (fabs(z) < 1.0f)
	{
		atan = z / (1.0f + 0.28f * z * z);

		if (x < 0.0f)
		{
			if (y < 0.0f)
			{
				return atan - FLT_PI;
			}

			return atan + FLT_PI;
		}
	}
	else
	{
		atan = FLT_PI2 - z / (z * z + 0.28f);

		if (y < 0.0f)
		{
			return atan - FLT_PI;
		}
	}

	return atan;
}

inline float Rad2Deg(const float &i)
{
	return i * (180.f / FLT_PI);
}

inline float Deg2Rad(const float &i)
{
	return i * (FLT_PI / 180.f);
}

inline float Sqrt(const float &i) // #include <xmmintrin.h>
{
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(i)));
}


class Vector
{
public:
	Vector();
	Vector(float, float, float);

	float  &operator[](int);
	Vector &operator=(const Vector &);

	Vector &operator+=(const Vector &);
	Vector &operator-=(const Vector &);
	Vector &operator*=(const Vector &);
	Vector &operator*=(const float);
	Vector &operator/=(const Vector &);
	Vector &operator/=(const float);

	Vector operator+(const Vector &) const;
	Vector operator-(const Vector &) const;
	Vector operator/(const Vector &) const;
	Vector operator/(const float) const;
	Vector operator*(const Vector &) const;
	Vector operator*(const float) const;

	bool	operator==(const Vector &) const;
	bool	operator!=(const Vector &) const;

	void	Zero();
	bool	IsZero(const float = FLT_EPSILON) const;
	bool	ToScreen(Vector &) const;

	Vector	Cross(const Vector &) const;
	Vector	Rotate(const Vector &) const;
	void	RotateInPlace(const Vector &);

	float	Length() const;
	float	Length2D() const;
	float	Dot(const Vector &) const;
	float	DistTo(const Vector &) const;
	float	Normalize();
	void	NormalizeAngle();

	float	x,y,z;
};

typedef Vector Angle;


class VectorAligned : public Vector
{
public:
	float w;
};


class matrix4x4
{
public:
	inline float *operator[](int i)
	{
		return m[i];
	}

	inline const float *operator[](int i) const
	{
		return m[i];
	}

	float m[4][4];
};

class matrix3x4
{
public:
	inline float *operator[](int i)
	{
		return m[i];
	}

	inline const float *operator[](int i) const
	{
		return m[i];
	}

	float m[3][4];
};

inline void Cross(const Vector &a, const Vector &b, Vector &x)
{
	x.x = ((a.y * b.z) - (a.z * b.y));
	x.y = ((a.z * b.x) - (a.x * b.z));
	x.z = ((a.x * b.y) - (a.y * b.x));
}

inline void AngleMatrix(const Angle &angles, matrix3x4 &matrix)
{
	float sp, sy, sr, cp, cy, cr;

	SinCos(Deg2Rad(angles.x), sp, cp);
	SinCos(Deg2Rad(angles.y), sy, cy);
	SinCos(Deg2Rad(angles.z), sr, cr);

	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;

	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = sp * crcy + srsy;
	matrix[1][2] = sp * crsy - srcy;
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

inline void AngleVectors(const Angle &angles, Vector &forward)
{
	float sp, sy, cp, cy;

	SinCos(Deg2Rad(angles.x), sp, cp);
	SinCos(Deg2Rad(angles.y), sy, cy);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward.Normalize();
}

inline void AngleVectors(const Angle &angles, Vector &forward, Vector &right, Vector &up)
{
	float sp, sy, sr, cp, cy, cr;

	SinCos(Deg2Rad(angles.x), sp, cp);
	SinCos(Deg2Rad(angles.y), sy, cy);
	SinCos(Deg2Rad(angles.z), sr, cr);


	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward.Normalize();

	right.x = -(sr * sp * cy) + (cr * sy);
	right.y = -(sr * sp * sy) + -(cr * cy);
	right.z = -(sr * cp);
	right.Normalize();

	up.x = cr * sp * cy + -sr * -sy;
	up.y = cr * sp * sy + -sr * cy;
	up.z = cr * cp;
	up.Normalize();
}

inline void VectorRotate(const Vector &i, const matrix3x4 &matrix, Vector &o)
{
	o.x = i.Dot(Vector(matrix[0][0], matrix[0][1], matrix[0][2]));
	o.y = i.Dot(Vector(matrix[1][0], matrix[1][1], matrix[1][2]));
	o.z = i.Dot(Vector(matrix[2][0], matrix[2][1], matrix[2][2]));
}

inline void VectorRotate(const Vector &i, const Angle &angles, Vector &o)
{
	matrix3x4 matrix;

	AngleMatrix(angles, matrix);
	VectorRotate(i, matrix, o);
}

inline void VectorAngles(const Vector &vec, Angle &angles)
{
	if (vec.x == 0.0f && vec.y == 0.0f)
	{
		if (vec.z > 0.0f)
		{
			angles.x = -90.0f;
		}
		else
		{
			angles.x = 90.0f;
		}
	}
	else
	{
		angles.x = Rad2Deg(Atan2(-vec.z, vec.Length2D()));
		angles.y = Rad2Deg(Atan2(vec.y, vec.x));
	}
}

inline void VectorAngles(const Vector &vec, const Vector &up, Angle &angles)
{
	Vector left;
	Cross(up, vec, left);

	left.Normalize();

	float len = vec.Length2D();
	angles.x = Rad2Deg(Atan2(-vec.z, len));

	if (len > 0.001f)
	{
		angles.y = Rad2Deg(Atan2(vec.y, vec.x));
		angles.z = Rad2Deg(Atan2(left.z, ((left.y * vec.x) - (left.x * vec.y))));
	}
	else
	{
		angles.y = Rad2Deg(Atan2(-left.x, left.y));
		angles.z = 0.0f;
	}

	//NormalizeAngles(angles);
}

inline void VectorTransform(const Vector &v, const matrix3x4 &matrix, Vector &o)
{
	o.x = v.Dot(Vector(matrix[0][0], matrix[0][1], matrix[0][2])) + matrix[0][3];
	o.y = v.Dot(Vector(matrix[1][0], matrix[1][1], matrix[1][2])) + matrix[1][3];
	o.z = v.Dot(Vector(matrix[2][0], matrix[2][1], matrix[2][2])) + matrix[2][3];
}

inline void MatrixGetColumn(const matrix3x4 &matrix, int i, Vector &o)
{
	o.x = matrix[0][i];
	o.y = matrix[1][i];
	o.z = matrix[2][i];
}

inline void MatrixPosition(const matrix3x4 &matrix, Vector &vec)
{
	MatrixGetColumn(matrix, 3, vec);
}

inline void MatrixAngles(const matrix3x4 &matrix, Angle &angles)
{
	Vector forward, left, up;

	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];

	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];

	up[2] = matrix[2][2];

	float len2d = forward.Length2D();
	if (len2d > 0.001f)
	{
		angles.x = Rad2Deg(Atan2(-forward.z, len2d));
		angles.y = Rad2Deg(Atan2(forward.y, forward.x));
		angles.z = Rad2Deg(Atan2(left.z, up.z));
	}
	else
	{
		angles.x = Rad2Deg(Atan2(-forward.z, len2d));
		angles.y = Rad2Deg(Atan2(-left.x, left.y));
		angles.z = 0.f;
	}
}

inline void MatrixAngles(const matrix3x4 &matrix, Angle &angles, Vector &vec)
{
	MatrixAngles(matrix, angles);
	MatrixPosition(matrix, vec);
}

inline Vector::Vector()
{
	x = y = z = 0.0f;
}

inline Vector::Vector(float a, float b, float c)
{
	x = a;
	y = b;
	z = c;
}

inline void Vector::RotateInPlace(const Angle &angles)
{
	VectorRotate(*this, angles, *this);
}

inline Vector Vector::Rotate(const Angle &angles) const
{
	Vector o;
	VectorRotate(*this, angles, o);

	return o;
}

inline Vector& Vector::operator=(const Vector &v)
{
	x = v.x;
	y = v.y;
	z = v.z;

	return *this;
}

inline Vector& Vector::operator+=(const Vector &v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

inline Vector& Vector::operator-=(const Vector &v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;

	return *this;
}

inline Vector& Vector::operator*=(const Vector &v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

inline Vector& Vector::operator*=(const float f)
{
	x *= f;
	y *= f;
	z *= f;

	return *this;
}

inline Vector& Vector::operator/=(const Vector &v)
{
	x /= v.x + FLT_EPSILON;
	y /= v.y + FLT_EPSILON;
	z /= v.z + FLT_EPSILON;

	return *this;
}

inline Vector& Vector::operator/=(const float f)
{
	x /= f + FLT_EPSILON;
	y /= f + FLT_EPSILON;
	z /= f + FLT_EPSILON;

	return *this;
}

inline Vector Vector::operator+(const Vector &v) const
{
	return Vector(x + v.x, y + v.y, z + v.z);
}

inline Vector Vector::operator-(const Vector &v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}

inline Vector Vector::operator/(const Vector &v) const
{
	return Vector(x / (v.x + FLT_EPSILON), y / (v.y + FLT_EPSILON), z / (v.z + FLT_EPSILON));
}

inline Vector Vector::operator/(const float f) const
{
	return Vector(x / (f + FLT_EPSILON), y / (f + FLT_EPSILON), z / (f + FLT_EPSILON));
}

inline Vector Vector::operator*(const Vector &v) const
{
	return Vector(x * v.x, y * v.y, z * v.z);
}

inline Vector Vector::operator*(const float f) const
{
	return Vector(x * f, y * f, z * f);
}

inline bool Vector::operator==(const Vector &v) const
{
	return
		v.x == x &&
		v.y == y &&
		v.z == z;
}

inline bool Vector::operator!=(const Vector &v) const
{
	return
		v.x != x ||
		v.y != y ||
		v.z != z;
}

inline float& Vector::operator[](int index)
{
	return ((float *)this)[index];
}

inline void Vector::Zero()
{
	x = y = z = 0.0f;
}

inline bool Vector::IsZero(const float f) const
{
	return (x > -f && x < f && y > -f && y < f && z > -f && z < f);
}

inline float Vector::Length() const
{
	return Sqrt((x * x) + (y * y) + (z * z));
}

inline float Vector::Length2D() const
{
	return Sqrt((x * x) + (y * y));
}

inline float Vector::DistTo(const Vector &v) const
{
	return (*this - v).Length();
}

inline float Vector::Normalize()
{
	float l = Length();
	float m = 1.0f / (l + FLT_EPSILON);

	x *= m;
	y *= m;
	z *= m;

	return l;
}

inline float Vector::Dot(const Vector &v) const
{
	return x * v.x + y * v.y + z * v.z;
}

inline Vector Vector::Cross(const Vector &v) const
{
	return Vector(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
}

inline void Vector::NormalizeAngle()
{
	while (x > 180.f)  x -= 360.f;
	while (x < -180.f) x += 360.f;

	while (y > 180.f)  y -= 360.f;
	while (y < -180.f) y += 360.f;

	while (z > 180.f)  z -= 360.f;
	while (z < -180.f) z += 360.f;
}


inline float DotProduct(const Vector &v1, const Vector &v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline Vector CrossProduct(const Vector &v1, const Vector &v2)
{
	return Vector(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}