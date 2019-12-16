#ifndef WIN32

#include <hl_memory.h>
#include <hl_gba.h>

#include <hl_graphics.h>	//**测试


rgb5 _mem_fill_color = 0;
void DrawIWRAMAllocMap(u32 start, u32 size)
{
	//32KB,32*1024 Bytes = 128*128*2 Btyes
	//128行，每行256B，128像素显示 2Byte/pixel
	start = start - IWRAM;
	int end = start + size;
	start = start >> 1;
	end = end >> 1;
	_mem_fill_color += Random();
	int x, y;
	for (int i = start; i < end; i++)
	{
		y = i >> 7;
		x = i - (y << 7);
		SetPixel3(x, y, _mem_fill_color);
	}
}

void DrawIWRAM()
{
	DrawRectangle(129, 129, 0, 0, 31 << 10);
	for (u32 addr = 0; addr < IWRAM_SIZE; addr += 2)
	{
		rgb5 dataColor = *(rgb5 *)(addr + IWRAM);
		int y = addr >> 8;
		int x = (addr >> 1) - (y << 7);
		SetPixel3(x, y, dataColor);
	}
}


u32 iheap_end_addr;

int mem_heap_init()
{
	iheap_end_addr = IWRAM + IWRAM_SIZE - 0x2000;	//32-8-bss-data kB

	//eheap = (Heap *)& __eheap_start;
	AreaE *tae = (AreaE *)&__eheap_start;
	tae->size = NULL_HEAD_FLAG;
	tae->nextArea = NULL;
	tae->lastArea = NULL;
	AreaI *tai = (AreaI *)&__iheap_start;
	tai->next = NULL;
	tai->end = NULL_HEAD_FLAG;
	DrawIWRAM();
	return 0;
}


//在EWROM的堆中分配 size(字节) 内存
void *mem_alloc_e(u32 size)
{
	if (size == 0) return 0;
	size = ((size + sizeof(AreaE) + 3) >> 2) << 2;

	AreaE *ta = (AreaE *)&__eheap_start;
	while ((u32)&__eheap_end - (u32)ta - ta->size >= size)
	{
		//最后一个区域且空间充足 / 两区域间空间充足
		if (ta->nextArea == NULL || (u32)ta->nextArea - (u32)ta - ta->size >= size)
		{
			//第一个区域
			if (ta->size == NULL_HEAD_FLAG)
			{
				ta->size = size;
				return &ta->data;
			}
			else
			{
				AreaE *newa = (AreaE *)((u32)ta + ta->size);
				newa->lastArea = ta;
				newa->nextArea = ta->nextArea;
				newa->size = size;
				ta->nextArea = newa;
				return &newa->data;
			}
		}
		//现区域不是最后一个且间隔空间不足
		ta = ta->nextArea;
	}
	return 0;
}

//释放在EWROM分配的内存
int mem_free_e(void *addr)
{
	AreaE *ta = (AreaE *)((u32)addr - sizeof(AreaE));
	//if (ta < &__eheap_start || ta > &__eheap_end) return 0;
	if (ta->lastArea == NULL)
	{
		//第一个区域,不清除nextArea;
		mem_clean(addr, ta->size - sizeof(AreaE));
		ta->size = NULL_HEAD_FLAG;
		//保留了下一区域的上一区域记录
	}
	else
	{
		ta->lastArea->nextArea = ta->nextArea;
		if (ta->nextArea != NULL) ta->nextArea->lastArea = ta->lastArea;
		mem_clean((void *)ta, ta->size);
	}
	return addr;
}

