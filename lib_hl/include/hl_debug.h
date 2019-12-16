#pragma once

#include <hl_system.h>
#include <hl_gba.h>
#include <hl_graphics.h>
#include <hl_math.h>

extern const u8 font_8x8[760];

//��16��������ӡת��Ϊ�ַ���char[9] (����8��'\0'1)
char *hexToString(u32 n, char str[9]);
//�з�������(s32)ת��Ϊ�ַ���char[12] (����1������10��'\0'1)
char *intToString(int n, char str[12]);
//��fp32ת�����ַ����������ţ�
char *fp32ToString(fp32 fp, char str[13]);


void PrintTextInMode3(char *str, int pos_x, int pos_y, u16 color);
void PrintHexInMode3(u32 n, int x, int y);
void PrintIntInMode3(int n, int x, int y);
void PrintFP32InMode3(fp32 fp, int x, int y);
void PrintXYZ32InMode3(Vector3 *vec, int x, int y);

