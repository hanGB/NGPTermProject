#include "stdafx.h"
#include "IVSSCGlobals.h"
#include "IVSSCGame.h"

void SetGame(HWND hWnd, RECT* rectView, RECT* tect, double* dx, double* dy, int block[][20], double bullet[][4], int* ecolor, double* cx, double* cy, double* seta, int* reload,
	double effect[][17], double reffect[][4], double enemy[][5], int* ecount, int* etime, BOOL* death, int* dcount, int* life, int* score, int* combo
	, int* ten, int* gametime, int level, double sgun[][3], int* scount, BOOL multi)//게임 세팅
{
	GetClientRect(hWnd, rectView);
	if (level == 1)
	{
		*dx = rectView->right / 20;
		*dy = rectView->bottom / 14;
	}
	else if (level == 0)
	{
		*dx = rectView->right / 15;
		*dy = rectView->bottom / 10;
	}
	else if (level == 2)
	{
		*dx = rectView->right / 20;
		*dy = rectView->bottom / 14;
	}
	rectView->top = 2 * (*dy);
	*cx = rectView->right / 2;
	*cy = rectView->bottom / 2;
	if (level == 2)
	{
		*cy = rectView->bottom / 2 + *dy;
	}
	*tect = { 0,0,rectView->right * 1 / 3,rectView->top };
	for (int i = 0; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{

			{
				if (level == 1)
				{
					if (j < 10)
					{
						block[i][j] = 0;
					}
					else
					{
						block[i][j] = 1;
					}
				}
				else if (level == 0)
				{
					if (j < 7)
					{
						block[i][j] = 0;
					}
					else
					{
						block[i][j] = 1;
					}
				}
				else if (level == 2)
				{
					if ((j < 10) && (i == 2 || i == 5 || i == 7 || i == 8 || i == 10 || i == 13))
					{
						block[i][j] = 0;
					}
					else if (j == 0 || j == 3 || j == 4 || j == 7 || j == 8)
					{
						block[i][j] = 0;
					}
					else if ((j == 1 && (i == 3 || i == 4)) || (j == 2 && (i == 3 || i == 4)))
					{
						block[i][j] = 0;
					}
					else if ((j >= 10) && (i == 2 || i == 5 || i == 7 || i == 8 || i == 10 || i == 13))
					{
						block[i][j] = 1;
					}
					else if (j == 11 || j == 12 || j == 15 || j == 16 || j == 19)
					{
						block[i][j] = 1;
					}
					else if ((j == 17 && (i == 11 || i == 12)) || (j == 18 && (i == 11 || i == 12)))
					{
						block[i][j] = 1;
					}
					else
					{
						block[i][j] = 2;
					}
				}
			}
		}
	}
	for (int i = 0; i < 6; i++)
	{
		bullet[i][0] = 0;
	}

	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		reffect[i][0] = 0;
		enemy[i][0] = 0;
	}

	for (int i = 0; i < LIMIT_ENEMY + 1; i++)
	{
		effect[i][0] = 0;
	}

	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		sgun[i][0] = 0;
	}
	*seta = 0;//총알 돌리기 위한 세타
	*reload = 0;//총알 장전

	*ecount = 0; //적 카운트
	*etime = 30; // 적 생성시간

	*death = false;//플레이어 죽음
	*dcount = 0;//부활 카운트
	*life = 3;

	*score = 0;
	*combo = 0;

	*ten = 0;
	*gametime = 0;
	*scount = 0;
	ecolor[0] = rand() % 240 + 10;
	ecolor[1] = rand() % 240 + 10;
	ecolor[2] = rand() % 240 + 10;
	SetTimer(hWnd, 1, 100, NULL);
}

void multireset(player* parray, RECT rectView, double dx, double dy)
{
	for (int i = 0; i < 2; i++)
	{
		for (int k = 0; k < 6; k++)
			parray[i].bullet[k][0] = 0;
	}

	parray[0].cx = dx * 2;
	parray[0].cy = dy * 4;

	parray[1].cx = rectView.right - dx * 2;
	parray[1].cy = rectView.bottom - dy * 2;
}
