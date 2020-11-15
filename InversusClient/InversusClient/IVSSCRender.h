#pragma once

//그라데이션
void DrawGr(HDC pDC, COLORREF start, COLORREF finish, RECT prect, BOOL direct); 

//보드판
void Hcreateboad(int block[][20], double dx, double dy, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH hBrush3, 
	HBRUSH oldBrush, HDC hMemDC);

//보드판 위에 특수총알
void Hsgun(double sgun[][3], double seta, double dx, double dy, HDC hMemDC);

//총알 회전
void Hrotategun(double* rx, double* ry, double cx, double cy, double dx, double dy, double seta, int i, BOOL death,
	double bullet[][4], HBRUSH oldBrush, HBRUSH hBrush, HBRUSH eBrush, HDC hMemDC);

//부활 이펙트
void Hrespwan(HPEN OldPen, HBRUSH oldBrush, HBRUSH unBrush, HBRUSH hBrush2, HBRUSH eBrush, 
	HPEN cPen, HPEN ePen, HDC hMemDC, double dx, double dy, double reffect[][4]);

//데스이펙트
void Hdeatheffect(HBRUSH oldBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC);

//상단 스코어 보드판
void Hscorebord(HBRUSH oldBrush, HBRUSH hBrush, HBRUSH hBrush2, HBRUSH eBrush, HDC hMemDC, RECT rectView, 
	double dx, double dy, int life, HFONT Font, HFONT OldFont, 
	char* str, int score, int combo, RECT tect, LOGFONT* lf, BOOL multi);

//일대일 승패 메세지
void Hgamewin(HDC hMemDC, RECT rectView, int check, char* str, HWND hWnd);

//게임 오버 메세지
void Hgameover(HDC hMemDC, RECT rectView, int score, char* str, HWND hWnd);
