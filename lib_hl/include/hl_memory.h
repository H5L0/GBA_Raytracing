#pragma once
#include <hl_system.h>
#include <hl_gba.h>

//�ⲿ������ �� ��ʼ��ַ���ͣ�
//0x0200 0000
extern char __eheap_start;
//�ⲿ������ �� ������ַ���ߣ�
extern char __eheap_end;

//�ڲ������� ��̬���� �����ַ
//0x0300 0000
//*δ��ʼ�����ݣ�
//��ROM�������ģ�
extern char __bss_start__;
extern char __bss_end__;

//�ڲ������� ȫ�ֱ��� �����ַ
extern char __data_start__;
extern char __data_end__;

//�ڲ������� �� ��ʼ��ַ ���ͣ�
extern char __iheap_start;
//û�� __iheap_end

//*������ ջ ��ʼ��ַ���ߣ�
extern char __sp_usr;



#define NULL_HEAD_FLAG 0		//��һ��Ԫ�صĿձ�ʶ
extern u32 iheap_end_addr;		//�ڲ������� �� ������ַ ��ֵ��

typedef struct AREAE AreaE;
typedef union AREAI AreaI;

int mem_heap_init();
void *mem_alloc_e(u32 size);
void *mem_alloc_i(u32 size);
int mem_free_e(void *addr);
int mem_free_i(void *addr);
void *mem_clean(void *start_adr, int clen);

void *mem_alloc_new(u32 size);		//�Զ��������䣬����size+4�ֽڵ��ڴ�
void *mem_new(void *area, u32 size);


//EWRAM �ڴ���� ͷ����Ϣ
typedef struct AREAE
{
	u32 size;			//�����С(char)(����ͷ)
	AreaE *lastArea;
	AreaE *nextArea;
	int data[0];		//data[0]��ռ�ռ�
}AreaE;

//IWRAM �ڴ���� ͷ����Ϣ
typedef union AREAI
{
	u32 info;
	struct { u16 next; u16 end; };
}AreaI;



