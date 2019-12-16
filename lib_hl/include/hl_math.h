#pragma once
#include <hl_system.h>

//32位定点数
typedef s32 fp32;

//32位定点数的小数位数 20bit
//整数大小 -2048-2047，小数精度 0.000001
#define FP32_FBIT		20
#define FP32_1		(1<<FP32_FBIT)		//fp32 1f
#define FP32_H5		(1<<(FP32_FBIT-1))	//fp32 0.5f
#define FP32_MAX		2147483647
#define FP32_MIN		(-2147483647-1)
#define FP32_MAXINT	( (1<<(31-FP32_FBIT))-1)
#define FP32_MININT	(-(1<<(31-FP32_FBIT)))
#define FP32_PI		(1686629713>>(29-FP32_FBIT))
#define FP32_SQRT2	(1518500249>>(30-FP32_FBIT))
#define FP32_SQRT3	(1859775393>>(30-FP32_FBIT))
#define FP32_F2(n)	(1<<(FP32_FBIT-(n)))	//fp32 1/(2^n)

//16位定点数
typedef s16 fp16;

//16位定点数的小数位数 10bit
//整数大小 -32-31，小数精度 0.001
#define FP16_FBIT 10
#define FP16_1	(1<<FP16_FBIT)
#define FP16_H5	(1<<(FP16_FBIT-1))
#define FP16_MAX	 32767
#define FP16_MIN -32768
#define FP16_MAXINT	( (1<<(15-FP16_FBIT))-1)
#define FP16_MININT	(-(1<<(16-FP16_FBIT)))

//32位三维矢量
typedef struct XYZ32 { fp32 x; fp32 y; fp32 z; } xyz, Vector3;
//16位三维矢量
typedef struct XYZ16 { fp16 r; fp16 g; fp16 b; } Color;

static const Color _color_black = { 0, 0, 0 };
static const Color _color_white = { FP16_1, FP16_1, FP16_1 };
//extern Color _color_black;
//extern Color _color_white;

// 3 * xyz32
typedef union MATRIX3x3
{
	struct { Vector3 right; Vector3 up; Vector3 forward; };
	struct { Vector3 vectors[3]; };
}Rotation, Matrix3x3;


////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------//
//------------------------------函数声明-------------------------------//
//----------------inline函数声明，static防止不内联时重定义----------------//
////////////////////////////////////////////////////////////////////////

/*----------------------------fp32函数声明------------------------------*/

static inline fp32	fp32_int(int n);
//static inline fp32	fp32_float(float f);
static inline fp32	fp32_100f(int n);
static inline fp32	fp32_fp16(fp16 f16);
static inline int		int_fp32(fp32);
//static inline fp32	fp32_add(fp32 a, fp32 b);
//static inline fp32	fp32_sub(fp32 a, fp32 b);
//static inline fp32	fp32_mul(fp32 a, fp32 b);
//static inline fp32	fp32_div(fp32 a, fp32 b);
static inline fp32	fp32_mul32(fp32 a, fp32 b);
static inline fp32	fp32_div32(fp32 a, fp32 b);
static inline fp32	fp32_mul64(fp32 a, fp32 b);
static inline fp32	fp32_div64(fp32 a, fp32 b);
static inline fp32	fp32_lerp(fp32 a, fp32 b, fp32 t);
static inline fp32	fp32_pow2(fp32 a);
static inline u64		fp32_pow2_64(fp32 a);
//static inline fp32	fp32_sqrt(fp32 n);

/*----------------------------fp16函数声明------------------------------*/

static inline fp16	fp16_int(int n);
static inline fp16	fp16_100f(int n);
//static inline fp16	fp16_float(float f);
static inline fp16	fp16_fp32(fp32 f32);
static inline fp16	fp16_mul32(fp16 a, fp16 b);
static inline fp16	fp16_div32(fp16 a, fp16 b);

/*----------------------------xyz32函数声明------------------------------*/

