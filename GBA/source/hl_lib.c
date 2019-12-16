#include <hl_math.h>
#include <hl_graphics.h>
#include <hl_debug.h>

//const Color _color_black = { 0, 0, 0 };
//const Color _color_white = { FP16_1, FP16_1, FP16_1 };

//整数开方
u32 int_sqrt(u32 x)
{
	u32 y;
	//初始估计值
	if (x > 0x4000000) y = 12288;
	else if (x > 0x1000000) y = 6144;
	else if (x > 0x400000) y = 3072;
	else if (x > 0x100000) y = 1536;
	else if (x > 0x40000) y = 768;
	else if (x > 0x10000) y = 384;
	else if (x > 0x4000) y = 192;
	else if (x > 0x1000) y = 96;
	else if (x > 0x400) y = 48;
	else if (x > 0x100) y = 24;
	else if (x > 0x40) y = 12;
	else y = 6;
	y = (y + x / y) >> 1;
	y = (y + x / y) >> 1;
	return y;
}

int u64_sqrt(u64 n)
{
	int sqrt = 0;
	int shift = 31;
	u64 sqrt2;
	while (shift >= 0)
	{
		sqrt2 = ((u64)(sqrt << 1) + (1 << shift)) << shift;
		if (sqrt2 <= n)
		{
			sqrt += (1 << shift);
			n -= sqrt2;
		}
		shift -= 1;
	}
	return sqrt;
}

//定点数准确开方
fp32 fp32_sqrt64(u64 fn)
{
	fn <<= FP32_FBIT;
	int sqrt = 0;
	int shift = (32 + FP32_FBIT) / 2;
	u64 sqrt2;
	while (shift >= 0)
	{
		//sqrt2 = (sqrt + nbit) * (sqrt + nbit) - sqrt * sqrt;
		sqrt2 = ((u64)(sqrt << 1) + (1 << shift)) << shift;
		if (sqrt2 <= fn)
		{
			sqrt += (1 << shift);
			fn -= sqrt2;
		}
		shift -= 1;
	}
	return sqrt;
}

//定点数低精度开方
fp32 fp32_sqrt32(fp32 n)
{
	int sqrt = 0;
	int shift = 15;
	int sqrt2;
	while (shift >= 0)
	{
		sqrt2 = ((sqrt << 1) + (1 << shift)) << shift;
		if (sqrt2 <= n)
		{
			sqrt += (1 << shift);
			n -= sqrt2;
		}
		shift--;
	}
	return sqrt << (FP32_FBIT / 2);
}

//定点数(可能)准确开方
fp32 fp32_sqrt64_nt(fp32 y)
{
	u64 x = ((u64)y) << FP32_FBIT;
	//u32 y;
	//初始估计值
	if (y > 0x10000000) y = 24576 << (FP32_FBIT / 2);
	else if (y > 0x4000000) y = 12288 << (FP32_FBIT / 2);
	else if (y > 0x1000000) y = 6144 << (FP32_FBIT / 2);
	else if (y > 0x400000) y = 3072 << (FP32_FBIT / 2);
	else if (y > 0x100000) y = 1536 << (FP32_FBIT / 2);
	else if (y > 0x40000) y = 768 << (FP32_FBIT / 2);
	else if (y > 0x10000) y = 384 << (FP32_FBIT / 2);
	else y = 192 << (FP32_FBIT / 2);
	y = (y + x / y) >> 1;
	y = (y + x / y) >> 1;
	y = (y + x / y) >> 1;
	return y;
}


#define _SIN_FBIT_SHIFT (SIN_TABLE_FBIT - FP32_FBIT)

//sin [0,4]->[-1,1]
//x: 1 = PI/2
fp32 sin_ht(fp32 x)
{
	x = (x >> (FP32_FBIT - 10)) % 4096;
	if (x < 0) x += 4096;
	if (x < 1024) return sin_table[x] >> _SIN_FBIT_SHIFT;			//[0-1023] -> [0-1023]
	if (x < 2048) return  sin_table[2048 - x] >> _SIN_FBIT_SHIFT;	//[1024-2047] -> [1024-1]
	if (x < 3072) return -sin_table[x - 2048] >> _SIN_FBIT_SHIFT;	//[2048-3071] -> [0-1023]	
	return -sin_table[4096 - x] >> _SIN_FBIT_SHIFT;					//[3072-4095] -> [1024-1]
}

