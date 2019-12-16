#pragma once

typedef signed	char	s8;		//8λ��������
typedef signed	short	s16;	//16λ��������
typedef signed	int		s32;	//32λ��������
typedef signed	long long s64;//64λ��������

typedef unsigned char		u8;		//8λ������
typedef unsigned short	u16;	//16λ������
typedef unsigned int		u32;	//32λ������
typedef unsigned long long u64;//64λ������

typedef volatile signed		char	vs8;	//8λ��������
typedef volatile signed		short	vs16;	//16λ��������
typedef volatile signed		int		vs32;	//32λ��������

typedef volatile unsigned	char	vu8;	//8λ������
typedef volatile unsigned	short	vu16;	//16λ������
typedef volatile unsigned	int		vu32;	//32λ������

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

#define BIT(n)	(1<<(n))		//��n����Ϊ1 (2^n)

#define SETFLAG(v,flag)	(v|flag)		//�趨Flag
#define HASFLAG(v,flag)	(v&flag)		//�Ƿ���Flag
#define NOFLAG(v,flag)	((v&flag)==0)		//û��Flag

#define HASFLAGS(v,flags)	((v&(flags))==(flags))	//Flags����


#define MethodDefine(ReturnType, Name, ...)				\
typedef ReturnType (*Name##FC) (__VA_ARGS__);				\
static inline ReturnType Name##_Default(__VA_ARGS__) {}



//����һ������ (FC)
//FC: void Method01(T1 a, T2 b);
//#define RETURN_Method01 void
//#define ARGS_Method01 (T1 a, T2 b)

//ʵ��FC���� (Name)
//RETURN_FC FC_Name ARGS_FC

//����FC����ָ�� (Name)
//RETURN_FC (*Name) ARGS_FC

//����FC���͵ķ���ָ��� ����(Name)
#define FC_PTR(FC, Name) RETURN_##FC (*Name) ARGS_##FC

//����FC���͵ķ���ָ��� ����(Name)
#define FC_DEF(FC, Name) RETURN_##FC FC##_##Name ARGS_##FC