static inline xyz *xyz_set(xyz *vc, fp32 x, fp32 y, fp32 z);
static inline xyz *xyz_setint(xyz *vc, int x, int y, int z);
static inline xyz *xyz_add(xyz *vc, xyz *a, xyz *b);
static inline xyz *xyz_sub(xyz *vc, xyz *a, xyz *b);
static inline xyz *xyz_neg(xyz *vc, xyz *a);
static inline xyz *xyz_scale(xyz *vc, xyz *a, fp32 n);
static inline xyz *xyz_scale_ms(xyz *vc, fp32 n);
static inline xyz *xyz_lerp(xyz *vc, xyz *a, xyz *b, fp32 t);
static inline fp32 xyz_dot(xyz *a, xyz *b);
static inline fp16 xyz_dot16(xyz *a, xyz *b);
static inline xyz *xyz_cross(xyz *vc, xyz *a, xyz *b);
static inline fp32 xyz_len2(xyz *c);
static inline u64	 xyz_len2_64(xyz *c);
static inline fp32 xyz_len(xyz *c);
static inline xyz *xyz_normalize(xyz *vc);
static inline xyz *xyz_reflect(xyz *vOut, xyz *vi, xyz *vn);
static inline Vector3 *xyz_rotateX(Vector3 *vc, fp32 x);
static inline Vector3 *xyz_rotateY(Vector3 *vc, fp32 y);
static inline Vector3 *xyz_rotateZ(Vector3 *vc, fp32 z);
Vector3 *xyz_rotate(Vector3 *vc, fp32 x, fp32 y, fp32 z);

/*----------------------------3*xyz32矩阵------------------------------*/

Rotation *InitRotation(Rotation *rot);
Vector3 *TransformToWorld(Rotation *rot, Vector3 *v, Vector3 *vec_out);
Vector3 *TransformToLocal(Rotation *rot, Vector3 *v, Vector3 *v_out);
static inline void TransformPointToWorld(Matrix3x3 *matrix, Vector3 *origin, Vector3 *pos_in, Vector3 *pos_out);
static inline void TransformPointToLocal(Matrix3x3 *matrix, Vector3 *origin, Vector3 *pos_in, Vector3 *pos_out);

static inline void TransformMatrixToWorld(Matrix3x3 *matrix, Matrix3x3 *m_in, Matrix3x3 *m_out);
static inline void TransformMatrixToLocal(Matrix3x3 *matrix, Matrix3x3 *m_in, Matrix3x3 *m_out);

Rotation *SetRotationForward(Rotation *rot, Vector3 *forward);
//***多次旋转基坐标系会导致极大的误差***
Rotation *Rotate(Rotation *rot, fp32 x, fp32 y, fp32 z);

/*-----------------------------基础数学运算-----------------------------*/

u32 int_sqrt(u32 x);
int u64_sqrt(u64 n);
fp32 fp32_sqrt64(u64 n);
fp32 fp32_sqrt32(fp32 n);
fp32 fp32_sqrt64_nt(fp32 y);

//**三角函数表**
#define SIN_TABLE_FBIT	20				//sin表中定点数的小数精度
#define ASIN_TABLE_FBIT	30				//asin表中定点数的小数精度
extern const fp32 sin_table[1025];		//sin[0,1025] = [0,PI/2]
extern const fp32 asin_table[1025];		//asin[0,1025] = [0,PI/2]

//[0,2π] = [0-4] -> [-1,1]
fp32 sin_ht(fp32 x);
//[0,2π] = [0-4] -> [-1,1]
fp32 cos_ht(fp32 x);
//[-1,1] -> [-1,1] = [-π/2,π/2]
fp32 asin_ht(fp32 x);
//[-1,1] -> [2, 0] = [π,0]
fp32 acos_ht(fp32 x);

static inline u32 RSA_simple(u32 n, u32 p, u32 q);


////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------//
//------------------------------函数定义-------------------------------//
//--------------------------------------------------------------------//
////////////////////////////////////////////////////////////////////////

/*-----------------------------fp32函数定义-----------------------------*/
//int -> fp32
static inline fp32	fp32_int(int n) { return n << FP32_FBIT; }
//float -> fp32
//static inline fp32	fp32_float(float f) { return (fp32)(f * (1 << FP32_FBIT)); }
//int/100 -> fp32
static inline fp32	fp32_100f(int n) { return (((s64)n << FP32_FBIT) + 50) / 100; }
//fp32 -> int
static inline int		int_fp32(fp32 f) { return f >> FP32_FBIT; }

