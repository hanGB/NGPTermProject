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
	ecolor[0] = 125;
	ecolor[1] = 125;
	ecolor[2] = 125;
	SetTimer(hWnd, 1, 100, NULL);
}
