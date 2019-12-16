#ifndef WIN32

#include <hl_memory.h>
#include <hl_gba.h>

#include <hl_graphics.h>	//**����


rgb5 _mem_fill_color = 0;
void DrawIWRAMAllocMap(u32 start, u32 size)
{
	//32KB,32*1024 Bytes = 128*128*2 Btyes
	//128�У�ÿ��256B��128������ʾ 2Byte/pixel
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


//��EWROM�Ķ��з��� size(�ֽ�) �ڴ�
void *mem_alloc_e(u32 size)
{
	if (size == 0) return 0;
	size = ((size + sizeof(AreaE) + 3) >> 2) << 2;

	AreaE *ta = (AreaE *)&__eheap_start;
	while ((u32)&__eheap_end - (u32)ta - ta->size >= size)
	{
		//���һ�������ҿռ���� / �������ռ����
		if (ta->nextArea == NULL || (u32)ta->nextArea - (u32)ta - ta->size >= size)
		{
			//��һ������
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
		//�����������һ���Ҽ���ռ䲻��
		ta = ta->nextArea;
	}
	return 0;
}

//�ͷ���EWROM������ڴ�
int mem_free_e(void *addr)
{
	AreaE *ta = (AreaE *)((u32)addr - sizeof(AreaE));
	//if (ta < &__eheap_start || ta > &__eheap_end) return 0;
	if (ta->lastArea == NULL)
	{
		//��һ������,�����nextArea;
		mem_clean(addr, ta->size - sizeof(AreaE));
		ta->size = NULL_HEAD_FLAG;
		//��������һ�������һ�����¼
	}
	else
	{
		ta->lastArea->nextArea = ta->nextArea;
		if (ta->nextArea != NULL) ta->nextArea->lastArea = ta->lastArea;
		mem_clean((void *)ta, ta->size);
	}
	return addr;
}

//��IWRAM�з��� size(�ֽ�) �Ŀռ�
//0������ʧ��
void *mem_alloc_i(u32 size)
{
	if (size == 0) return 0;
	size = ((size + sizeof(AreaI) + 3) >> 2) << 2;

	AreaI *ta = (AreaI *)&__iheap_start;
	while (iheap_end_addr - (ta->end + IWRAM) > size)
	{
		//���һ�������ҿռ���� / �������ռ����
		if (ta->next == NULL || ta->next - ta->end >= size)
		{
			//��һ������
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
		//�����������һ���Ҽ���ռ䲻��
		ta = (AreaI *)(ta->next + IWRAM);
	}
	return 0;
}

//�ͷ���IWRAM�з�����ڴ�
//0���ͷ�ʧ��
int mem_free_i(void *addr)
{
	AreaI *ta = (AreaI *)((u32)addr - sizeof(AreaI));
	AreaI *ia = (AreaI *)&__iheap_start;
	AreaI *la;
	//��һ������, �����next
	if (ta == ia)
	{
		mem_clean(addr, ta->end + IWRAM - (u32)ta - sizeof(AreaI));
		ta->end = NULL_HEAD_FLAG;
		return addr;
	}
	do
	{
		//��ia��Ѱ��һ����
		la = ia;
		ia = (AreaI *)(ia->next + IWRAM);
		//��Ѱ��ta����laΪ��һ����
		if (ia == ta)
		{
			la->next = ta->next;
			mem_clean((void *)ta, ta->end + IWRAM - (u32)ta);
			return addr;
		}
	} while ((u32)ia < iheap_end_addr);

	return 0;	//��ѰԽ��,�ͷ�ʧ��
}


//��adr~adr+len���ڴ����� (char)
void *mem_clean(void *start_adr, int clen)
{
	int *adr = start_adr;
	int ilen = clen >> 2;
	clen -= ilen << 2;
	while ((ilen--) > 0) *(adr++) = 0;
	while ((ilen--) > 0) *((char *)adr++) = 0;
	return start_adr;
}


//����size+4�ֽڵ��ڴ棬�����Զ���������
void *mem_alloc_new(u32 size)
{
	//void *area = size > 0 ? mem_alloc_i(size + 4) : mem_alloc_e(-size + 4);
	void *area = mem_alloc_i(size + 4);
	//������Ԫ��Ϊuint�����������Ϣ����ַ�ϵ���ֵΪ ͷ��ַ + sizeof(uint)
	*(u32 *)area = (u32)area + 4;
	//����ͷ��ַ
	return area;
}

//��area�Ŀ��ദ����size�ֽڴ�С��Ԫ��
//mem_new��ʹ��area��ǰ4�ֽ�(uint)�����������Ϣ
void *mem_new(void *area, u32 size)
{
	//ָ�򴢴������Ϣ��ָ�룬ֵ�Ƿ�����ͷ��ַ
	u32 *ptInfo = (u32 *)area;
	//ָ�򴢴������Ϣ��ָ��
	AreaI *areaInfo = (AreaI *)((char *)area - sizeof(AreaI));
	//�ж���Ԫ���Ƿ�Խ��	
	if (*ptInfo + size > areaInfo->end + IWRAM) return NULL;
	//��һԪ�ؽ�����ַ����Ԫ�ؿ�ʼ��ַ
	void *startAddr = (void *)*ptInfo;
	//���·�����Ϣ
	*ptInfo += size;
	//������Ԫ�ؿ�ʼ��ַ
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