fp32 cos_ht(fp32 x)
{
	x = (x >> (FP32_FBIT - 10)) % 4096;
	if (x < 0) x = -x;
	if (x < 1024) return  sin_table[1024 - x] >> _SIN_FBIT_SHIFT;
	if (x < 2048) return -sin_table[x - 1024] >> _SIN_FBIT_SHIFT;
	if (x < 3072) return -sin_table[3072 - x] >> _SIN_FBIT_SHIFT;
	return sin_table[x - 3072] >> _SIN_FBIT_SHIFT;
}

#define _ASIN_FBIT_SHIFT (ASIN_TABLE_FBIT - FP32_FBIT)

//Asin [-1,1]-> [-1,1]
//y: 1 = PI/2
fp32 asin_ht(fp32 x)
{
	x = x >> (FP32_FBIT - 10);
	if (x < -1024 || x > 1024) return 0;
	if (x < 0) return -asin_table[-x] >> _ASIN_FBIT_SHIFT;
	else return asin_table[x] >> _ASIN_FBIT_SHIFT;
}

//Acos [-1,1]-> [2,0]
fp32 acos_ht(fp32 x)
{
	x = x >> (FP32_FBIT - 10);
	if (x < -1024 || x > 1024) return 0;
	if (x < 0) return FP32_1 + (asin_table[-x] >> _ASIN_FBIT_SHIFT);
	else return FP32_1 - (asin_table[x] >> _ASIN_FBIT_SHIFT);
}


//=====================================Ratation====================================//

//初始化为标志正交基底
Rotation *InitRotation(Rotation *rot)
{
	xyz_set(&rot->right, FP32_1, 0, 0);
	xyz_set(&rot->up, 0, FP32_1, 0);
	xyz_set(&rot->forward, 0, 0, FP32_1);
	return rot;
}

//局部坐标转换到世界坐标
//空间线性变换，v和vec_out不可相同
Vector3 *TransformToWorld(Rotation *rot, Vector3 *v, Vector3 *vec_out)
{
	vec_out->x = (((s64)v->x) * rot->right.x + ((s64)v->y) * rot->up.x + ((s64)v->z) * rot->forward.x) >> FP32_FBIT;
	vec_out->y = (((s64)v->x) * rot->right.y + ((s64)v->y) * rot->up.y + ((s64)v->z) * rot->forward.y) >> FP32_FBIT;
	vec_out->z = (((s64)v->x) * rot->right.z + ((s64)v->y) * rot->up.z + ((s64)v->z) * rot->forward.z) >> FP32_FBIT;
	//vec_out->x = fp32_mul64(v->x, rot->right.x) + fp32_mul64(v->y, rot->up.x) + fp32_mul64(v->z, rot->forward.x);
	//vec_out->y = fp32_mul64(v->x, rot->right.y) + fp32_mul64(v->y, rot->up.y) + fp32_mul64(v->z, rot->forward.y);
	//vec_out->z = fp32_mul64(v->x, rot->right.z) + fp32_mul64(v->y, rot->up.z) + fp32_mul64(v->z, rot->forward.z);
	return vec_out;
}

Vector3 *TransformToLocal(Rotation *rot, Vector3 *v, Vector3 *vec_out)
{
	vec_out->x = xyz_dot(v, &rot->right);
	vec_out->y = xyz_dot(v, &rot->up);
	vec_out->z = xyz_dot(v, &rot->forward);
	return vec_out;
}

//以forword(单位向量)设定rotation
Rotation *SetRotationForward(Rotation *rot, Vector3 *forward)
{
	rot->forward = *forward;
	fp32 xxzz = fp32_sqrt64(fp32_pow2(forward->x) + fp32_pow2(forward->z));
	rot->up.x = ((s64)-forward->y) * forward->x / xxzz;
	rot->up.y = xxzz;
	rot->up.z = ((s64)-forward->y) * forward->z / xxzz;
	//左手坐标系叉乘
	xyz_cross(&rot->right, &rot->up, &rot->forward);
	return rot;
}

