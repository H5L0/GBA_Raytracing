#include <hl_input.h>
#include <hl_debug.h>
#include <hl_timers.h>
#include "RT_base.h"
#include "RT_define.h"

void RayTracing(Scene *sc, int targetSize)
{
	Ray ray;
	Color pixel;
	//ֻ��Ⱦλ�����������Ͻǵ�����
	SetMosaic(targetSize);
	for (int y = 0; y < SCREEN_Y; y += targetSize)
	{
		for (int x = 0; x < SCREEN_X; x += targetSize)
		{
			//�ڸ������»���һ�����أ��γ�һ������
			//SetPixel3(x, y + 1, 0);
			ScreenToRay(sc->camera, &ray, x, y);
			pixel = Trace(sc, &ray);
			SetPixel3(x, y, rgb5_color16(&pixel));
			//***����16λ��ɫ
			//SetPixel3(x, y, &pixel);
		}
	}
}

void RayTracing_Adv(Scene *sc, int targetSize)
{
	Ray ray;
	Color pixel;
	int x, y;
	int lastSize = 0;
	for (int psize = 16; psize > targetSize - 1; psize >>= 1)
	{
		rgb5 c;
		if (lastSize > 0)
		{
			for (y = 0; y < SCREEN_Y; y += lastSize)
			{
				for (x = 0; x < SCREEN_X; x += lastSize)
				{
					c = GetPixel3(x, y);
					SetPixel3(x + psize, y, c);
					SetPixel3(x, y + psize, c);
					SetPixel3(x + psize, y + psize, c);
				}
			}
		}

		SetMosaic(psize);
		for (y = 0; y < SCREEN_Y; y += psize)
		{
			for (x = 0; x < SCREEN_X; x += psize)
			{
				//��������Ⱦ�㣬(mosSize - 1)Ϊ1111...
				if (lastSize && !(x & (lastSize - 1)) && !(y & (lastSize - 1))) continue;
				//����ָʾ�ڵ�
				SetPixel3(x, y, 0);
				ScreenToRay(sc->camera, &ray, x, y);
				pixel = Trace(sc, &ray);
				SetPixel3(x, y, rgb5_color16(&pixel));
			}
		}
		lastSize = psize;
	}
}

void RayTracing_Ani(Scene *sc, int targetSize)
{
	int x, y;
	Ray ray;
	Color pixel;
	SetMosaic(targetSize);
	for (y = 0; y < SCREEN_Y; y += targetSize)
	{
		for (x = 0; x < SCREEN_X; x += targetSize)
		{
			ScreenToRay(sc->camera, &ray, x, y);
			pixel = Trace(sc, &ray);
			SetPixel3(x + 1, y, rgb5_color16(&pixel));
		}
	}

	for (y = 0; y < SCREEN_Y; y += targetSize)
	{
		for (x = 0; x < SCREEN_X; x += targetSize)
		{
			SetPixel3(x, y, GetPixel3(x + 1, y));
		}
	}
}

void RayTracing_Pattern(Scene *sc, int targetSize)
{
	Ray ray;
	Color pixel;
	int x, y;
	int lastSize = 16;
	for (int psize = 16; psize > targetSize - 1; psize >>= 1)
	{
		SetMosaic(psize);
		for (y = (psize & 16) ? 0 : psize; y < SCREEN_Y; y += lastSize)
		{
			for (x = 0; x < SCREEN_X; x++)
			{
				//����ָʾ�ڵ�
				//SetPixel3(x, y, 0);
				ScreenToRay(sc->camera, &ray, x, y);
				pixel = Trace(sc, &ray);
				SetPixel3(x, y, rgb5_color16(&pixel));
			}
		}
		lastSize = psize;
	}
}

void RayTracing_Fly(Scene *sc, int targetSize)
{
	Ray ray;
	Color pixel;
	rgb5 rgb;
	int x, y;
	int lastSize = 0;

	InitTargetController(sc);
	//InitFPSController(sc);

	//�����￪ʼ������Ⱦ
	lb_restart:;

	SetMosaic(16);
	//�����ƶ�
	TargetControllerUpdate(sc);
	//FPSControllerUpdate(sc);

	ResetTestTimer();

	//����Ԥ��
	for (y = 0; y < SCREEN_Y; y += 16)
	{
		for (x = 0; x < SCREEN_X; x += 16)
		{
			pixel = Trace(sc, ScreenToRay(sc->camera, &ray, x, y));
			rgb = rgb5_color16(&pixel);
			SetPixel3(x, y, rgb);
			//SetPixel3(x + 16, y, rgb);
			//SetPixel3(x, y + 16, rgb);
			//SetPixel3(x + 16, y + 16, rgb);
		}
	}
	if (NeedUpdateController()) goto lb_restart;

	//������Ⱦ
	for (int psize = 8; psize > targetSize - 1; psize >>= 1)
	{
		lastSize = psize << 1;
		//���������
		for (y = 0; y < SCREEN_Y; y += lastSize)
		{
			for (x = 0; x < SCREEN_X; x += lastSize)
			{
				rgb = GetPixel3(x, y);
				SetPixel3(x + psize, y, rgb);
				SetPixel3(x, y + psize, rgb);
				SetPixel3(x + psize, y + psize, rgb);
			}
		}

		SetMosaic(psize);
		for (y = 0; y < SCREEN_Y; y += psize)
		{
			//if (psize == 1)PrintFP32InMode3(GetTestTimer(), 8, 150);
			for (x = 0; x < SCREEN_X; x += psize)
			{
				if (NeedUpdateController()) goto lb_restart;

				//��������Ⱦ�㣬(mosSize - 1)Ϊ1111...
				if (lastSize != 16 && !(x & (lastSize - 1)) && !(y & (lastSize - 1))) continue;
				SetPixel3(x, y, 0);
				pixel = Trace(sc, ScreenToRay(sc->camera, &ray, x, y));
				SetPixel3(x, y, rgb5_color16(&pixel));
			}
			//PrintFP32InMode3(GetTestTimer(), 8, 150);
		}
	}

	PrintFP32InMode3(GetTestTimer(), 8, 150);
	while (!NeedUpdateController());
	goto lb_restart;
}


void RayTracing_16(Scene *sc, int line_start, int line_end)
{
	Ray ray;
	Color pixel;
	SetMosaic(1);
	for (int y = line_start; y < line_end; y++)
	{
		for (int x = 0; x < SCREEN_X; x++)
		{
			ScreenToRay(sc->camera, &ray, x, y);
			pixel = Trace(sc, &ray);
			SetPixel_16(x, y, &pixel);
		}
	}
}