//fp32 + fp32 **事实上没有用的必要
//static inline fp32	fp32_add(fp32 a, fp32 b) { return a + b; }
//fp32 - fp32 **事实上没有用的必要
//static inline fp32	fp32_sub(fp32 a, fp32 b) { return a - b; }
//fp32 * fp32 **不应采用**
//**几何平均值大于0.15625 / 45.25会溢出
//static inline fp32	fp32_mul(fp32 a, fp32 b) { return (a * b) >> FP32_FLOATBIT; }
//fp32 / fp32 **不应采用**
//a > 0.0039会溢出 ** b < 1也可能溢出
//static inline fp32	fp32_div(fp32 a, fp32 b) { return ((a << FP32_FLOATBIT) / b); }

//fp32 * fp32 (32位低精度运算) (a * b < 1024)
//e_max = 2*fp32_max*2^(-f_bit/2) + 2^(-f_bit)
//a = b = 4 : e_r = 0.04%
//a = b = 1 : e_r = 0.195%
static inline fp32	fp32_mul32(fp32 a, fp32 b)
{ return (a >> ((FP32_FBIT & 1) + FP32_FBIT / 2)) *(b >> (FP32_FBIT / 2)); }

//fp32 / fp32 (32位低精度运算) (|a| < 2, |b| > 0.001, 范围内不溢出)
static inline fp32	fp32_div32(fp32 a, fp32 b)
{ return (a << ((FP32_FBIT & 1) + FP32_FBIT / 2)) / (b >> (FP32_FBIT / 2)); }

//fp32 * fp32 (64位安全运算)
static inline fp32	fp32_mul64(fp32 a, fp32 b)
{ return (((s64)a) * b) >> FP32_FBIT; }

//fp32 / fp32 (64位安全运算) *b<1仍可能溢出
static inline fp32	fp32_div64(fp32 a, fp32 b)
{ return (((s64)a) << FP32_FBIT) / b; }

//fp32^2
static inline fp32	fp32_pow2(fp32 a)
{ return (((s64)a) * a) >> FP32_FBIT; }

static inline u64	fp32_pow2_64(fp32 a)
{ return (((s64)a) * a) >> FP32_FBIT; }

static inline fp32	fp32_lerp(fp32 a, fp32 b, fp32 t)
{ return a + fp32_mul64(b - a, t); }

/*------------------------------fp16函数定义-----------------------------*/
//int -> fp16
static inline fp16	fp16_int(int n) { return n << FP16_FBIT; }
//int/100 -> fp16   **n/100 > 31 会溢出
static inline fp16	fp16_100f(int n) { return (((s32)n << FP16_FBIT) + 50) / 100; }
//float -> fp16
//static inline fp16	fp16_float(float f) { return (fp16)(f * (1 << FP16_FBIT)); }
//fp32 -> fp16   **fp32 > 31 会溢出
static inline fp16	fp16_fp32(fp32 f32) { return f32 >> (FP32_FBIT - FP16_FBIT); }
//fp16 -> fp32
static inline fp32	fp32_fp16(fp16 f16) { return f16 << (FP32_FBIT - FP16_FBIT); }
//fp16 * fp16 (32位安全运算)
static inline fp16	fp16_mul32(fp16 a, fp16 b) { return(((s32)a) * b) >> FP16_FBIT; }
//fp16 / fp16 (32位安全运算)
static inline fp16	fp16_div32(fp16 a, fp16 b) { return(((s32)a) << FP16_FBIT) / b; }


