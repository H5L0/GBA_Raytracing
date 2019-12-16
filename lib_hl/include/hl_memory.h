#pragma once
#include <hl_system.h>
#include <hl_gba.h>

//外部工作区 堆 开始地址（低）
//0x0200 0000
extern char __eheap_start;
//外部工作区 堆 结束地址（高）
extern char __eheap_end;

//内部工作区 静态变量 储存地址
//0x0300 0000
//*未初始化数据？
//从ROM拷贝来的？
extern char __bss_start__;
extern char __bss_end__;

//内部工作区 全局变量 储存地址
extern char __data_start__;
extern char __data_end__;

//内部工作区 堆 开始地址 （低）
extern char __iheap_start;
//没有 __iheap_end

//*可能是 栈 开始地址（高）
extern char __sp_usr;



#define NULL_HEAD_FLAG 0		//第一个元素的空标识
extern u32 iheap_end_addr;		//内部工作区 堆 结束地址 （值）

typedef struct AREAE AreaE;
typedef union AREAI AreaI;

int mem_heap_init();
void *mem_alloc_e(u32 size);
void *mem_alloc_i(u32 size);
int mem_free_e(void *addr);
int mem_free_i(void *addr);
void *mem_clean(void *start_adr, int clen);

void *mem_alloc_new(u32 size);		//自动变量分配，分配size+4字节的内存
void *mem_new(void *area, u32 size);


//EWRAM 内存分区 头部信息
typedef struct AREAE
{
	u32 size;			//区域大小(char)(包括头)
	AreaE *lastArea;
	AreaE *nextArea;
	int data[0];		//data[0]不占空间
}AreaE;

//IWRAM 内存分区 头部信息
typedef union AREAI
{
	u32 info;
	struct { u16 next; u16 end; };
}AreaI;



