#include "stdafx.h"
#include "IVSSCGlobals.h"
#include "IVSSCRender.h"

extern CData clnt_data;
extern player parray[MAX_PLAYER];

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

void Hcreateboad(int block[][20], double dx, double dy, HDC hMemDC)
{//보드판
	HBRUSH hBrush, oldBrush;
	HPEN hPen, oldPen;

	hPen = CreatePen(PS_SOLID, 3, RGB(125, 125, 125));//보드
	oldPen = (HPEN)SelectObject(hMemDC, hPen);

	for (int i = 2; i < 20; i++)
	{
		for (int j = 0; j < 20; j++)
		{
			if (block[i][j] == clnt_data.ci)//흰색
			{
				hBrush = CreateSolidBrush(RGB(255, 255, 255));//흰색
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
			else
			{
				hBrush = CreateSolidBrush(RGB(0, 0, 0));//검정
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				Rectangle(hMemDC, dx * j, dy * i, dx * (j + 1), dy * (i + 1));
			}
			SelectObject(hMemDC, oldBrush);
			DeleteObject(hBrush);
		}
	}
	SelectObject(hMemDC, oldPen);
	DeleteObject(hPen);
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

void Hrespwan(HDC hMemDC, int* ecolor, double dx, double dy)
{//부활 이펙트
	HPEN hPen, oldPen;
	HBRUSH hBrush, oldBrush;

	for (int id = 0; id < MAX_PLAYER + 1; id++)
	{
		if (parray[id].death) {
			//부활 이펙트
			if (parray[id].reffect[0] > 0)
			{
				if (parray[id].nu == clnt_data.ci) {
					hPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));//주인공
					oldPen = (HPEN)SelectObject(hMemDC, hPen);
				}
				else {
					hPen = CreatePen(PS_SOLID, 3, RGB(ecolor[0], ecolor[1], ecolor[2]));//적
					oldPen = (HPEN)SelectObject(hMemDC, hPen);
				}
				hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);//투명
				oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

				Rectangle(hMemDC, parray[id].reffect[1] - dx / 2 - parray[id].reffect[0] * 30,
					parray[id].reffect[2] - dy / 2 - parray[id].reffect[0] * 30,
					parray[id].reffect[1] + dx / 2 + parray[id].reffect[0] * 30,
					parray[id].reffect[2] + dy / 2 + parray[id].reffect[0] * 30);

				SelectObject(hMemDC, oldBrush);
				DeleteObject(hBrush);

				if (parray[id].nu == clnt_data.ci) {
					hBrush = CreateSolidBrush(RGB(0, 0, 0));//검정
					oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				}
				else {
					hBrush = CreateSolidBrush(RGB(ecolor[0], ecolor[1], ecolor[2]));//적
					oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
				}
				Rectangle(hMemDC, parray[id].reffect[1] - dx / 2 + parray[id].reffect[0] * 30,
					parray[id].reffect[2] - dy / 2 + parray[id].reffect[0] * 30,
					parray[id].reffect[1] + dx / 2 - parray[id].reffect[0] * 30,
					parray[id].reffect[2] + dy / 2 - parray[id].reffect[0] * 30);

				SelectObject(hMemDC, oldBrush);
				DeleteObject(hBrush);
				SelectObject(hMemDC, oldPen);
				DeleteObject(hPen);
			}
		}
	}
}

void Hdeatheffect(HDC hMemDC, int* ecolor)//데스이펙트
{
	HBRUSH hBrush, oldBrush;

	for (int id = 0; id < MAX_PLAYER + 1; id++)
	{
		if (id == clnt_data.ci) {
			hBrush = CreateSolidBrush(RGB(0, 0, 0));//검정
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
		}
		else {
			hBrush = CreateSolidBrush(RGB(ecolor[0], ecolor[1], ecolor[2]));//적
			oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);
		}
		if (parray[id].d_effect[0] > 0)
		{
			Ellipse(hMemDC, parray[id].d_effect[1] - parray[id].d_effect[9], parray[id].d_effect[2] - parray[id].d_effect[9], parray[id].d_effect[1] + parray[id].d_effect[9], parray[id].d_effect[2] + parray[id].d_effect[9]);
			Ellipse(hMemDC, parray[id].d_effect[3] - parray[id].d_effect[10], parray[id].d_effect[4] - parray[id].d_effect[10], parray[id].d_effect[3] + parray[id].d_effect[10], parray[id].d_effect[4] + parray[id].d_effect[10]);
			Ellipse(hMemDC, parray[id].d_effect[5] - parray[id].d_effect[11], parray[id].d_effect[6] - parray[id].d_effect[11], parray[id].d_effect[5] + parray[id].d_effect[11], parray[id].d_effect[6] + parray[id].d_effect[11]);
			Ellipse(hMemDC, parray[id].d_effect[7] - parray[id].d_effect[12], parray[id].d_effect[8] - parray[id].d_effect[12], parray[id].d_effect[7] + parray[id].d_effect[12], parray[id].d_effect[8] + parray[id].d_effect[12]);
		}
		SelectObject(hMemDC, oldBrush);
		DeleteObject(hBrush);
	}
}

void Hscorebord(HDC hMemDC, RECT rectView, double dx, double dy, int life, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi)
{//상단 스코어보드판
	HBRUSH hBrush, oldBrush;
	HFONT hFont, oldFont;

	hBrush = CreateSolidBrush(RGB(255, 255, 255));//흰색
	oldBrush = (HBRUSH)SelectObject(hMemDC, hBrush);

	Rectangle(hMemDC, 0, 0, rectView.right, rectView.top);

	SelectObject(hMemDC, oldBrush);
	DeleteObject(hBrush);

	lf->lfHeight = dy;
	hFont = CreateFontIndirect(lf);
	oldFont = (HFONT)SelectObject(hMemDC, hFont);

	wsprintf(str, "versus mode");
	DrawText(hMemDC, str, -1, &tect, DT_LEFT);

	SelectObject(hMemDC, oldFont);
	DeleteObject(hFont);
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