/*------------------------------xyz32函数定义----------------------------*/
//设置矢量
static inline xyz *xyz_set(xyz *vc, fp32 x, fp32 y, fp32 z)
{ vc->x = x; vc->y = y; vc->z = z; return vc; }
//使用int设置矢量
static inline xyz *xyz_setint(xyz *vc, int x, int y, int z)
{ vc->x = fp32_int(x); vc->y = fp32_int(y); vc->z = fp32_int(z); return vc; }
//vc = a + b
static inline xyz *xyz_add(xyz *vc, xyz *a, xyz *b)
{ vc->x = a->x + b->x; vc->y = a->y + b->y; vc->z = a->z + b->z; return vc; }
//vc = a - b
static inline xyz *xyz_sub(xyz *vc, xyz *a, xyz *b)
{ vc->x = a->x - b->x; vc->y = a->y - b->y; vc->z = a->z - b->z; return vc; }
//vc = -a
static inline xyz *xyz_neg(xyz *vc, xyz *a)
{ vc->x = -a->x; vc->y = -a->y; vc->z = -a->z; return vc; }
//vc = (xyz) a * (fp32) n
static inline xyz *xyz_scale(xyz *vc, xyz *a, fp32 n)
{
	vc->x = fp32_mul64(a->x, n);
	vc->y = fp32_mul64(a->y, n);
	vc->z = fp32_mul64(a->z, n);
	return vc;
}
//vc *= (fp32)n
static inline xyz *xyz_scale_ms(xyz *vc, fp32 n)
{
	vc->x = fp32_mul64(vc->x, n);
	vc->y = fp32_mul64(vc->y, n);
	vc->z = fp32_mul64(vc->z, n);
	return vc;
}

static inline xyz *xyz_lerp(xyz *vc, xyz *a, xyz *b, fp32 t)
{
	vc->x = a->x + fp32_mul64(b->x - a->x, t);
	vc->y = a->y + fp32_mul64(b->y - a->y, t);
	vc->z = a->z + fp32_mul64(b->z - a->z, t);
	return vc;
}

//a · b  矢量点乘
static inline fp32 xyz_dot(xyz *a, xyz *b)
{ return ((s64)a->x * b->x + (s64)a->y * b->y + (s64)a->z * b->z) >> FP32_FBIT; }
//{ return fp32_mul64(a->x, b->x) + fp32_mul64(a->y, b->y) + fp32_mul64(a->z, b->z); }

//a · b  矢量点乘 (低精度，结果为fp16)
static inline fp16 xyz_dot16(xyz *a, xyz *b)
{ return fp16_fp32(fp32_mul32(a->x, b->x) + fp32_mul32(a->y, b->y) + fp32_mul32(a->z, b->z)); }

//vc = a x b  矢量叉乘 **注意结果按 左手坐标系 规则
static inline xyz *xyz_cross(xyz *vc, xyz *a, xyz *b)
{
	vc->x = ((s64)a->y * b->z - (s64)a->z * b->y) >> FP32_FBIT;
	vc->y = (-(s64)a->x * b->z + (s64)a->z * b->x) >> FP32_FBIT;
	vc->z = ((s64)a->x * b->y - (s64)a->y * b->x) >> FP32_FBIT;
	//vc->x = fp32_mul64(a->y, b->z) - fp32_mul64(a->z, b->y);
	//vc->y = -fp32_mul64(a->x, b->z) + fp32_mul64(a->z, b->x);
	//vc->z = fp32_mul64(a->x, b->y) - fp32_mul64(a->y, b->x);
	return vc;
}

//c的模长的平方 (32位运算)
static inline fp32 xyz_len2(xyz *c)
{ return fp32_pow2(c->x) + fp32_pow2(c->y) + fp32_pow2(c->z); }

//c的模长的平方 (返回64位)
static inline u64 xyz_len2_64(xyz *c)
{ return fp32_pow2_64(c->x) + fp32_pow2_64(c->y) + fp32_pow2_64(c->z); }

//c的模长
static inline fp32 xyz_len(xyz *c)
{ return u64_sqrt((s64)c->x * c->x + (s64)c->y * c->y + (s64)c->z * c->z); }

