#pragma once

typedef signed	char	s8;		//8位正负整数
typedef signed	short	s16;	//16位正负整数
typedef signed	int		s32;	//32位正负整数
typedef signed	long long s64;//64位正负整数

typedef unsigned char		u8;		//8位正整数
typedef unsigned short	u16;	//16位正整数
typedef unsigned int		u32;	//32位正整数
typedef unsigned long long u64;//64位正整数

typedef volatile signed		char	vs8;	//8位正负整数
typedef volatile signed		short	vs16;	//16位正负整数
typedef volatile signed		int		vs32;	//32位正负整数

typedef volatile unsigned	char	vu8;	//8位正整数
typedef volatile unsigned	short	vu16;	//16位正整数
typedef volatile unsigned	int		vu32;	//32位正整数

typedef int Bool;


#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#define BIT(n)	(1<<(n))		//第n比特为1 (2^n)

#define SETFLAG(v,flag)	(v|flag)		//设定Flag
#define HASFLAG(v,flag)	(v&flag)		//是否有Flag
#define NOFLAG(v,flag)	((v&flag)==0)		//没有Flag

#define HASFLAGS(v,flags)	((v&(flags))==(flags))	//Flags都有


#define MethodDefine(ReturnType, Name, ...)				\
typedef ReturnType (*Name##FC) (__VA_ARGS__);				\
static inline ReturnType Name##_Default(__VA_ARGS__) {}



//定义一个方法 (FC)
//FC: void Method01(T1 a, T2 b);
//#define RETURN_Method01 void
//#define ARGS_Method01 (T1 a, T2 b)

//实现FC方法 (Name)
//RETURN_FC FC_Name ARGS_FC

//创建FC方法指针 (Name)
//RETURN_FC (*Name) ARGS_FC

//创建FC类型的方法指针的 变量(Name)
#define FC_PTR(FC, Name) RETURN_##FC (*Name) ARGS_##FC

//定义FC类型的方法指针的 函数(Name)
#define FC_DEF(FC, Name) RETURN_##FC FC##_##Name ARGS_##FC
