#include "stdafx.h"
#include "IVSSSGlobals.h"
#include "IVSSSGame.h"

void SetGame(HWND hWnd, RECT* rectView, RECT* tect, float* dx, float* dy, int block[][20], float bullet[][4], int* ecolor, float* cx, float* cy, float* seta, int* reload,
	float effect[][17], float reffect[][4], float enemy[][5], int* ecount, int* etime, BOOL* death, int* dcount, int* life, int* score, int* combo
	, int* ten, int* gametime, int level, float sgun[][3], int* scount, BOOL multi)//게임 세팅
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
	ecolor[0] = rand() % 240 + 10;
	ecolor[1] = rand() % 240 + 10;
	ecolor[2] = rand() % 240 + 10;
	SetTimer(hWnd, 1, 100, NULL);
}