//向量归一化
static inline xyz *xyz_normalize(xyz *vc)
{
	fp32 len = xyz_len(vc);
	vc->x = fp32_div64(vc->x, len);
	vc->y = fp32_div64(vc->y, len);
	vc->z = fp32_div64(vc->z, len);
	return vc;
}
//反射向量
static inline xyz *xyz_reflect(xyz *v_out, xyz *vi, xyz *vn)
{
	fp32 dr2 = xyz_dot(vn, vi) * 2;
	v_out->x = vi->x - fp32_mul64(dr2, vn->x);
	v_out->y = vi->y - fp32_mul64(dr2, vn->y);
	v_out->z = vi->z - fp32_mul64(dr2, vn->z);
	return v_out;
}
//折射向量**
static inline xyz *xyz32_refract(xyz *v_out, xyz *vi, xyz *vn, fp32 g1, fp32 g2)
{
	fp32 nlt = fp32_div64(g1, g2);
	fp32 dot = xyz_dot(vi, vn);
	fp32 q2 = FP32_1 - fp32_mul64(fp32_pow2(nlt), (FP32_1 - fp32_pow2(dot)));
	if (q2 < 0)
	{
		//全反射
		dot *= 2;
		v_out->x = vi->x - fp32_mul64(dot, vn->x);
		v_out->y = vi->y - fp32_mul64(dot, vn->y);
		v_out->z = vi->z - fp32_mul64(dot, vn->z);
	}
	else
	{
		q2 = fp32_mul64(nlt, dot) - fp32_sqrt64(q2);
		v_out->x = fp32_mul64(q2, vn->x) + fp32_mul64(nlt, vi->x);
		v_out->y = fp32_mul64(q2, vn->y) + fp32_mul64(nlt, vi->y);
		v_out->z = fp32_mul64(q2, vn->z) + fp32_mul64(nlt, vi->z);
	}
	return v_out;
}

static inline Vector3 *xyz_rotateX(Vector3 *vc, fp32 x)
{
	fp32 sin = sin_ht(x);
	fp32 cos = cos_ht(x);
	vc->y = fp32_mul64(vc->y, cos) - fp32_mul64(vc->z, sin);
	vc->z = fp32_mul64(vc->y, sin) + fp32_mul64(vc->z, cos);
	return vc;
}

static inline Vector3 *xyz_rotateY(Vector3 *vc, fp32 y)
{
	fp32 sin = sin_ht(y);
	fp32 cos = cos_ht(y);
	vc->x = fp32_mul64(vc->x, cos) - fp32_mul64(vc->z, sin);
	vc->z = fp32_mul64(vc->x, sin) + fp32_mul64(vc->z, cos);
	return vc;
}

static inline Vector3 *xyz_rotateZ(Vector3 *vc, fp32 z)
{
	fp32 sin = sin_ht(z);
	fp32 cos = cos_ht(z);
	vc->x = fp32_mul64(vc->x, cos) - fp32_mul64(vc->y, sin);
	vc->y = fp32_mul64(vc->x, sin) + fp32_mul64(vc->y, cos);
	return vc;
}

/*----------------------------3*xyz32矩阵函数定义-----------------------------*/

static inline void TransformPointToWorld(Matrix3x3 *matrix, Vector3 *origin, Vector3 *pos_in, Vector3 *pos_out)
{
	Vector3 vector;
	xyz_add(pos_out, origin, TransformToWorld(matrix, pos_in, &vector));
}

static inline void TransformPointToLocal(Matrix3x3 *matrix, Vector3 *origin, Vector3 *pos_in, Vector3 *pos_out)
{
	Vector3 vector;
	TransformToLocal(matrix, xyz_sub(&vector, pos_in, origin), pos_out);
}

static inline void TransformMatrixToWorld(Matrix3x3 *matrix, Matrix3x3 *m_in, Matrix3x3 *m_out)
{
	TransformToWorld(matrix, &m_in->right, &m_out->right);
	TransformToWorld(matrix, &m_in->up, &m_out->up);
	TransformToWorld(matrix, &m_in->forward, &m_out->forward);
}

static inline void TransformMatrixToLocal(Matrix3x3 *matrix, Matrix3x3 *m_in, Matrix3x3 *m_out)
{
	TransformToLocal(matrix, &m_in->right, &m_out->right);
	TransformToLocal(matrix, &m_in->up, &m_out->up);
	TransformToLocal(matrix, &m_in->forward, &m_out->forward);
}


//其他非内联函数在.c文件中

/*-----------------------------基础数学函数定义------------------------------*/

static inline fp32 len_xy(fp32 x, fp32 y)
{
	return u64_sqrt((u64)x * x + (u64)y * y);
}

static inline u32 RSA_simple(u32 n, u32 p, u32 q)
{
	//return ((u64)n * n) % (p * q);
	return ((u64)n * n) % (p * q);
}

static u32 rdCount = 1;
static inline u32 Random()
{
	rdCount = ((rdCount + 20222019) * 16807L) % (((u32)1 << 31) - 1);
	return rdCount;
}

static inline u32 Random_MK2()
{
	rdCount = rdCount + 301;
	return rdCount;
}