Vector3 *xyz_rotate(Vector3 *vc, fp32 x, fp32 y, fp32 z)
{
	xyz_rotateX(vc, x);
	xyz_rotateY(vc, y);
	xyz_rotateZ(vc, z);
	return vc;
}

//***多次旋转基坐标系会导致极大的误差***
Rotation *Rotate(Rotation *rot, fp32 x, fp32 y, fp32 z)
{
	xyz_normalize(xyz_rotate(&rot->right, x, y, z));
	xyz_normalize(xyz_rotate(&rot->up, x, y, z));
	xyz_cross(&rot->forward, &rot->right, &rot->up);
	xyz_cross(&rot->right, &rot->up, &rot->forward);
	return rot;
}



/*============================Graphics==========================*/

void DrawRectangle(u8 size_x, u8 size_y, u8 pos_x, u8 pos_y, rgb5 color)
{
	//u16 *vram = (u16 *)VRAM;	//地址
	//u16 vram = *(u16 *)VRAM		//值
	if (size_x > 240) size_x = 240;
	if (size_y > 160) size_y = 160;
	for (int y = 0; y < size_x; y++)
	{
		for (int x = 0; x < size_y; x++)
		{
			*(u16 *)(VRAM + ((240 * (pos_y + y) + pos_x + x) << 1)) = color;
			//((u16 *)VRAM)[240 * (pos_y + y) + pos_x + x] = color;
		}
	}
}

void DrawLine(int start_x, int start_y, int end_x, int end_y, rgb5 color)
{
	int x, y, rt;
	x = end_x - start_x;
	y = end_y - start_y;

	if ((x > 0 ? x : -x) > (y > 0 ? y : -y))
	{
		if (end_x < start_x)
		{
			rt = start_x;
			start_x = end_x;
			end_x = rt;
			rt = start_y;
			start_y = end_y;
			end_y = rt;
		}
		//x
		rt = (y << 16) / x;
		for (x = start_x; x <= end_x; x++)
		{
			y = start_y + (((x - start_x) * rt) >> 16);
			if (x<0 || x>SCREEN_X - 1) continue;
			if (y<0 || y>SCREEN_Y - 1) continue;
			SetPixel3(x, y, color);
		}
	}
	else
	{
		if (end_y < start_y)
		{
			rt = start_x;
			start_x = end_x;
			end_x = rt;
			rt = start_y;
			start_y = end_y;
			end_y = rt;
		}
		//y
		rt = (x << 16) / y;
		for (y = start_y; y <= end_y; y++)
		{
			x = start_x + (((y - start_y) * rt) >> 16);
			if (x<0 || x>SCREEN_X - 1) continue;
			if (y<0 || y>SCREEN_Y - 1) continue;
			SetPixel3(x, y, color);
		}
	}

}

//清除屏幕（MODE3）
void ClearScreen()
{
	for (int i = 0; i < 76800; i += 4)
	{
		*(u32 *)(VRAM + i) = 0;
	}
}

//绘制有两条轴线的背景
void DrawAxisBackground(int x, int y)
{
	if (y == SCREEN_Y / 2) SetPixel3(x, y, RGB5_RED);
	else if (x == SCREEN_X / 2)	SetPixel3(x, y, RGB5_GREEN);
	else SetPixel3(x, y, RGB5((y >> 4) + 11, (y >> 4) + 11, 31));
}

//t : fp32 [0,2^20)
rgb5 rgb5_Lerp(rgb5 a, rgb5 b, fp32 t)
{
	return  a + (((((b >> 10) - (a >> 10)) *t) >> FP32_FBIT) << 10) +
		(((((b & RGB5_GREEN) - (a & RGB5_GREEN)) * t) >> FP32_FBIT) &RGB5_GREEN) +
		((((b & RGB5_RED) - (a & RGB5_RED)) * t) >> FP32_FBIT);
}