//在IWRAM中分配 size(字节) 的空间
//0：分配失败
void *mem_alloc_i(u32 size)
{
	if (size == 0) return 0;
	size = ((size + sizeof(AreaI) + 3) >> 2) << 2;

	AreaI *ta = (AreaI *)&__iheap_start;
	while (iheap_end_addr - (ta->end + IWRAM) > size)
	{
		//最后一个区域且空间充足 / 两区域间空间充足
		if (ta->next == NULL || ta->next - ta->end >= size)
		{
			//第一个区域
			if (ta->end == NULL_HEAD_FLAG)
			{
				ta->end = (u32)ta - IWRAM + size;
				DrawIWRAMAllocMap((u32)ta, size);
				return (void *)((u32)ta + sizeof(AreaI));
			}
			else
			{
				AreaI *na = (AreaI *)(ta->end + IWRAM);
				na->next = ta->next;
				na->end = ta->end + size;
				ta->next = ta->end;
				DrawIWRAMAllocMap((u32)na, size);
				return (void *)((u32)na + sizeof(AreaI));
			}
		}
		//现区域不是最后一个且间隔空间不足
		ta = (AreaI *)(ta->next + IWRAM);
	}
	return 0;
}

//释放在IWRAM中分配的内存
//0：释放失败
int mem_free_i(void *addr)
{
	AreaI *ta = (AreaI *)((u32)addr - sizeof(AreaI));
	AreaI *ia = (AreaI *)&__iheap_start;
	AreaI *la;
	//第一个区域, 不清除next
	if (ta == ia)
	{
		mem_clean(addr, ta->end + IWRAM - (u32)ta - sizeof(AreaI));
		ta->end = NULL_HEAD_FLAG;
		return addr;
	}
	do
	{
		//从ia搜寻下一分区
		la = ia;
		ia = (AreaI *)(ia->next + IWRAM);
		//搜寻到ta，则la为上一分区
		if (ia == ta)
		{
			la->next = ta->next;
			mem_clean((void *)ta, ta->end + IWRAM - (u32)ta);
			return addr;
		}
	} while ((u32)ia < iheap_end_addr);

	return 0;	//搜寻越界,释放失败
}


//将adr~adr+len的内存置零 (char)
void *mem_clean(void *start_adr, int clen)
{
	int *adr = start_adr;
	int ilen = clen >> 2;
	clen -= ilen << 2;
	while ((ilen--) > 0) *(adr++) = 0;
	while ((ilen--) > 0) *((char *)adr++) = 0;
	return start_adr;
}


//分配size+4字节的内存，用于自动变量分配
void *mem_alloc_new(u32 size)
{
	//void *area = size > 0 ? mem_alloc_i(size + 4) : mem_alloc_e(-size + 4);
	void *area = mem_alloc_i(size + 4);
	//分区首元素为uint，储存分配信息，地址上的数值为 头地址 + sizeof(uint)
	*(u32 *)area = (u32)area + 4;
	//返回头地址
	return area;
}

//在area的空余处分配size字节大小的元素
//mem_new将使用area的前4字节(uint)来储存分配信息
void *mem_new(void *area, u32 size)
{
	//指向储存分配信息的指针，值是分区开头地址
	u32 *ptInfo = (u32 *)area;
	//指向储存分区信息的指针
	AreaI *areaInfo = (AreaI *)((char *)area - sizeof(AreaI));
	//判断新元素是否越界	
	if (*ptInfo + size > areaInfo->end + IWRAM) return NULL;
	//上一元素结束地址，新元素开始地址
	void *startAddr = (void *)*ptInfo;
	//更新分配信息
	*ptInfo += size;
	//返回新元素开始地址
	return startAddr;
}



void mallocTest()
{
	mem_heap_init();
	//PrintHexInMode3((u32)& __iheap_start, 32, 16);

	int *ar1 = (int *)mem_alloc_i(16);
	//PrintHexInMode3((u32)ar1, 32, 32);
	for (int i = 0; i < 16; i++) ar1[i] = 0x11111111;

	int *ar2 = (int *)mem_alloc_i(32);
	//PrintHexInMode3((u32)ar2, 32, 48);
	for (int i = 0; i < 32; i++) ar2[i] = 0x22222222;

	mem_free_i(ar1);
	//return;

	int *ar3 = (int *)mem_alloc_i(8);
	for (int i = 0; i < 8; i++) ar3[i] = 0x33333333;

	int *ar4 = (int *)mem_alloc_i(16);
	for (int i = 0; i < 16; i++) ar4[i] = 0x44444444;
}

#endif // #ifdef WIN32