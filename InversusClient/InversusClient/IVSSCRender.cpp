#include "stdafx.h"
#include "IVSSCGlobals.h"
#include "IVSSCRender.h"

extern CData clnt_data;

void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct)//그라데이션
{
	int rs, gs, bs;
	rs = GetRValue(start);
	gs = GetGValue(start);
	bs = GetBValue(start);
	int rf, gf, bf;
	rf = GetRValue(finish);
	gf = GetGValue(finish);
	bf = GetBValue(finish);

	TRIVERTEX vert[2];
	GRADIENT_RECT gRect;
	vert[0].x = prect.left;
	vert[0].y = prect.top;
	vert[0].Red = rs * 16 * 16;
	vert[0].Green = gs * 16 * 16;
	vert[0].Blue = bs * 16 * 16;
	vert[0].Alpha = 0x0000;
	vert[1].x = prect.right;
	vert[1].y = prect.bottom;
	vert[1].Red = rf * 16 * 16;
	vert[1].Green = gf * 16 * 16;
	vert[1].Blue = bf * 16 * 16;
	vert[1].Alpha = 0x0000;
	gRect.UpperLeft = 1;
	gRect.LowerRight = 0;
	if (direct)
		GradientFill(pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
	else
		GradientFill(pDC, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_V);
}

void Hcreateboad(int block[][20], double dx, double dy, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH hBrush3, HBRUSH oldBrush, HDC hMemDC)
{//보드판
	for (int i = 2; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (block[i][j] == clnt_data.ci)//흰색
			{
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
			else
			{
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
		}
	}
}

void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC)//보드판 위에 특수총알
{
	for (int i = 0; i < LIMIT_SGUN; i++)
	{
		if (sgun[i][0] == 1)
		{
			int gx, gy;
			gx = dx / 3 * cos(seta + i) + sgun[i][1];
			gy = dy / 3 * sin(seta + i) + sgun[i][2];
			Ellipse(hMemDC, gx - dx / 10, gy - dx / 10, gx + dx / 10, gy + dx / 10);
		}
	}
}

void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, BOOL death, double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC)
{//총알 회전
	rx[i] = dx / 3 * cos(seta + i) + cx;
	ry[i] = dy / 3 * sin(seta + i) + cy;
	if (death == false)
	{
		if (bullet[i][0] == 1)
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
		else
			oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
		Ellipse(hMemDC, rx[i] - dx / 10, ry[i] - dx / 10, rx[i] + dx / 10, ry[i] + dx / 10);
	}
}

void Hrespwan(HPEN OldPen, HBRUSH oldBrush, HBRUSH unBrush, HBRUSH hBrush2, HBRUSH eBrush, HPEN cPen, HPEN ePen, HDC hMemDC, double dx, double dy, double reffect[][4])
{//부활 이펙트
	for (int i = 0; i < LIMIT_ENEMY; i++)
	{
		//부활 이펙트
		if (reffect[i][0] > 0)
		{
			if (reffect[i][3] == -1)
				OldPen = (HPEN)SelectObject(hMemDC, cPen);
			else
				OldPen = (HPEN)SelectObject(hMemDC, ePen);
			oldBrush = (HBRUSH)SelectObject(hMemDC, unBrush);
			Rectangle(hMemDC, reffect[i][1] - dx / 2 - reffect[i][0], reffect[i][2] - dy / 2 - reffect[i][0], reffect[i][1] + dx / 2 + reffect[i][0], reffect[i][2] + dy / 2 + reffect[i][0]);
			if (reffect[i][3] == -1)
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
			else
				oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
			Rectangle(hMemDC, reffect[i][1] - dx / 2 + reffect[i][0], reffect[i][2] - dy / 2 + reffect[i][0], reffect[i][1] + dx / 2 - reffect[i][0], reffect[i][2] + dy / 2 - reffect[i][0]);
		}
	}
}

void Hdeatheffect(double effect[][17], HBRUSH oldBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC)//데스이펙트
{
	for (int i = 0; i < LIMIT_ENEMY + 1; i++)
	{
		if (i == 0)
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush2);
		else
			oldBrush = (HBRUSH)SelectObject(hMemDC, eBrush);
		if (effect[i][0] > 0)
		{
			Ellipse(hMemDC, effect[i][1] - effect[i][9], effect[i][2] - effect[i][9], effect[i][1] + effect[i][9], effect[i][2] + effect[i][9]);
			Ellipse(hMemDC, effect[i][3] - effect[i][10], effect[i][4] - effect[i][10], effect[i][3] + effect[i][10], effect[i][4] + effect[i][10]);
			Ellipse(hMemDC, effect[i][5] - effect[i][11], effect[i][6] - effect[i][11], effect[i][5] + effect[i][11], effect[i][6] + effect[i][11]);
			Ellipse(hMemDC, effect[i][7] - effect[i][12], effect[i][8] - effect[i][12], effect[i][7] + effect[i][12], effect[i][8] + effect[i][12]);
		}
	}
}

void Hscorebord(HBRUSH oldBrush, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC, RECT rectView, double dx, double dy, int life, HFONT Font, HFONT OldFont, char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi)
{//상단 스코어보드판
	oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
	Rectangle(hMemDC, 0, 0, rectView.right, rectView.top);
	lf->lfHeight = dy;
	OldFont = (HFONT)SelectObject(hMemDC, Font);

	{
		wsprintf(str, "versus mode");
		DrawText(hMemDC, str, -1, &tect, DT_LEFT);
	}
}

void Hgamewin(HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd)//일대일 승패 메세지
{
	SetBkColor(hMemDC, RGB(255, 255, 255));
	RECT aect = { rectView.left,rectView.bottom * 1 / 4,rectView.right,rectView.bottom * 3 / 4 };
	if (check == 0)
		wsprintf(str, "Player BLACK WIN\nPress Enter key to reset game");
	else
		wsprintf(str, "Player WHITE WIN\nPress Enter key to reset game");
	DrawText(hMemDC, str, -1, &aect, DT_CENTER);
	KillTimer(hWnd, 1);
}

void Hgameover(HDC hMemDC, RECT rectView, int score, char* str, HWND hWnd)//게임오버메세지
{
	SetBkColor(hMemDC, RGB(255, 255, 255));
	RECT aect = { rectView.left,rectView.bottom * 1 / 4,rectView.right,rectView.bottom * 3 / 4 };
	wsprintf(str, "Game Over\n%d\nPress Enter key to reset game", score);
	DrawText(hMemDC, str, -1, &aect, DT_CENTER);
	KillTimer(hWnd, 1);
}