//t : fp16 [0,1023]
rgb10 rgb10_Lerp(rgb10 a, rgb10 b, fp16 t)
{
	return a + (((((b >> 20) - (a >> 20)) *t) >> FP16_FBIT) << 20) +
		(((((b & RGB10_GREEN) - (a & RGB10_GREEN)) * t) >> FP16_FBIT) &RGB10_GREEN) +
		((((b & RGB10_RED) - (a & RGB10_RED)) * t) >> FP16_FBIT);

}



/*=============================Debug===========================*/

//将16进制数打印转换为字符串char[9] (数字8，'\0'1)
char *hexToString(u32 n, char str[9])
{
	int x;
	for (int i = 7; i >= 0; i--)
	{
		x = n & 0b1111;
		str[i] = x + (x < 10 ? 48 : 55);
		n >>= 4;
	}
	str[8] = '\0';
	return str;
}


//有符号整数(s32)转化为字符串char[12] (符号1，数字10，'\0'1)
char *intToString(int n, char str[12])
{
	u8 p = 0;	//前半部分p为整数正负，后半部分表示第p位数字
	if (n < 0) { p = 1; str[0] = '-'; n = -n; }
	u8 i = 10;
	while (i > 0)
	{
		str[i + p - 1] = n % 10 + 48;
		n = n / 10;
		i--;
		//当余下数字都为零，将之前的数字移动到数组头部
		if (n == 0)
		{
			u8 end = 10 - i + p;
			for (; p < end; p++) { str[p] = str[p + i]; }
			str[end] = '\0';		//结束符
			return str;
		}
	}
	return str;
}

//将fp32转换到字符串（带符号）
char *fp32ToString(fp32 fp, char str[14])
{
	if (fp < 0) { str[0] = '-'; fp = -fp; }
	else { str[0] = '+'; }
	//将以2进制表示的fp转换到10进制 **64位避免int_max/1000000溢出
	u64 n = (fp * 1000000ull + FP32_H5) >> FP32_FBIT;
	int i, k;
	for (i = 12; i > 0; i--)
	{
		//倒数第7个为小数点
		if (i == 6) { str[6] = '.'; continue; }
		str[i] = n % 10 + 48;
		n = n / 10;
		//若整数部分余下为0
		if (n == 0 && i < 6) break;
	}
	//将数字前移
	for (k = 1; k < 13 - i; k++) { str[k] = str[k + i - 1]; }
	str[14 - i] = '\0';
	return str;
}


void PrintTextInMode3(char *str, int pos_x, int pos_y, u16 color)
{
	if (pos_y > SCREEN_Y - 1) return;
	u16 *vram = ((u16 *)VRAM);
	int x, y;	//字符点阵内坐标
	int map_line;
	for (int i = 0; i < 256; i++)
	{
		char ch = *(str + i);	//字符编码
		if (ch == '\0') break;
		for (y = 0; y < 8; y++)
		{
			map_line = font_8x8[(ch - 32) * 8 + y];
			for (x = 0; x < 8; x++)
			{
				if ((map_line & (1 << (8 - x))) != 0)
				{
					vram[240 * (pos_y + y) + pos_x + i * 8 + x] = color;
				}
				else vram[240 * (pos_y + y) + pos_x + i * 8 + x] = 0;
			}
		}
	}
}


void PrintHexInMode3(u32 n, int x, int y)
{
	char str[9];
	hexToString(n, str);
	PrintTextInMode3(str, x, y, RGB5_WHITE);
}

void PrintIntInMode3(int n, int x, int y)
{
	char str[11];
	intToString(n, str);
	PrintTextInMode3(str, x, y, RGB5_WHITE);
}

void PrintFP32InMode3(fp32 fp, int x, int y)
{
	char str[14];
	fp32ToString(fp, str);
	PrintTextInMode3(str, x, y, RGB5_WHITE);
}

void PrintXYZ32InMode3(Vector3 *vec, int x, int y)
{
	char str[13];
	fp32ToString(vec->x, str);
	PrintTextInMode3(str, x, y, RGB5(31, 0, 0));
	fp32ToString(vec->y, str);
	PrintTextInMode3(str, x, y + 10, RGB5(0, 31, 0));
	fp32ToString(vec->z, str);
	PrintTextInMode3(str, x, y + 20, RGB5(0, 0, 31));
}
